# ESP32‑POE Port Tester  
ESP32‑POE or ESP32-POE-ISO LLDP detecting Network Diagnostic Tool  
Version: v1.0

ESP32‑POE Port Tester is a compact network diagnostic utility built for the **Olimex ESP32‑POE** board.  
It uses the ESP32’s internal EMAC + LAN8720 PHY to **listen for LLDP frames**.
Displaying the results on an **SSD1306 OLED**.  

This project is written as an ino file for **Arduino IDE**. (See Build & Flash below)

---

## ✨ Features

### ✔️ Splash Screen  
Displays project name and version on boot.

### ✔️ LLDP Sniffer  
- Intercepts raw Ethernet frames
- Shows progress bar during the listening window  
- Displays switch information on the OLED  

---

## 🛠 Hardware Requirements

- **SSD1306 128×64 OLED (I2C)**  
- **Olimex ESP32‑POE**  

![Pihut-Cart](https://github.com/Mraanderson/esp32-poe-lldp/blob/main/docs/ESP32-POE_OLED.png)

- Ethernet connection to a switch that sends LLDP

---

## 🔌 Wiring

### OLED (I2C)
| OLED Pin | ESP32‑POE Pin | UEXT 10p |
|----------|----------------|------|
| VCC      | 3.3V          |  1   |
| GND      | GND           |  2   |
| SCL      | GPIO 16       |  5   |
| SDA      | GPIO 13       |  6   |
![UEXT-PINS](https://github.com/Mraanderson/esp32-poe-lldp/blob/main/docs/UEXT-PINS.jpg)

https://www.olimex.com/Products/Modules/images/UEXT-PINS.jpg

Direct board pads SDA32 and SCL33 (+3v & Gnd) can also be set with a small code adjustment.

https://github.com/OLIMEX/ESP32-POE/blob/master/DOCUMENTS/ESP32-POE-user-manual.pdf

<img width="2248" height="1529" alt="image" src="https://github.com/Mraanderson/esp32-poe-lldp/blob/main/docs/esp32-poe-info.png" />

---

## 🚀 Build & Flash (Arduino IDE)

1. Install Arduino IDE (I used Kubuntu, IDE v1.8.19)
2. Add the board URL in File > Preferences > Additional Boards Manager URLs
```
https://espressif.github.io/arduino-esp32/package_esp32_index.json
```
3. Tools > Board > Board Manager: Add **esp32** v2.0.17
4. Tools > Board > ESP32 Arduino > **OLIMEX ESP32-PoE** (or ...PoE-ISO if you went for protection!)
5. Tools > Manage Libraries
   ```
   Adafruit BusIO
   Adafruit GFX Library
   Adafruit SSD1306
   ```

**Web flasher to follow**

---

## 🧪 Current Status - Initially reading LLDP

- LLDP sniffing implemented
- Forget on exit

To follow, maybe:
- DHCP testing
- Captive portal further information
- Botton controlled menu on screen
- UI polishing

---

## 📜 License - MIT
