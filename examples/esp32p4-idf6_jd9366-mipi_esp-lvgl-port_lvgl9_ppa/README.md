# JD9366 MIPI DSI + LVGL 9 on ESP32-P4

This example runs the LVGL widgets demo in landscape on the 800 × 1280
JD9366 panel. Before flashing while the board is powered, press BOOT to
turn off the display, then enter download mode.

## Optional RGB565 presentation path

On ESP-IDF 6.1, enable **JD9366 example → Use double-buffered PPA landscape
presentation (RGB565)** in `idf.py menuconfig`, then run `idf.py build`.
The option is off by default, so the original `esp_lvgl_port` path remains
available for comparison. Keep LVGL color depth at 16 and PSRAM enabled.
The logical DIRECT canvas and two DPI framebuffers take 6,144,000 bytes
in total. The panel timing and initialization are unchanged.

Each scan buffer accumulates its own dirty rectangle, including changes
rendered since that buffer was last submitted. After the last LVGL flush
area, PPA rotates the required rectangle into the back buffer. Distant
updates may expand that rectangle, and a full redraw still rotates the
whole frame.

DPI's completion callback follows the DMA restart and does not identify
which buffer was just released. The example therefore waits for two
boundaries after submission before writing to the old buffer. This wait
takes place at reuse, so those boundaries can pass while LVGL draws or
is idle. LVGL can reuse the independent logical canvas once PPA returns.

To check the integration on hardware, open the widgets demo, confirm
touch mapping near all four corners, then switch tabs and drag a slider.
Rapidly alternate edits in separate areas and look for old pixels
reappearing or tearing. Also check the BOOT display toggle and monitor
for scan-buffer timeouts.

## Reference measurements

These measurements are from a separate validation firmware on an OSPTEK
P4C5 rev3.2 with ESP-IDF 6.1 and LVGL 9.6. They compare the previous
full-frame PPA path (two boundaries waited after submission) with the
dirty-rectangle algorithm above. They are **not** measurements of this
example or its default `esp_lvgl_port` path.

Both workloads ran at a 250 ms interval: four warm-up frames followed by
60 measured frames per configuration, with touch and other periodic UI
updates suspended. The digit changed a 15,660-pixel LVGL dirty area; the
page switch changed the full 1,024,000-pixel canvas. The table reports
nearest-rank P95 (frame 57 of 60 after sorting), in ms. See the
[240 per-frame samples](evidence/landscape-perf.csv) for the raw values.

| Phase | Single digit before | Single digit after | Page switch before | Page switch after |
| --- | ---: | ---: | ---: | ---: |
| LVGL drawing | 0.976 | 0.978 | 54.920 | 55.226 |
| PPA rotation | 25.118 | 0.536 | 25.826 | 25.810 |
| Scan-buffer wait | 32.239 | 0.003 | 31.040 | 0.004 |
| Entire LVGL refresh | 58.627 | 2.038 | 106.404 | 82.112 |

On that same firmware, alternating updates to two pressure readouts
produced 12 pixel-exact comparisons between the submitted buffer and
the rotated reference canvas, six in each landscape orientation. This
validates buffer contents. Optical latency and sustained updates fast
enough to require a scan-buffer wait were not measured.
