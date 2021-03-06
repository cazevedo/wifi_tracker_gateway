/*
  API ref: https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/wifi/esp_now.html
*/
#include <esp_now.h>
#include <WiFi.h>
// #include <user_interface.h>

#include "main.h"
#include "crc.h"

#define CHANNEL 1
#define NOPASSWORD 0
#define PASSWORD "12345678"

// Message packet;
Message *packetPtr;
// Set a custom base MAC address
// In AP mode is added 1 to the base MAC address: CC:CC:CC:CC:CC:CD
// In ST mode is added 0 to the base MAC address: CC:CC:CC:CC:CC:CC
uint8_t CustomMacAddress[] = {0xCC, 0xCC, 0xCC, 0xCC, 0xCC, 0xCC};

// Init ESP Now with fallback
void InitESPNow() {
  WiFi.disconnect();
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESPNow Init Success");
  }
  else {
    Serial.println("ESPNow Init Failed");
    // Retry InitESPNow, add a counte and then restart?
    // InitESPNow();
    // or Simply Restart
    ESP.restart();
  }
}

// config AP SSID
void configDeviceAP() {
  char* SSID = "Gateway-Tracker";
  bool result;
  if (NOPASSWORD)
    result = WiFi.softAP(SSID);
  else
    result = WiFi.softAP(SSID, PASSWORD, CHANNEL, false, 8);

  if (!result) {
    Serial.println("AP Config failed.");
  } else {
    Serial.println("AP Config Success. Broadcasting with AP: " + String(SSID));
  }
}

// callback when data is recv from Master
void OnDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len) {
  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.print("\nMAC address : "); Serial.println(macStr);
  packetPtr = (Message*) data;
  // Serial.print("Last Packet Recv Data: "); Serial.println(*data);
  Serial.print("Seq: ");
  Serial.println((*packetPtr).seq);
  Serial.print("Timestamp: ");
  Serial.println((*packetPtr).timestamp);
  Serial.print("CRC: ");
  Serial.println((*packetPtr).crc);
  Serial.println(" ");
}

void setup() {
	delay(2000);
  // uint8_t mac[6] {0xb8, 0xd7, 0x63, 0x00, 0xfe, 0xef};
  // wifi_set_macaddr(STATION_IF, mac);

  Serial.begin(115200);
  Serial.println("ESPNow Gateway");

  // Set Custom MAC Address
  esp_base_mac_addr_set(&CustomMacAddress[0]);
  // esp_wifi_set_mac(ESP_IF_WIFI_STA, &customMacAdress[0]);

  // //Set device in AP mode to begin with
  // WiFi.mode(WIFI_AP);
  // // configure device AP mode
  // configDeviceAP();
  // // This is the mac address of the Slave in AP Mode
  // Serial.print("AP MAC: ");
  // Serial.println(WiFi.softAPmacAddress());

  //Set device in STA mode to begin with
  WiFi.mode(WIFI_STA);
  Serial.println("ESPNow Node");

  // This is the mac address of the Node in Station Mode
  Serial.print("Node MAC: ");
  Serial.println(WiFi.macAddress());

  // Init ESPNow with a fallback logic
  InitESPNow();
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info.
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
	Serial.println("Hello");
	delay(5000);
}