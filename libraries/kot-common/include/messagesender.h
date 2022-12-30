#pragma once

#include <ESP8266WiFi.h>

#define ESPNOW_OK 0
#define ESPNOW_ERR_INIT_FAILED 1
#define ESPNOW_ERR_PAIRING_FAILED 2
#define ESPNOW_ERR_REGISTER_CB_FAILED 3
#define ESPNOW_SEND_FAILED 4
#define ESPNOW_SEND_ACK_FAILED 5

int initEspNowSender();

int sendDataEspNow(uint8_t* data, int len);