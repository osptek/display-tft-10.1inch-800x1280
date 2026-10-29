# 示例工程 / Sample Projects

本目录收录本规格屏幕的示例工程，每个子文件夹是一个独立工程，打开对应文件夹按其中的 README 编译即可。
这些工程是参考代码，引脚、背光、触摸等配置请按你的硬件修改；选用前请对照仓库首页各型号的驱动 IC 和接口确认。

This folder holds sample projects for this display spec. Each sub-folder is a standalone project — open it and follow its own README to build.
They are reference code; adjust pins, backlight, touch and other settings to your hardware. Before choosing one, check the driver IC and interface against the models listed on the repository home page.

| 例程 / Project | 平台 / Platform | 驱动 IC / Driver IC | 接口 / Interface | 说明 / Description |
|---|---|---|---|---|
| [`10.1寸JD9366-MIPI`](./10.1寸JD9366-MIPI/) | Rockchip RK3506 · Linux | JD9366 | MIPI | 设备树与触摸驱动参考、I2C 触摸驱动添加指南 / Device tree and touch driver reference, I2C touch driver guide |
| [`esp32p4-idf5-jd9366-mipi-lvgl-port-lvgl9`](./esp32p4-idf5-jd9366-mipi-lvgl-port-lvgl9/) | ESP32-P4 · ESP-IDF 5 | JD9366 | MIPI | LVGL 9 / LVGL 9 |
| [`esp32p4-idf5-jd9366-mipi-lvgl-port-lvgl9-ppa`](./esp32p4-idf5-jd9366-mipi-lvgl-port-lvgl9-ppa/) | ESP32-P4 · ESP-IDF 5 | JD9366 | MIPI | LVGL 9、PPA 硬件加速 / LVGL 9, PPA acceleration |
| [`esp32p4-idf6-jd9366-mipi-lvgl-port-lvgl9`](./esp32p4-idf6-jd9366-mipi-lvgl-port-lvgl9/) | ESP32-P4 · ESP-IDF 6 | JD9366 | MIPI | LVGL 9 / LVGL 9 |
| [`esp32p4-idf6-jd9366-mipi-lvgl-port-lvgl9-ppa`](./esp32p4-idf6-jd9366-mipi-lvgl-port-lvgl9-ppa/) | ESP32-P4 · ESP-IDF 6 | JD9366 | MIPI | LVGL 9、PPA 硬件加速 / LVGL 9, PPA acceleration |
| [`rpi5-lvgl-jd9366-touch-800x1280`](./rpi5-lvgl-jd9366-touch-800x1280/) | Raspberry Pi 5 | JD9366 | MIPI DSI | LVGL、触摸 / LVGL, touch |
| [`rpi5-panel-jd9366-800x1280`](./rpi5-panel-jd9366-800x1280/) | Raspberry Pi 5 | JD9366 | MIPI DSI | 面板驱动（仅显示）/ panel driver (display only) |
| [`rpi5-panel-jd9366-touch-800x1280`](./rpi5-panel-jd9366-touch-800x1280/) | Raspberry Pi 5 | JD9366 | MIPI DSI | 面板驱动、触摸 / panel driver, touch |

如有问题请联系技术支持。 / Contact technical support if you have questions.
