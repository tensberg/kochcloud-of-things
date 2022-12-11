#include "Arduino.h"

// Wifi
#include <ESP8266WiFi.h>
#include <espnow.h>

#include "messagesender.h"
#include "gatewayrobot/gatewayrobot.h"
#include "messages/stromzaehler.h"
#include "log.h"
#include "leds.h"

// global variables
volatile bool espNowDataSent;
volatile uint8_t espNowLastSendStatus;

void espNowOnDataSent(uint8_t *mac_addr, uint8_t dataSendStatus) {
  espNowDataSent = true;
  espNowLastSendStatus = dataSendStatus;
}

void initEspNowSender() {
  WiFi.mode(WIFI_STA);
  
  if (esp_now_init() != 0) {
    log("ESP Now initialization failed");
    ledSignal(true, false, 3, 250, 250);
    delay(5000);
    ESP.restart();
  }
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  int res = esp_now_add_peer(gatewayrobot_mac_address, ESP_NOW_ROLE_CONTROLLER, gatewayrobot_wifi_channel, NULL, 0);
  if (res == 0) {
    log("ESPNow paired");
  } else {
    log("ESPNow pairing failed:");
    log(res);
    ledSignal(false, true, 2, 500, 500);
  }
  // register callback function
  res = esp_now_register_send_cb(espNowOnDataSent);
  if (res != 0) {
    log("failed to register send cb: ");
    log(res);
    ledSignal(false, true, 3, 500, 500);
  }
}

bool sendDataEspNow(uint8_t* data, int len) {
  espNowDataSent = false;
  int res = esp_now_send(NULL, data, len);
  if (res != 0) {
    log("ESPNow send failed: ");
    log(res);
  }
  // wait for async data transfer to complete
  int retry = 0;
  while (!espNowDataSent && ++retry<100) {
    delay(10);
  }
  if (espNowDataSent) {
    log("ESPNow send complete");
  } else {
    log("ESPNow send failed");
  }

  return espNowDataSent;
}

