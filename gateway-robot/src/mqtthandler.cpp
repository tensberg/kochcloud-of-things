#include "mqtthandler.h"

#include <math.h>
#include <Adafruit_ST7735.h>
#include <ArduinoJson.h>

#include "robotdisplay.h"

#define TEXT_BUFFER_SIZE 100

char text_buffer[TEXT_BUFFER_SIZE];

#define MESSAGE_TOPIC "message"
#define SCREEN_TOPIC "screen"

void handleMessage(const char* topic, const byte* payload, uint16_t length) 
{
    // deserialize msgpack-encoded payload to JsonDocument 
    StaticJsonDocument<400> json;
    deserializeMsgPack(json, payload, (size_t) length);
    
    if (!json.containsKey("m")) 
    {
        return;
    }
    const char* message = json["m"];
    uint16_t color = ST77XX_BLACK;
    if (json.containsKey("c"))
    {
        color = json["c"];
    }
    int16_t x = -1;
    if (json.containsKey("x"))
    {
        x = json["x"];
    }
    int16_t y = -1;
    if (json.containsKey("y"))
    {
        y = json["y"];
    }

    drawText(message, color, x, y);
}

void handleScreen(const char* topic, const byte* payload, uint16_t length)
{
    // deserialize msgpack-encoded payload to JsonDocument 
    StaticJsonDocument<400> json;
    deserializeMsgPack(json, payload, (size_t) length);
    
    if (json.containsKey("c")) 
    {
        uint16_t color = json["c"];
        fillScreen(color);
    }
    if (json.containsKey("i")) 
    {
        const char* image = json["i"];
        int16_t x = 0;
        if (json.containsKey("x"))
        {
            x = json["x"];
        }
        int16_t y = 0;
        if (json.containsKey("y"))
        {
            y = json["y"];
        }
        drawImage(image, x, y);
    }
    if (json.containsKey("b")) 
    {
        uint8_t brightness = json["b"];
        setBacklight(brightness);
    }
}

MqttSubscription subscriptions[] = 
{ 
    { MESSAGE_TOPIC, handleMessage },
    { SCREEN_TOPIC, handleScreen }
};
