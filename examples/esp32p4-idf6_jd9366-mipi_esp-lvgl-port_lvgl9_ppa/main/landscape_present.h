#pragma once

#include "esp_lcd_panel_ops.h"
#include "esp_lcd_touch.h"
#include "lvgl.h"

// 调用方须持有 LVGL 锁，且 DPI 面板须有两张 RGB565 扫描缓冲。
lv_display_t *landscape_present_create(esp_lcd_panel_handle_t panel,
                                      esp_lcd_touch_handle_t touch,
                                      int32_t native_width, int32_t native_height);
