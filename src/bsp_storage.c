#include "bsp/esp-bsp.h"

#include "driver/spi_common.h"
#include "esp_vfs_fat.h"
#include "esp_log.h"
#include "esp_spiffs.h"

static const char *TAG = "bsp_storage";
static sdmmc_card_t *s_bsp_sdcard;

esp_err_t bsp_spiffs_mount(void)
{
    const esp_vfs_spiffs_conf_t conf = {
        .base_path = BSP_SPIFFS_MOUNT_POINT,
        .partition_label = BSP_SPIFFS_PARTITION_LABEL,
        .max_files = BSP_SPIFFS_MAX_FILES,
#ifdef CONFIG_BSP_SPIFFS_FORMAT_ON_MOUNT_FAIL
        .format_if_mount_failed = true,
#else
        .format_if_mount_failed = false,
#endif
    };

    esp_err_t ret = esp_vfs_spiffs_register(&conf);
    if (ret != ESP_OK) {
        return ret;
    }

    size_t total = 0;
    size_t used = 0;
    ret = esp_spiffs_info(conf.partition_label, &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "SPIFFS mounted at %s (total=%u, used=%u)", conf.base_path, (unsigned) total, (unsigned) used);
    return ESP_OK;
}

esp_err_t bsp_spiffs_unmount(void)
{
    return esp_vfs_spiffs_unregister(BSP_SPIFFS_PARTITION_LABEL);
}

sdmmc_card_t *bsp_sdcard_get_handle(void)
{
    return s_bsp_sdcard;
}

esp_err_t bsp_sdcard_mount(void)
{
    if (s_bsp_sdcard != NULL) {
        return ESP_OK;
    }

    const sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    sdmmc_host_t sdspi_host = host;
    sdspi_host.slot = BSP_SDSPI_HOST;

    const spi_bus_config_t bus_cfg = {
        .mosi_io_num = BSP_SD_MOSI_GPIO,
        .miso_io_num = BSP_SD_MISO_GPIO,
        .sclk_io_num = BSP_SD_CLK_GPIO,
        .quadwp_io_num = GPIO_NUM_NC,
        .quadhd_io_num = GPIO_NUM_NC,
        .max_transfer_sz = 4000,
    };

    esp_err_t ret = spi_bus_initialize(BSP_SDSPI_HOST, &bus_cfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        return ret;
    }

    sdspi_device_config_t slot_cfg = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_cfg.gpio_cs = BSP_SD_CS_GPIO;
    slot_cfg.host_id = BSP_SDSPI_HOST;

    const esp_vfs_fat_sdmmc_mount_config_t mount_cfg = {
#ifdef CONFIG_BSP_SD_FORMAT_ON_MOUNT_FAIL
        .format_if_mount_failed = true,
#else
        .format_if_mount_failed = false,
#endif
        .max_files = 5,
        .allocation_unit_size = 16 * 1024,
    };

    ret = esp_vfs_fat_sdspi_mount(BSP_SD_MOUNT_POINT, &sdspi_host, &slot_cfg, &mount_cfg, &s_bsp_sdcard);
    if (ret != ESP_OK) {
        spi_bus_free(BSP_SDSPI_HOST);
        return ret;
    }

    ESP_LOGI(TAG, "SD card mounted at %s", BSP_SD_MOUNT_POINT);
    return ESP_OK;
}

esp_err_t bsp_sdcard_unmount(void)
{
    if (s_bsp_sdcard == NULL) {
        return ESP_OK;
    }

    esp_err_t ret = esp_vfs_fat_sdcard_unmount(BSP_SD_MOUNT_POINT, s_bsp_sdcard);
    s_bsp_sdcard = NULL;

    esp_err_t bus_ret = spi_bus_free(BSP_SDSPI_HOST);
    if (ret != ESP_OK) {
        return ret;
    }

    return bus_ret;
}
