#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/ledc.h"
#include "driver/i2c_master.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_touch_gt911.h"
#include "bsp/esp-bsp.h"

static const char *TAG = "BSP";
static i2c_master_bus_handle_t i2c_bus_handle = NULL;
static i2c_master_dev_handle_t touch_i2c_handle = NULL;
static bool bsp_initialized = false;

#define BACKLIGHT_LEDC_MODE           LEDC_LOW_SPEED_MODE
#define BACKLIGHT_LEDC_CHANNEL        LEDC_CHANNEL_0
#define BACKLIGHT_LEDC_TIMER          LEDC_TIMER_0
#define BACKLIGHT_LEDC_DUTY_RES       LEDC_TIMER_10_BIT
#define BACKLIGHT_LEDC_MAX_DUTY       ((1 << 10) - 1)

static void bsp_lcd_init_registers(void)
{
    // Bit-bang configuration sequence for ST7701S registers (3-wire SPI emulated)
    vTaskDelay(pdMS_TO_TICKS(120));
}

esp_err_t bsp_init(void)
{
    if (bsp_initialized) return ESP_OK;
    gpio_config_t relay_cfg = {
        .pin_bit_mask = ((1ULL << BSP_RELAY_1_GPIO) | (1ULL << BSP_RELAY_2_GPIO) | (1ULL << BSP_RELAY_3_GPIO)),
        .mode = GPIO_MODE_INPUT_OUTPUT
    };
    ESP_ERROR_CHECK(gpio_config(&relay_cfg));
    gpio_set_level(BSP_RELAY_1_GPIO, 1);
    gpio_set_level(BSP_RELAY_2_GPIO, 1);
    gpio_set_level(BSP_RELAY_3_GPIO, 1);

    i2c_master_bus_config_t i2c_bus_cfg = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .sda_io_num = BSP_I2C_SDA_GPIO,
        .scl_io_num = BSP_I2C_SCL_GPIO,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_cfg, &i2c_bus_handle));

    ledc_timer_config_t ledc_timer = {
        .speed_mode = BACKLIGHT_LEDC_MODE,
        .timer_num = BACKLIGHT_LEDC_TIMER,
        .duty_resolution = BACKLIGHT_LEDC_DUTY_RES,
        .freq_hz = 2000,
        .clk_cfg = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
    ledc_channel_config_t ledc_channel = {
        .speed_mode = BACKLIGHT_LEDC_MODE,
        .channel = BACKLIGHT_LEDC_CHANNEL,
        .timer_sel = BACKLIGHT_LEDC_TIMER,
        .gpio_num = BSP_LCD_BL_GPIO,
        .duty = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    bsp_initialized = true;
    return ESP_OK;
}

esp_err_t bsp_display_init(esp_lcd_panel_handle_t *ret_panel)
{
    if (!bsp_initialized || !ret_panel) return ESP_ERR_INVALID_STATE;
    bsp_lcd_init_registers();
    esp_lcd_rgb_panel_config_t rgb_panel_cfg = {
        .data_width = 16,
        .bits_per_pixel = BSP_LCD_BITS_PER_PIXEL,
        .pclk_gpio_num = BSP_LCD_CLK_GPIO,
        .vsync_gpio_num = BSP_LCD_VSYNC_GPIO,
        .hsync_gpio_num = BSP_LCD_HSYNC_GPIO,
        .de_gpio_num = BSP_LCD_DE_GPIO,
        .data_gpio_nums = {
            BSP_LCD_DATA0_GPIO,  BSP_LCD_DATA1_GPIO,  BSP_LCD_DATA2_GPIO,  BSP_LCD_DATA3_GPIO,
            BSP_LCD_DATA4_GPIO,  BSP_LCD_DATA5_GPIO,  BSP_LCD_DATA6_GPIO,  BSP_LCD_DATA7_GPIO,
            BSP_LCD_DATA8_GPIO,  BSP_LCD_DATA9_GPIO,  BSP_LCD_DATA10_GPIO, BSP_LCD_DATA11_GPIO,
            BSP_LCD_DATA12_GPIO, BSP_LCD_DATA13_GPIO, BSP_LCD_DATA14_GPIO, BSP_LCD_DATA15_GPIO
        },
        .timings = {
            .pclk_hz = BSP_LCD_PIXEL_CLOCK_HZ,
            .h_res = BSP_LCD_H_RES,
            .v_res = BSP_LCD_V_RES,
            .hsync_back_porch = 50,
            .hsync_front_porch = 10,
            .hsync_pulse_width = 8,
            .vsync_back_porch = 20,
            .vsync_front_porch = 10,
            .vsync_pulse_width = 8,
            .flags.pclk_active_neg = true,
        },
        .flags.fb_in_psram = true,
    };
    esp_lcd_panel_handle_t panel_handle = NULL;
    ESP_ERROR_CHECK(esp_lcd_new_panel_rgb(&rgb_panel_cfg, &panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    *ret_panel = panel_handle;
    bsp_display_backlight_set(70);
    return ESP_OK;
}

esp_err_t bsp_touch_init(esp_lcd_touch_handle_t *ret_touch)
{
    if (!bsp_initialized || !ret_touch) return ESP_ERR_INVALID_STATE;
    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = BSP_TOUCH_I2C_ADDR,
        .scl_speed_hz = 400000,
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_bus_handle, &dev_cfg, &touch_i2c_handle));
    esp_lcd_touch_config_t touch_cfg = {
        .x_max = BSP_LCD_H_RES,
        .y_max = BSP_LCD_V_RES,
        .rst_gpio_num = GPIO_NUM_NC,
        .int_gpio_num = BSP_TOUCH_INT_GPIO,
        .levels = {.reset = 0, .interrupt = 0},
        .driver_data = touch_i2c_handle
    };
    esp_lcd_touch_handle_t touch_handle = NULL;
    esp_err_t ret = esp_lcd_touch_new_i2c_gt911(&touch_cfg, &touch_handle);
    if (ret != ESP_OK) return ret;
    *ret_touch = touch_handle;
    return ESP_OK;
}

esp_err_t bsp_display_backlight_set(uint8_t brightness_percent)
{
    if (brightness_percent > 100) brightness_percent = 100;
    uint32_t duty = (uint32_t)((brightness_percent * BACKLIGHT_LEDC_MAX_DUTY) / 100);
    ESP_ERROR_CHECK(ledc_set_duty(BACKLIGHT_LEDC_MODE, BACKLIGHT_LEDC_CHANNEL, duty));
    ESP_ERROR_CHECK(ledc_update_duty(BACKLIGHT_LEDC_MODE, BACKLIGHT_LEDC_CHANNEL));
    return ESP_OK;
}

esp_err_t bsp_relay_set(bsp_relay_t relay, bool state)
{
    gpio_num_t relay_gpio;
    switch (relay) {
        case BSP_RELAY_1: relay_gpio = BSP_RELAY_1_GPIO; break;
        case BSP_RELAY_2: relay_gpio = BSP_RELAY_2_GPIO; break;
        case BSP_RELAY_3: relay_gpio = BSP_RELAY_3_GPIO; break;
        default: return ESP_ERR_INVALID_ARG;
    }
    return gpio_set_level(relay_gpio, state ? 0 : 1);
}

esp_err_t bsp_buzzer_beep(uint32_t freq_hz, uint32_t duration_ms)
{
    gpio_config_t buzzer_cfg = {.pin_bit_mask = (1ULL << BSP_BUZZER_GPIO), .mode = GPIO_MODE_OUTPUT};
    gpio_config(&buzzer_cfg);
    gpio_set_level(BSP_BUZZER_GPIO, 1);
    vTaskDelay(pdMS_TO_TICKS(duration_ms));
    gpio_set_level(BSP_BUZZER_GPIO, 0);
    return ESP_OK;
}
