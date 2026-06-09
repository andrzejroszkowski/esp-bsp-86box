idf_component_register(
    SRCS
        "src/bsp_86box.c"
        "src/bsp_display.c"
        "src/bsp_touch.c"
        "src/bsp_storage.c"
        "src/bsp_audio.c"
    INCLUDE_DIRS "include"
    REQUIRES driver esp_lcd lvgl__lvgl spiffs
    PRIV_REQUIRES esp_lcd_touch_gt911
)
