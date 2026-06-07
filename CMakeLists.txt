idf_component_register(
    SRCS "src/esp_bsp.c"
    INCLUDE_DIRS "include"
    REQUIRES driver esp_lcd ledc
    PRIV_REQUIRES esp_lcd_touch_gt911
)
