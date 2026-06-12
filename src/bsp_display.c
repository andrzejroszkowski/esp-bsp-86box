#include "bsp/esp-bsp.h"

#include "driver/ledc.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lvgl_port.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "bsp_display";
static bool s_backlight_initialized;
static int s_backlight_percent = 70;
static bool s_lvgl_initialized;
static lv_display_t *s_display_handle;
static lv_indev_t *s_touch_input_handle;

#define BACKLIGHT_LEDC_MODE           LEDC_LOW_SPEED_MODE
#define BACKLIGHT_LEDC_CHANNEL        LEDC_CHANNEL_0
#define BACKLIGHT_LEDC_TIMER          LEDC_TIMER_0
#define BACKLIGHT_LEDC_DUTY_RES       LEDC_TIMER_10_BIT
#define BACKLIGHT_LEDC_MAX_DUTY       ((1 << 10) - 1)

static void bsp_lcd_init_registers(void)
{
    vTaskDelay(pdMS_TO_TICKS(120));
}

static lv_display_t *bsp_display_lcd_init(void)
{
    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_handle_t panel_handle = NULL;
    const bsp_display_config_t display_cfg = {
        .max_transfer_sz = (BSP_LCD_H_RES * 100) * (int) sizeof(uint16_t),
    };

    esp_err_t ret = bsp_display_new(&display_cfg, &panel_handle, &io_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize LCD panel: %s", esp_err_to_name(ret));
        return NULL;
    }

    ret = esp_lcd_panel_disp_on_off(panel_handle, true);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to enable LCD output: %s", esp_err_to_name(ret));
        return NULL;
    }

    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = io_handle,
        .panel_handle = panel_handle,
        .buffer_size = BSP_LCD_H_RES * 100,
        .double_buffer = 0,
        .hres = BSP_LCD_H_RES,
        .vres = BSP_LCD_V_RES,
        .monochrome = false,
        .flags = {
            .buff_dma = true,
#if LVGL_VERSION_MAJOR >= 9
            .swap_bytes = (BSP_LCD_BIG_ENDIAN ? true : false),
#endif
        },
    };

    return lvgl_port_add_disp(&disp_cfg);
}

static lv_indev_t *bsp_display_indev_init(lv_display_t *disp)
{
    esp_lcd_touch_handle_t touch_handle = NULL;
    esp_err_t ret = bsp_touch_new(NULL, &touch_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize touch controller: %s", esp_err_to_name(ret));
        return NULL;
    }

    const lvgl_port_touch_cfg_t touch_cfg = {
        .disp = disp,
        .handle = touch_handle,
    };

    return lvgl_port_add_touch(&touch_cfg);
}

esp_err_t bsp_display_brightness_init(void)
{
    if (s_backlight_initialized) {
        return ESP_OK;
    }

    const ledc_timer_config_t ledc_timer = {
        .speed_mode = BACKLIGHT_LEDC_MODE,
        .timer_num = BACKLIGHT_LEDC_TIMER,
        .duty_resolution = BACKLIGHT_LEDC_DUTY_RES,
        .freq_hz = 2000,
        .clk_cfg = LEDC_AUTO_CLK,
    };
    esp_err_t ret = ledc_timer_config(&ledc_timer);
    if (ret != ESP_OK) {
        return ret;
    }

    const ledc_channel_config_t ledc_channel = {
        .speed_mode = BACKLIGHT_LEDC_MODE,
        .channel = BACKLIGHT_LEDC_CHANNEL,
        .timer_sel = BACKLIGHT_LEDC_TIMER,
        .gpio_num = BSP_LCD_BL_GPIO,
        .duty = 0,
    };
    ret = ledc_channel_config(&ledc_channel);
    if (ret != ESP_OK) {
        return ret;
    }

    s_backlight_initialized = true;
    return ESP_OK;
}

