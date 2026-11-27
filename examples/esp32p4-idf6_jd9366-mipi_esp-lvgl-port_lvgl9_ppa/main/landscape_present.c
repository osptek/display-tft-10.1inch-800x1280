#include "landscape_present.h"

#include <string.h>
#include "driver/ppa.h"
#include "esp_attr.h"
#include "esp_cache.h"
#include "esp_heap_caps.h"
#include "esp_lcd_mipi_dsi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

enum {
    SCAN_BUFFER_COUNT = 2,
    // DMA 重启与提交可能并发；回调不报告缓冲编号。
    RELEASE_BOUNDARIES = 2,
    FRAME_TIMEOUT_MS = 500,
    CACHE_ALIGNMENT = 128,
};

static struct {
    esp_lcd_panel_handle_t panel;
    esp_lcd_touch_handle_t touch;
    ppa_client_handle_t ppa;
    SemaphoreHandle_t boundaries;
    void *scan[SCAN_BUFFER_COUNT];
    lv_area_t dirty[SCAN_BUFFER_COUNT];
    bool pending[SCAN_BUFFER_COUNT];
    unsigned back;
    bool awaiting_release;
    int32_t width;
    int32_t height;
    size_t stride;
    size_t bytes;
} present;

static bool IRAM_ATTR frame_complete(esp_lcd_panel_handle_t panel,
                                     esp_lcd_dpi_panel_event_data_t *event, void *ctx)
{
    BaseType_t wake = pdFALSE;
    xSemaphoreGiveFromISR(present.boundaries, &wake);
    return wake == pdTRUE;
}

static void flush(lv_display_t *display, const lv_area_t *area, uint8_t *pixels)
{
    // 后备缓冲可能缺失两次提交之间的更新，两张缓冲必须分别累积脏区。
    for (unsigned i = 0; i < SCAN_BUFFER_COUNT; ++i) {
        if (present.pending[i]) {
            lv_area_t *dirty = &present.dirty[i];
            dirty->x1 = LV_MIN(dirty->x1, area->x1);
            dirty->y1 = LV_MIN(dirty->y1, area->y1);
            dirty->x2 = LV_MAX(dirty->x2, area->x2);
            dirty->y2 = LV_MAX(dirty->y2, area->y2);
        } else {
            present.dirty[i] = *area;
            present.pending[i] = true;
        }
    }
    if (!lv_display_flush_is_last(display)) {
        lv_display_flush_ready(display);
        return;
    }

    const unsigned back = present.back;
    const lv_area_t rect = present.dirty[back];
    const ppa_srm_oper_config_t op = {
        .in = {
            .buffer = pixels,
            .pic_w = present.width,
            .pic_h = present.height,
            .block_offset_x = rect.x1,
            .block_offset_y = rect.y1,
            .block_w = lv_area_get_width(&rect),
            .block_h = lv_area_get_height(&rect),
            .srm_cm = PPA_SRM_COLOR_MODE_RGB565,
        },
        .out = {
            .buffer = present.scan[back],
            .buffer_size = present.bytes,
            .pic_w = present.height,
            .pic_h = present.width,
            .block_offset_x = rect.y1,
            .block_offset_y = present.width - 1 - rect.x2,
            .srm_cm = PPA_SRM_COLOR_MODE_RGB565,
        },
        .rotation_angle = PPA_SRM_ROTATION_ANGLE_90,
        .scale_x = 1.0f,
        .scale_y = 1.0f,
        .mode = PPA_TRANS_MODE_BLOCKING,
    };
    ESP_ERROR_CHECK(esp_cache_msync(pixels + rect.y1 * present.stride,
                                  op.in.block_h * present.stride, ESP_CACHE_MSYNC_FLAG_DIR_C2M));

    // 独立的逻辑画布可继续绘制；仅在即将重用旧扫描缓冲前等待。
    if (present.awaiting_release) {
        for (unsigned i = 0; i < RELEASE_BOUNDARIES; ++i) {
            ESP_ERROR_CHECK(xSemaphoreTake(present.boundaries, pdMS_TO_TICKS(FRAME_TIMEOUT_MS)) == pdTRUE
                            ? ESP_OK : ESP_ERR_TIMEOUT);
        }
    }
    ESP_ERROR_CHECK(ppa_do_scale_rotate_mirror(present.ppa, &op));
    // DPI 提交会回写 CPU 缓存；先失效旧内容，保留 PPA 的 DMA 输出。
    ESP_ERROR_CHECK(esp_cache_msync(present.scan[back], present.bytes, ESP_CACHE_MSYNC_FLAG_DIR_M2C));
    ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(present.panel, 0, 0, present.height, present.width,
                                            present.scan[back]));
    xQueueReset(present.boundaries);
    present.awaiting_release = true;
    present.pending[back] = false;
    present.back ^= 1;
    lv_display_flush_ready(display);
}

