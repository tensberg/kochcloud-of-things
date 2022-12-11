#include "Arduino.h"

#include <espnow.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

#include "messagereceiver.h"
#include "messagehandler.h"
#include "shared/gatewayrobot/gatewayrobot.h"
#include "shared/messages/stromzaehler.h"
#include "shared/messages/zisternensensor.h"
#include "log.h"

#define MAC_ADDRESS_LEN 6
#define DEVICE_LEN 2

ZisternensensorDevice zisternensensorDevice(zisternensensor_mac_address, "zisternensensor");
StromzaehlerDevice stromzaehlerDevice(stromzaehler_mac_address, "stromzaehler");

Device* devices[DEVICE_LEN] = 
{
    &stromzaehlerDevice,
    &zisternensensorDevice,
};

void espNowDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len)
{
    for (int i = 0; i < DEVICE_LEN; i++)
    {
        Device* device = devices[i];
        if (memcmp(mac, device->macAddress, MAC_ADDRESS_LEN) == 0)
        {
            device->receiveMessage(incomingData, len);
        }
    }
}

void initEspNowReceiver()
{
    WiFi.mode(WIFI_STA);
    wifi_set_channel(gatewayrobot_wifi_channel);

    if (esp_now_init() != 0)
    {
        log("ESP Now initialization failed");
        delay(5000);
        ESP.restart();
    }

    esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
    int res = esp_now_register_recv_cb(espNowDataRecv);
    if (res != 0)
    {
        log("failed to register recv cb: ");
        log(res);
    }
    log("ESPNow initialized");
}

void espNowProcessReceivedMessages()
{
    for (int i = 0; i < DEVICE_LEN; i++)
    {
        devices[i]->handleReceivedMessage();
    }
}