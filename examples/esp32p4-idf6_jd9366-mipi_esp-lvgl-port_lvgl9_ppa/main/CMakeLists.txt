idf_component_register(SRCS "main.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES esp_lcd esp_driver_gpio esp_driver_i2c esp_hw_support
                                     esp_lcd_jd9366 esp_lcd_touch_jd9366 esp_driver_ppa esp_mm)

if(CONFIG_EXAMPLE_JD9366_DIRTY_LANDSCAPE)
    target_sources(${COMPONENT_LIB} PRIVATE "landscape_present.c")
endif()