static void read_touch(lv_indev_t *indev, lv_indev_data_t *data)
{
    esp_lcd_touch_point_data_t point = {0};
    uint8_t count = 0;
    data->state = LV_INDEV_STATE_RELEASED;
    if (esp_lcd_touch_read_data(present.touch) != ESP_OK ||
        esp_lcd_touch_get_data(present.touch, &point, &count, 1) != ESP_OK || count == 0) {
        return;
    }
    const int32_t x = LV_MIN(point.x, present.height - 1);
    const int32_t y = LV_MIN(point.y, present.width - 1);
    // 逆变换与 PPA 的 90° 方向一致。
    data->point.x = present.width - 1 - y;
    data->point.y = x;
    data->state = LV_INDEV_STATE_PRESSED;
}

lv_display_t *landscape_present_create(esp_lcd_panel_handle_t panel,
                                      esp_lcd_touch_handle_t touch,
                                      int32_t native_width, int32_t native_height)
{
    present.panel = panel;
    present.touch = touch;
    present.width = native_height;
    present.height = native_width;
    present.stride = (size_t)native_height * sizeof(uint16_t);
    present.bytes = present.stride * native_width;
    present.back = 1; // DPI 启动时扫描第 0 张。
    ESP_ERROR_CHECK(esp_lcd_dpi_panel_get_frame_buffer(panel, SCAN_BUFFER_COUNT,
                                                     &present.scan[0], &present.scan[1]));
    present.boundaries = xSemaphoreCreateCounting(RELEASE_BOUNDARIES, 0);
    ESP_ERROR_CHECK(present.boundaries ? ESP_OK : ESP_ERR_NO_MEM);
    const ppa_client_config_t ppa_config = { .oper_type = PPA_OPERATION_SRM };
    ESP_ERROR_CHECK(ppa_register_client(&ppa_config, &present.ppa));

    uint8_t *canvas = heap_caps_aligned_alloc(CACHE_ALIGNMENT, present.bytes,
                                              MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    ESP_ERROR_CHECK(canvas ? ESP_OK : ESP_ERR_NO_MEM);
    memset(canvas, 0, present.bytes);
    for (unsigned i = 0; i < SCAN_BUFFER_COUNT; ++i) {
        present.dirty[i] = (lv_area_t){0, 0, present.width - 1, present.height - 1};
        present.pending[i] = true;
    }

    const esp_lcd_dpi_panel_event_callbacks_t callbacks = { .on_frame_buf_complete = frame_complete };
    ESP_ERROR_CHECK(esp_lcd_dpi_panel_register_event_callbacks(panel, &callbacks, NULL));
    lv_display_t *display = lv_display_create(present.width, present.height);
    ESP_ERROR_CHECK(display ? ESP_OK : ESP_ERR_NO_MEM);
    lv_display_set_color_format(display, LV_COLOR_FORMAT_RGB565);
    lv_display_set_buffers(display, canvas, NULL, present.bytes, LV_DISPLAY_RENDER_MODE_DIRECT);
    lv_display_set_flush_cb(display, flush);

    lv_indev_t *input = lv_indev_create();
    ESP_ERROR_CHECK(input ? ESP_OK : ESP_ERR_NO_MEM);
    lv_indev_set_type(input, LV_INDEV_TYPE_POINTER);
    lv_indev_set_display(input, display);
    lv_indev_set_read_cb(input, read_touch);
    return display;
}
