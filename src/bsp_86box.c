#include "bsp/esp-bsp.h"

#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "bsp_86box";
static i2c_master_bus_handle_t s_i2c_bus_handle;
static bool s_bsp_initialized;

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

static const char *s_version =
    TOSTRING(BSP_86BOX_VERSION_MAJOR) "."
    TOSTRING(BSP_86BOX_VERSION_MINOR) "."
    TOSTRING(BSP_86BOX_VERSION_PATCH);

const char *bsp_get_version(void)
{
    return s_version;
}

esp_err_t bsp_i2c_init(void)
{
    if (s_i2c_bus_handle != NULL) {
        return ESP_OK;
    }

    const i2c_master_bus_config_t i2c_bus_cfg = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .sda_io_num = BSP_I2C_SDA_GPIO,
        .scl_io_num = BSP_I2C_SCL_GPIO,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    return i2c_new_master_bus(&i2c_bus_cfg, &s_i2c_bus_handle);
}

esp_err_t bsp_i2c_deinit(void)
{
    if (s_i2c_bus_handle == NULL) {
        return ESP_OK;
    }

    esp_err_t ret = i2c_del_master_bus(s_i2c_bus_handle);
    if (ret == ESP_OK) {
        s_i2c_bus_handle = NULL;
    }

    return ret;
}

i2c_master_bus_handle_t bsp_i2c_get_handle(void)
{
    if (bsp_i2c_init() != ESP_OK) {
        return NULL;
    }

    return s_i2c_bus_handle;
}

esp_err_t bsp_init(void)
{
    if (s_bsp_initialized) {
        return ESP_OK;
    }

    ESP_LOGI(TAG, "Initializing BSP v%s", bsp_get_version());

    const gpio_config_t relay_cfg = {
        .pin_bit_mask = ((1ULL << BSP_RELAY_1_GPIO) | (1ULL << BSP_RELAY_2_GPIO) | (1ULL << BSP_RELAY_3_GPIO)),
        .mode = GPIO_MODE_INPUT_OUTPUT,
    };

    esp_err_t ret = gpio_config(&relay_cfg);
    if (ret != ESP_OK) {
        return ret;
    }

    ret = gpio_set_level(BSP_RELAY_1_GPIO, 1);
    if (ret != ESP_OK) {
        return ret;
    }
    ret = gpio_set_level(BSP_RELAY_2_GPIO, 1);
    if (ret != ESP_OK) {
        return ret;
    }
    ret = gpio_set_level(BSP_RELAY_3_GPIO, 1);
    if (ret != ESP_OK) {
        return ret;
    }

    ret = bsp_i2c_init();
    if (ret != ESP_OK) {
        return ret;
    }

    ret = bsp_display_brightness_init();
    if (ret != ESP_OK) {
        return ret;
    }

    s_bsp_initialized = true;
    return ESP_OK;
}

esp_err_t bsp_relay_set(bsp_relay_t relay, bool state)
{
    esp_err_t ret = bsp_init();
    if (ret != ESP_OK) {
        return ret;
    }

    gpio_num_t relay_gpio;

    switch (relay) {
        case BSP_RELAY_1:
            relay_gpio = BSP_RELAY_1_GPIO;
            break;
        case BSP_RELAY_2:
            relay_gpio = BSP_RELAY_2_GPIO;
            break;
        case BSP_RELAY_3:
            relay_gpio = BSP_RELAY_3_GPIO;
            break;
        default:
            return ESP_ERR_INVALID_ARG;
    }

    return gpio_set_level(relay_gpio, state ? 0 : 1);
}

esp_err_t bsp_buzzer_beep(uint32_t freq_hz, uint32_t duration_ms)
{
    (void) freq_hz;

    const gpio_config_t buzzer_cfg = {
        .pin_bit_mask = (1ULL << BSP_BUZZER_GPIO),
        .mode = GPIO_MODE_OUTPUT,
    };

    esp_err_t ret = gpio_config(&buzzer_cfg);
    if (ret != ESP_OK) {
        return ret;
    }

    ret = gpio_set_level(BSP_BUZZER_GPIO, 1);
    if (ret != ESP_OK) {
        return ret;
    }

    vTaskDelay(pdMS_TO_TICKS(duration_ms));

    return gpio_set_level(BSP_BUZZER_GPIO, 0);
}
