/*
  ESP32-POE LLDP Sniffer (Ultra-Slim Edition)
  v1.0
  Initial POC. Boot and receive LLDP Hostname, management IP and Port number
  v1.1
   - OLED spacing tidy
   - Comments for OLED connections
*/
 
#include <Arduino.h>
#include <ETH.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "esp_eth.h"
#include "esp_bt.h"
 
#define OLED_SDA 13
#define OLED_SCL 16

// Or use the board pads
// #define OLED_SDA 32
// #define OLED_SCL 33
 
#define FW_VERSION "LLDP v1.1"
 
Adafruit_SSD1306 display(128, 64, &Wire, -1);
 
volatile bool lldpSeen = false;
String sysName = "Unknown";
String portId  = "Unknown";
String mgmtIp  = "Unknown";
 
// ---------------- Helpers ----------------
String ipv4ToStr(const uint8_t *p) {
  char buf[16];
  snprintf(buf, sizeof(buf), "%u.%u.%u.%u", p[0], p[1], p[2], p[3]);
  return String(buf);
}
 
void oledClear() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
}
 
void oledSplash() {
  oledClear();
  display.setTextSize(2);
  display.println("Port Info");
  display.setTextSize(1);
  display.println();
  display.setTextSize(1);
  display.println(FW_VERSION);
  display.display();
}
 
void oledWaiting() {
  oledClear();
  display.println("Waiting for LLDP...");
  display.display();
}
 
// ---------------- Improved LLDP Display ----------------
void oledLLDP() {
  oledClear();
 
  // Title - removed
// ---- Switch Hostname (auto-wrap) ----
display.setTextSize(1);
//display.println("Switch Hostname:");
 
int16_t x1, y1;
uint16_t w, h;
display.getTextBounds(sysName, 0, 0, &x1, &y1, &w, &h);
 
if (w > 120) {
  // Split long hostname into two readable halves
  int mid = sysName.length() / 2;
 
  // Try to split at a dash or dot for neatness
  for (int i = mid; i > 0; i--) {
    if (sysName[i] == '-' || sysName[i] == '.' || sysName[i] == '_') {
      mid = i;
      break;
    }
  }
 
  String line1 = sysName.substring(0, mid);
  String line2 = sysName.substring(mid);
 
  display.println(line1);
  display.println(line2);
} else {
  display.println(sysName);
}
 
  // ---- Mgmt IP ----
  display.println();
  display.print("IP:");
  display.println(mgmtIp);
  display.println();
  
  // ---- Port ID (large font) ----
  display.setTextSize(2);
  display.print("P:");
  display.println(portId);
 
  display.display();
}
 
// ---------------- LLDP Parser ----------------
void parseLLDP(uint8_t *buf, uint32_t len) {
  int idx = 14;
 
  while (idx + 2 <= (int)len) {
    uint16_t hdr = (buf[idx] << 8) | buf[idx + 1];
    uint8_t type = (hdr >> 9) & 0x7F;
    uint16_t tlvLen = hdr & 0x1FF;
    idx += 2;
 
    if (tlvLen == 0 || idx + tlvLen > (int)len) break;
 
    const uint8_t *v = &buf[idx];
 
    switch (type) {
      case 2:
        if (tlvLen > 1) portId = String((char *)(v + 1), tlvLen - 1);
        break;
 
      case 5:
        sysName = String((char *)v, tlvLen);
        break;
 
      case 8:
        if (tlvLen >= 7 && v[1] == 1) mgmtIp = ipv4ToStr(&v[2]);
        break;
    }
 
    idx += tlvLen;
  }
 
  lldpSeen = true;
}
 
// ---------------- Ethernet Sniffer ----------------
static esp_err_t eth_rx_cb(esp_eth_handle_t h, uint8_t *buf, uint32_t len, void *priv) {
  if (len < 14) return ESP_OK;
 
  uint16_t ethertype = (buf[12] << 8) | buf[13];
 
  if (ethertype == 0x8100 && len >= 18) {
    ethertype = (buf[16] << 8) | buf[17];
  }
 
  if (ethertype == 0x88CC) {
    parseLLDP(buf, len);
  }
 
  return ESP_OK;
}
 
// ---------------- Setup ----------------
void setup() {
  Serial.begin(115200);
  esp_bt_controller_disable();
 
  Wire.begin(OLED_SDA, OLED_SCL);
  Wire.setClock(400000);
 
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true) delay(1000);
  }
 
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.display();
 
  oledSplash();
  delay(2000);
  oledWaiting();
 
  ETH.begin();
 
  // SAFE STRUCT HACK FOR OLD ESP32 CORES
  typedef struct {
      void* vtable;
      esp_eth_handle_t eth_handle;
  } ETHClassHack;
 
  esp_eth_handle_t h = ((ETHClassHack*)&ETH)->eth_handle;
 
  if (h) {
    bool enable = true;
    esp_eth_ioctl(h, ETH_CMD_S_PROMISCUOUS, &enable);
    esp_eth_update_input_path(h, eth_rx_cb, nullptr);
  }
}
 
// ---------------- Loop ----------------
void loop() {
  if (lldpSeen) {
    oledLLDP();
 
    // Sleep forever without triggering watchdog
    while (true) {
      vTaskDelay(portMAX_DELAY);
    }
  }
 
  delay(100);
}