esp_err_t bsp_display_brightness_set(int brightness_percent)
{
    esp_err_t ret = bsp_display_brightness_init();
    if (ret != ESP_OK) {
        return ret;
    }

    if (brightness_percent < 0) {
        brightness_percent = 0;
    } else if (brightness_percent > 100) {
        brightness_percent = 100;
    }

    if (brightness_percent > 0) {
        s_backlight_percent = brightness_percent;
    }

    const uint32_t duty = (uint32_t)((brightness_percent * BACKLIGHT_LEDC_MAX_DUTY) / 100);
    ret = ledc_set_duty(BACKLIGHT_LEDC_MODE, BACKLIGHT_LEDC_CHANNEL, duty);
    if (ret != ESP_OK) {
        return ret;
    }

    return ledc_update_duty(BACKLIGHT_LEDC_MODE, BACKLIGHT_LEDC_CHANNEL);
}

esp_err_t bsp_display_backlight_on(void)
{
    return bsp_display_brightness_set(s_backlight_percent);
}

esp_err_t bsp_display_backlight_off(void)
{
    return bsp_display_brightness_set(0);
}

esp_err_t bsp_display_new(const bsp_display_config_t *config,
                          esp_lcd_panel_handle_t *ret_panel,
                          esp_lcd_panel_io_handle_t *ret_io)
{
    (void) config;

    if (ret_panel == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    if (ret_io != NULL) {
        *ret_io = NULL;
    }

    esp_err_t ret = bsp_init();
    if (ret != ESP_OK) {
        return ret;
    }

    bsp_lcd_init_registers();

    const esp_lcd_rgb_panel_config_t rgb_panel_cfg = {
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
            BSP_LCD_DATA12_GPIO, BSP_LCD_DATA13_GPIO, BSP_LCD_DATA14_GPIO, BSP_LCD_DATA15_GPIO,
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

    ret = esp_lcd_new_rgb_panel(&rgb_panel_cfg, ret_panel);
    if (ret != ESP_OK) {
        return ret;
    }

    ret = esp_lcd_panel_reset(*ret_panel);
    if (ret != ESP_OK) {
        esp_lcd_panel_del(*ret_panel);
        *ret_panel = NULL;
        return ret;
    }

    ret = esp_lcd_panel_init(*ret_panel);
    if (ret != ESP_OK) {
        esp_lcd_panel_del(*ret_panel);
        *ret_panel = NULL;
        return ret;
    }

    return bsp_display_brightness_set(s_backlight_percent);
}

esp_err_t bsp_display_init(esp_lcd_panel_handle_t *ret_panel)
{
    return bsp_display_new(NULL, ret_panel, NULL);
}

esp_err_t bsp_display_backlight_set(uint8_t brightness_percent)
{
    return bsp_display_brightness_set((int) brightness_percent);
}

lv_display_t *bsp_display_start(void)
{
    if (s_display_handle != NULL) {
        return s_display_handle;
    }

    const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    if (!s_lvgl_initialized) {
        esp_err_t ret = lvgl_port_init(&lvgl_cfg);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "Failed to initialize LVGL port: %s", esp_err_to_name(ret));
            return NULL;
        }
        s_lvgl_initialized = true;
    }

    s_display_handle = bsp_display_lcd_init();
    if (s_display_handle == NULL) {
        return NULL;
    }

    s_touch_input_handle = bsp_display_indev_init(s_display_handle);
    if (s_touch_input_handle == NULL) {
        return NULL;
    }

    return s_display_handle;
}

lv_indev_t *bsp_display_get_input_dev(void)
{
    return s_touch_input_handle;
}

bool bsp_display_lock(uint32_t timeout_ms)
{
    if (!s_lvgl_initialized) {
        return true;
    }

    return lvgl_port_lock(timeout_ms);
}

void bsp_display_unlock(void)
{
    if (s_lvgl_initialized) {
        lvgl_port_unlock();
    }
}

void bsp_display_rotate(lv_display_t *disp, lv_display_rotation_t rotation)
{
    if (disp != NULL) {
        lv_display_set_rotation(disp, rotation);
    }
}

esp_err_t bsp_display_enter_sleep(void)
{
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t bsp_display_exit_sleep(void)
{
    return ESP_ERR_NOT_SUPPORTED;
}
