/**
 * @file esp_bsp.h
 * @brief Board Support Package for ESP32-4848S040 (Guition 4.0" 480x480 Smart Display)
 * @version 1.0.0
 *
 * Target Framework: ESP-IDF v5.5 and later
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "sdkconfig.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "driver/i2s_std.h"
#include "esp_err.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_touch.h"
#include "esp_lcd_types.h"
#include "lvgl.h"

/**************************************************************************************************
 * BSP Board Name
 **************************************************************************************************/

#define BSP_BOARD_86BOX

/**************************************************************************************************
 * BSP Capabilities
 **************************************************************************************************/

#define BSP_CAPS_DISPLAY        1
#define BSP_CAPS_TOUCH          1
#define BSP_CAPS_BUTTONS        0
#define BSP_CAPS_AUDIO          0
#define BSP_CAPS_AUDIO_SPEAKER  0
#define BSP_CAPS_AUDIO_MIC      0
#define BSP_CAPS_SDCARD         0
#define BSP_CAPS_IMU            0

/**************************************************************************************************
 * BSP Version
 **************************************************************************************************/

#define BSP_86BOX_VERSION_MAJOR 1
#define BSP_86BOX_VERSION_MINOR 0
#define BSP_86BOX_VERSION_PATCH 0

/**************************************************************************************************
 * Pinout
 **************************************************************************************************/

#define BSP_LCD_CLK_GPIO        (21)
#define BSP_LCD_HSYNC_GPIO      (16)
#define BSP_LCD_VSYNC_GPIO      (17)
#define BSP_LCD_DE_GPIO         (18)
#define BSP_LCD_CS_GPIO         (39)
#define BSP_LCD_BL_GPIO         (38)

#define BSP_LCD_DATA0_GPIO      (11)
#define BSP_LCD_DATA1_GPIO      (12)
#define BSP_LCD_DATA2_GPIO      (13)
#define BSP_LCD_DATA3_GPIO      (14)
#define BSP_LCD_DATA4_GPIO      (0)
#define BSP_LCD_DATA5_GPIO      (8)
#define BSP_LCD_DATA6_GPIO      (20)
#define BSP_LCD_DATA7_GPIO      (3)
#define BSP_LCD_DATA8_GPIO      (46)
#define BSP_LCD_DATA9_GPIO      (9)
#define BSP_LCD_DATA10_GPIO     (10)
#define BSP_LCD_DATA11_GPIO     (4)
#define BSP_LCD_DATA12_GPIO     (5)
#define BSP_LCD_DATA13_GPIO     (6)
#define BSP_LCD_DATA14_GPIO     (7)
#define BSP_LCD_DATA15_GPIO     (15)

#define BSP_I2C_SDA_GPIO        (19)
#define BSP_I2C_SCL_GPIO        (45)
#define BSP_TOUCH_INT_GPIO      (-1)
#define BSP_TOUCH_RST_GPIO      (-1)
#define BSP_TOUCH_I2C_ADDR      (0x14)

#define BSP_SD_CS_GPIO          (42)
#define BSP_SD_MOSI_GPIO        (47)
#define BSP_SD_CLK_GPIO         (48)
#define BSP_SD_MISO_GPIO        (41)

#define BSP_RELAY_1_GPIO        (40)
#define BSP_RELAY_2_GPIO        (2)
#define BSP_RELAY_3_GPIO        (1)
#define BSP_BUZZER_GPIO         (35)

/**************************************************************************************************
 * Display and Touch Parameters
 **************************************************************************************************/

#define BSP_LCD_H_RES           (480)
#define BSP_LCD_V_RES           (480)
#define BSP_LCD_PIXEL_CLOCK_HZ  (14 * 1000 * 1000)
#define BSP_LCD_COLOR_SPACE     (ESP_LCD_COLOR_SPACE_RGB)
#define BSP_LCD_BIG_ENDIAN      (0)
#define BSP_LCD_BITS_PER_PIXEL  (16)

/**************************************************************************************************
 * SPIFFS
 **************************************************************************************************/

