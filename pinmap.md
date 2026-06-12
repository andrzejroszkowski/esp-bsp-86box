# Pin Map (Pinout) — ESP32-4848S040 (ESP32-S3)

This document reflects the pin assignments from `pinmap.txt`, which provides the authoritative `-D NAME=VALUE` board parameters.

---

## 1. LCD Display (RGB Interface / ST7701S Controller)

The display operates in parallel RGB mode, driven directly by the ESP32-S3 LCD peripherals.

### RGB Timing and Backlight Lines

| LCD Signal | ESP32-S3 GPIO | Function Description |
| :--- | :---: | :--- |
| **LCD_CLK**  | `GPIO 21` | Pixel Clock Signal (PCLK) |
| **LCD_HSYNC**| `GPIO 16` | Horizontal Synchronization |
| **LCD_VSYNC**| `GPIO 17` | Vertical Synchronization |
| **LCD_DE**   | `GPIO 18` | Data Enable |
| **LCD_BL**   | `GPIO 38` | Display Backlight (Backlight PWM) |

### Panel Setup Bus

| LCD Signal | ESP32-S3 GPIO | Function Description |
| :--- | :---: | :--- |
| **LCD_CS**   | `GPIO 39` | Panel Chip Select |
| **LCD_SCLK** | `GPIO 48` | Panel Serial Clock |
| **LCD_MOSI** | `GPIO 47` | Panel Serial Data Out |
| **LCD_DC**   | Not connected | `TFT_DC=-1` |
| **LCD_MISO** | Not connected | `TFT_MISO=-1` |
| **LCD_RST**  | Not connected | `TFT_RST=-1` |
| **LCD_BUSY** | Not connected | `TFT_BUSY=-1` |

### Color Data Bus (RGB565 Data Bus)

| Data Line | ESP32-S3 GPIO | Color Description |
| :--- | :---: | :--- |
| **LCD_R0** | `GPIO 11` | Red - Bit 0 |
| **LCD_R1** | `GPIO 12` | Red - Bit 1 |
| **LCD_R2** | `GPIO 13` | Red - Bit 2 |
| **LCD_R3** | `GPIO 14` | Red - Bit 3 |
| **LCD_R4** | `GPIO 0`  | Red - Bit 4 |
| **LCD_G0** | `GPIO 8`  | Green - Bit 0 |
| **LCD_G1** | `GPIO 20` | Green - Bit 1 |
| **LCD_G2** | `GPIO 3`  | Green - Bit 2 |
| **LCD_G3** | `GPIO 46` | Green - Bit 3 |
| **LCD_G4** | `GPIO 9`  | Green - Bit 4 |
| **LCD_G5** | `GPIO 10` | Green - Bit 5 |
| **LCD_B0** | `GPIO 4`  | Blue - Bit 0 |
| **LCD_B1** | `GPIO 5`  | Blue - Bit 1 |
| **LCD_B2** | `GPIO 6`  | Blue - Bit 2 |
| **LCD_B3** | `GPIO 7`  | Blue - Bit 3 |
| **LCD_B4** | `GPIO 15` | Blue - Bit 4 |

---

## 2. Touch Panel (I2C / GT911 Controller)

The touch screen communicates via a dedicated I2C bus.

* **Device I2C Address**: `0x5D` (or alternatively `0x14`)

| Touch Signal | ESP32-S3 GPIO | Function Description |
| :--- | :---: | :--- |
| **TP_SDA** | `GPIO 19` | I2C Data Line (SDA) |
| **TP_SCL** | `GPIO 45` | I2C Clock Line (SCL) |
| **TP_INT** | Not connected | `TOUCH_IRQ=-1` |
| **TP_RST** | Not connected | `TOUCH_RST=-1` |

---

## 3. USB / UART Reserved Pins

| Signal | ESP32-S3 GPIO | Function Description |
| :--- | :---: | :--- |
| **USB_TXD** | `GPIO 43` | Reserved USB/UART TX pin |
| **USB_RXD** | `GPIO 44` | Reserved USB/UART RX pin |
| **BOOT**    | `GPIO 0`  | Bootloader Button (Shared with LCD_R4 line) |

---

## 4. MicroSD Card Slot (Validation Header)

These pins are taken from `to_validate.h`.

| SD Signal | ESP32-S3 GPIO | Function Description |
| :--- | :---: | :--- |
| **SD_CS**   | `GPIO 42` | Chip Select |
| **SD_MOSI** | `GPIO 47` | Master Out Slave In (Data In) |
| **SD_CLK**  | `GPIO 48` | Serial Clock |
| **SD_MISO** | `GPIO 41` | Master In Slave Out (Data Out) |

---

## 5. Relay Outputs and Buzzer (Validation Header)

These pins are taken from `to_validate.h`.

| Peripheral | ESP32-S3 GPIO | Signal Type / Notes |
| :--- | :---: | :--- |
| **Relay 1** | `GPIO 40` | Digital Control |
| **Relay 2** | `GPIO 2`  | Digital Control |
| **Relay 3** | `GPIO 1`  | Digital Control |
| **Buzzer**  | `GPIO 35` | Acoustic Indicator |
