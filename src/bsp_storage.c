#include "bsp/esp-bsp.h"

#include "esp_log.h"
#include "esp_spiffs.h"

static const char *TAG = "bsp_storage";

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
