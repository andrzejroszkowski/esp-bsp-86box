idf_component_register(
    SRCS "src/esp-bsp.c"
    INCLUDE_DIRS "include"
    REQUIRES driver esp_lcd
    PRIV_REQUIRES esp_lcd_touch_gt911
)