#ifdef CONFIG_BSP_SPIFFS_MOUNT_POINT
#define BSP_SPIFFS_MOUNT_POINT      CONFIG_BSP_SPIFFS_MOUNT_POINT
#else
#define BSP_SPIFFS_MOUNT_POINT      "/spiffs"
#endif

#ifdef CONFIG_BSP_SPIFFS_PARTITION_LABEL
#define BSP_SPIFFS_PARTITION_LABEL  CONFIG_BSP_SPIFFS_PARTITION_LABEL
#else
#define BSP_SPIFFS_PARTITION_LABEL  "storage"
#endif

#ifdef CONFIG_BSP_SPIFFS_MAX_FILES
#define BSP_SPIFFS_MAX_FILES        CONFIG_BSP_SPIFFS_MAX_FILES
#else
#define BSP_SPIFFS_MAX_FILES        (5)
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int max_transfer_sz;
} bsp_display_config_t;

typedef struct {
    void *dummy;
} bsp_touch_config_t;

typedef struct esp_codec_dev *esp_codec_dev_handle_t;

typedef struct {
    uint32_t sample_rate;
    uint8_t channel;
    uint8_t bits_per_sample;
    i2s_mclk_multiple_t mclk_multiple;
} esp_codec_dev_sample_info_t;

typedef enum {
    BSP_RELAY_1 = 0,
    BSP_RELAY_2,
    BSP_RELAY_3,
    BSP_RELAY_MAX
} bsp_relay_t;

esp_err_t bsp_init(void);
const char *bsp_get_version(void);

esp_err_t bsp_i2c_init(void);
esp_err_t bsp_i2c_deinit(void);
i2c_master_bus_handle_t bsp_i2c_get_handle(void);

esp_err_t bsp_display_brightness_init(void);
esp_err_t bsp_display_brightness_set(int brightness_percent);
esp_err_t bsp_display_backlight_on(void);
esp_err_t bsp_display_backlight_off(void);
esp_err_t bsp_display_new(const bsp_display_config_t *config,
                          esp_lcd_panel_handle_t *ret_panel,
                          esp_lcd_panel_io_handle_t *ret_io);
lv_display_t *bsp_display_start(void);
lv_indev_t *bsp_display_get_input_dev(void);
bool bsp_display_lock(uint32_t timeout_ms);
void bsp_display_unlock(void);
void bsp_display_rotate(lv_display_t *disp, lv_display_rotation_t rotation);
esp_err_t bsp_display_enter_sleep(void);
esp_err_t bsp_display_exit_sleep(void);

esp_err_t bsp_touch_new(const bsp_touch_config_t *config, esp_lcd_touch_handle_t *ret_touch);

esp_err_t bsp_spiffs_mount(void);
esp_err_t bsp_spiffs_unmount(void);

esp_codec_dev_handle_t bsp_audio_codec_speaker_init(void);
esp_codec_dev_handle_t bsp_audio_codec_microphone_init(void);
esp_err_t esp_codec_dev_set_out_vol(esp_codec_dev_handle_t handle, int volume);
esp_err_t esp_codec_dev_set_in_gain(esp_codec_dev_handle_t handle, float gain);
esp_err_t esp_codec_dev_open(esp_codec_dev_handle_t handle, const esp_codec_dev_sample_info_t *fs);
esp_err_t esp_codec_dev_close(esp_codec_dev_handle_t handle);
esp_err_t esp_codec_dev_write(esp_codec_dev_handle_t handle, const void *data, size_t len);
esp_err_t esp_codec_dev_read(esp_codec_dev_handle_t handle, void *data, size_t len);

esp_err_t bsp_relay_set(bsp_relay_t relay, bool state);
esp_err_t bsp_buzzer_beep(uint32_t freq_hz, uint32_t duration_ms);

/* Backward-compatible wrappers for the previous local API. */
esp_err_t bsp_display_init(esp_lcd_panel_handle_t *ret_panel);
esp_err_t bsp_touch_init(esp_lcd_touch_handle_t *ret_touch);
esp_err_t bsp_display_backlight_set(uint8_t brightness_percent);

#ifdef __cplusplus
}
#endif
