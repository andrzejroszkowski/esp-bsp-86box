# esp-bsp-86box
ESP32-4848S040 BSP Driver

## Menuconfig

Storage-related BSP options are available in `menuconfig` under:

- `Component config -> Board Support Package(86BOX) -> SPIFFS - Virtual File System`
- `Component config -> Board Support Package(86BOX) -> SD card - Virtual File System`

The SD card menu includes:

- `CONFIG_BSP_SD_MOUNT_POINT` with default `"/sdcard"`
- `CONFIG_BSP_SD_FORMAT_ON_MOUNT_FAIL` with default `n`

The SPIFFS menu includes:

- `CONFIG_BSP_SPIFFS_MOUNT_POINT`
- `CONFIG_BSP_SPIFFS_PARTITION_LABEL`
- `CONFIG_BSP_SPIFFS_MAX_FILES`
- `CONFIG_BSP_SPIFFS_FORMAT_ON_MOUNT_FAIL`
