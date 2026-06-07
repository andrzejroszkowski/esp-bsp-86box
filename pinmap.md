# Pin Map (Pinout) — ESP32-4848S040 (ESP32-S3)

This document contains the complete hardware pin configuration for the ESP32-S3 microcontroller used in the 4.0" (480x480 pixels) touch panel module.

---

## 1. LCD Display (RGB Interface / ST7701S Controller)

The display operates in parallel RGB mode, driven directly by the ESP32-S3 LCD peripherals.

### Control Lines


| LCD Signal | ESP32-S3 GPIO | Function Description |
| :--- | :---: | :--- |
| **LCD_CLK**  | `GPIO 10` | Pixel Clock Signal (PCLK) |
| **LCD_HSYNC**| `GPIO 39` | Horizontal Synchronization |
| **LCD_VSYNC**| `GPIO 48` | Vertical Synchronization |
| **LCD_DE**   | `GPIO 47` | Data Enable |
| **LCD_BL**   | `GPIO 38` | Display Backlight (Backlight PWM) |

### Color Data Bus (RGB 565 Data Bus)


| Data Line | ESP32-S3 GPIO | Color Description |
| :--- | :---: | :--- |
| **LCD_R0** | `GPIO 11` | Red - Bit 0 |
| **LCD_R1** | `GPIO 12` | Red - Bit 1 |
| **LCD_R2** | `GPIO 13` | Red - Bit 2 |
| **LCD_R3** | `GPIO 14` | Red - Bit 3 |
| **LCD_R4** | `GPIO 0`  | Red - Bit 4 |
| **LCD_G0** | `GPIO 8`  | Green - Bit 0 |
| **LCD_G1** | `GPIO 2`  | Green - Bit 1 |
| **LCD_G2** | `GPIO 46` | Green - Bit 2 |
| **LCD_G3** | `GPIO 9`  | Green - Bit 3 |
| **LCD_G4** | `GPIO 1`  | Green - Bit 4 |
| **LCD_G5** | `GPIO 3`  | Green - Bit 5 |
| **LCD_B0** | `GPIO 42` | Blue - Bit 0 |
| **LCD_B1** | `GPIO 21` | Blue - Bit 1 |
| **LCD_B2** | `GPIO 15` | Blue - Bit 2 |
| **LCD_B3** | `GPIO 16` | Blue - Bit 3 |
| **LCD_B4** | `GPIO 17` | Blue - Bit 4 |

---

## 2. Touch Panel (I2C / GT911 Controller)

The touch screen communicates via a dedicated I2C bus (shared with optional external sensors).

* **Device I2C Address**: `0x5D` (or alternatively `0x14`)



| Touch Signal | ESP32-S3 GPIO | Function Description |
| :--- | :---: | :--- |
| **TP_SDA** | `GPIO 19` | I2C Data Line (SDA) |
| **TP_SCL** | `GPIO 20` | I2C Clock Line (SCL) |
| **TP_INT** | `GPIO 18` | Touch Panel Interrupt |
| **TP_RST** | `GPIO 38` | Touch Controller Reset (Shared with backlight / internal) |

---

## 3. MicroSD Card Slot (SPI / FSPI)

The TF card slot utilizes a high-speed SPI interface.



| SD Signal | ESP32-S3 GPIO | Function Description |
| :--- | :---: | :--- |
| **SD_CS**   | `GPIO 34` | Chip Select |
| **SD_MOSI** | `GPIO 35` | Master Out Slave In (Data In) |
| **SD_CLK**  | `GPIO 36` | Serial Clock |
| **SD_MISO** | `GPIO 37` | Master In Slave Out (Data Out) |

---

## 4. Relay Outputs and Peripherals (Hardware Version Dependent)

Boards come in versions without relays or equipped with 1 to 3 power relays.



| Peripheral | ESP32-S3 GPIO | Signal Type / Notes |
| :--- | :---: | :--- |
| **Relay 1** | `GPIO 40` | Digital Control (Active LOW / Inverted) |
| **Relay 2** | `GPIO 41` | Digital Control (Active LOW / Inverted) |
| **Relay 3** | `GPIO 4`  | Present in 3-channel versions |
| **Buzzer**  | `GPIO 45` | Acoustic Indicator (PWM) |

---

## 5. UART and System Ports



| Signal | ESP32-S3 GPIO | Function Description |
| :--- | :---: | :--- |
| **TXD0** | `GPIO 43` | UART Transmission (Programming via CH340) |
| **RXD0** | `GPIO 44` | UART Reception (Programming via CH340) |
| **BOOT** | `GPIO 0`  | Bootloader Button (Shared with LCD_R4 line) |
