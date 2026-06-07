/**
 * @file esp_bsp.h
 * @brief Board Support Package for ESP32-4848S040 (Guition 4.0" 480x480 Smart Display)
 * @version 1.0.0
 * 
 * Target Framework: ESP-IDF v5.5 and later
 */

#pragma once

#include "sdkconfig.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include "esp_lcd_types.h"
#include "esp_lcd_touch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Hardware Pinout Configuration (ESP32-S3)
 ******************************************************************************/

/* LCD RGB Interface Pins */
#define BSP_LCD_CLK_GPIO        (10)
#define BSP_LCD_HSYNC_GPIO      (39)
#define BSP_LCD_VSYNC_GPIO      (48)
#define BSP_LCD_DE_GPIO         (47)
#define BSP_LCD_BL_GPIO         (38)

#define BSP_LCD_DATA0_GPIO      (11) // R0
#define BSP_LCD_DATA1_GPIO      (12) // R1
#define BSP_LCD_DATA2_GPIO      (13) // R2
#define BSP_LCD_DATA3_GPIO      (14) // R3
#define BSP_LCD_DATA4_GPIO      (0)  // R4 (Shared with BOOT)
#define BSP_LCD_DATA5_GPIO      (8)  // G0
#define BSP_LCD_DATA6_GPIO      (2)  // G1
#define BSP_LCD_DATA7_GPIO      (46) // G2
#define BSP_LCD_DATA8_GPIO      (9)  // G3
#define BSP_LCD_DATA9_GPIO      (1)  // G4
#define BSP_LCD_DATA10_GPIO     (3)  // G5
#define BSP_LCD_DATA11_GPIO     (42) // B0
#define BSP_LCD_DATA12_GPIO     (21) // B1
#define BSP_LCD_DATA13_GPIO     (15) // B2
#define BSP_LCD_DATA14_GPIO     (16) // B3
#define BSP_LCD_DATA15_GPIO     (17) // B4

/* Touch Screen Pins (I2C) */
#define BSP_I2C_SDA_GPIO        (19)
#define BSP_I2C_SCL_GPIO        (20)
#define BSP_TOUCH_INT_GPIO      (18)
#define BSP_TOUCH_RST_GPIO      (38) // Internally tied to Backlight on some hardware revisions
#define BSP_TOUCH_I2C_ADDR      (0x5D)

/* SD Card Pins (SPI) */
#define BSP_SD_CS_GPIO          (34)
#define BSP_SD_MOSI_GPIO        (35)
#define BSP_SD_CLK_GPIO         (36)
#define BSP_SD_MISO_GPIO        (37)

/* Relay & Audio Peripherals */
#define BSP_RELAY_1_GPIO        (40)
#define BSP_RELAY_2_GPIO        (41)
#define BSP_RELAY_3_GPIO        (4)
#define BSP_BUZZER_GPIO         (45)

/*******************************************************************************
 * Display and Touch Parameters
 ******************************************************************************/
#define BSP_LCD_H_RES           (480)
#define BSP_LCD_V_RES           (480)
#define BSP_LCD_PIXEL_CLOCK_HZ  (14 * 1000 * 1000) // 14MHz typical for ST7701S 480x480
#define BSP_LCD_COLOR_SPACE     (ESP_LCD_COLOR_SPACE_RGB)
#define BSP_LCD_BIG_ENDIAN      (0)
#define BSP_LCD_BITS_PER_PIXEL  (16) // RGB565

/*******************************************************************************
 * Public Enumerations and Types
 ******************************************************************************/

/**
 * @brief Board Relays enumeration
 */
typedef enum {
    BSP_RELAY_1 = 0,
    BSP_RELAY_2,
    BSP_RELAY_3,
    BSP_RELAY_MAX
} bsp_relay_t;

/*******************************************************************************
 * Public API Declarations
 ******************************************************************************/

/**
 * @brief Initialize all basic board peripherals (I2C, GPIOs, Relays, Backlight PWM)
 * 
 * @return esp_err_t ESP_OK on success, appropriate error code otherwise
 */
esp_err_t bsp_init(void);

/**
 * @brief Initialize the LCD display panel via esp_lcd RGB driver
 * 
 * @param[out] ret_panel Pointer to store the created LCD panel handle
 * @return esp_err_t ESP_OK on success
 */
esp_err_t bsp_display_init(esp_lcd_panel_handle_t *ret_panel);

/**
 * @brief Initialize the capacitive touch panel driver (GT911)
 * 
 * @param[out] ret_touch Pointer to store the created Touch handle
 * @return esp_err_t ESP_OK on success
 */
esp_err_t bsp_touch_init(esp_lcd_touch_handle_t *ret_touch);

/**
 * @brief Set LCD Backlight brightness level
 * 
 * @param brightness_percent Brightness value from 0 to 100
 * @return esp_err_t ESP_OK on success
 */
esp_err_t bsp_display_backlight_set(uint8_t brightness_percent);

/**
 * @brief Control board onboard power relays
 * 
 * @param relay Relay index (BSP_RELAY_1, BSP_RELAY_2, BSP_RELAY_3)
 * @param state true to turn ON (close), false to turn OFF (open)
 * @return esp_err_t ESP_OK on success
 */
esp_err_t bsp_relay_set(bsp_relay_t relay, bool state);

/**
 * @brief Trigger the onboard buzzer for a specified duration using hardware PWM
 * 
 * @param freq_hz Frequency in Hz
 * @param duration_ms Duration in milliseconds (0 for indefinite blocking depends on implementation)
 * @return esp_err_t ESP_OK on success
 */
esp_err_t bsp_buzzer_beep(uint32_t freq_hz, uint32_t duration_ms);

#ifdef __cplusplus
}
#endif
