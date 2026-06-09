#include "bsp/esp-bsp.h"

#include "esp_lcd_touch_gt911.h"

esp_err_t bsp_touch_new(const bsp_touch_config_t *config, esp_lcd_touch_handle_t *ret_touch)
{
    (void) config;

    if (ret_touch == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    i2c_master_bus_handle_t i2c_handle = bsp_i2c_get_handle();
    if (i2c_handle == NULL) {
        return ESP_FAIL;
    }

    const esp_lcd_touch_config_t touch_cfg = {
        .x_max = BSP_LCD_H_RES,
        .y_max = BSP_LCD_V_RES,
        .rst_gpio_num = BSP_TOUCH_RST_GPIO,
        .int_gpio_num = BSP_TOUCH_INT_GPIO,
        .levels = {
            .reset = 0,
            .interrupt = 0,
        },
    };

    esp_lcd_panel_io_i2c_config_t touch_io_cfg = ESP_LCD_TOUCH_IO_I2C_GT911_CONFIG();
    touch_io_cfg.dev_addr = BSP_TOUCH_I2C_ADDR;

    esp_lcd_panel_io_handle_t touch_io_handle = NULL;
    esp_err_t ret = esp_lcd_new_panel_io_i2c(i2c_handle, &touch_io_cfg, &touch_io_handle);
    if (ret != ESP_OK) {
        return ret;
    }

    ret = esp_lcd_touch_new_i2c_gt911(touch_io_handle, &touch_cfg, ret_touch);
    if (ret != ESP_OK) {
        esp_lcd_panel_io_del(touch_io_handle);
        return ret;
    }

    return ESP_OK;
}

esp_err_t bsp_touch_init(esp_lcd_touch_handle_t *ret_touch)
{
    return bsp_touch_new(NULL, ret_touch);
}
