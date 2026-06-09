#include "bsp/esp-bsp.h"

esp_codec_dev_handle_t bsp_audio_codec_speaker_init(void)
{
    return NULL;
}

esp_codec_dev_handle_t bsp_audio_codec_microphone_init(void)
{
    return NULL;
}

esp_err_t esp_codec_dev_set_out_vol(esp_codec_dev_handle_t handle, int volume)
{
    (void) handle;
    (void) volume;
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t esp_codec_dev_set_in_gain(esp_codec_dev_handle_t handle, float gain)
{
    (void) handle;
    (void) gain;
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t esp_codec_dev_open(esp_codec_dev_handle_t handle, const esp_codec_dev_sample_info_t *fs)
{
    (void) handle;
    (void) fs;
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t esp_codec_dev_close(esp_codec_dev_handle_t handle)
{
    (void) handle;
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t esp_codec_dev_write(esp_codec_dev_handle_t handle, const void *data, size_t len)
{
    (void) handle;
    (void) data;
    (void) len;
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t esp_codec_dev_read(esp_codec_dev_handle_t handle, void *data, size_t len)
{
    (void) handle;
    (void) data;
    (void) len;
    return ESP_ERR_NOT_SUPPORTED;
}
