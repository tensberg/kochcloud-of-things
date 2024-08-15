#include "mqtthandler.h"

#include <math.h>
#include <Adafruit_ST7735.h>
#include <ArduinoJson.h>

#include "robotdisplay.h"
#include "leds.h"
#include "thermometer.h"

#define MESSAGE_TOPIC "message"
#define DISPLAY_TOPIC "display"
#define DISPLAY_STATE_TOPIC "display/state"
#define LEDS_TOPIC "leds"
#define LEDS_STATE_TOPIC "leds/state"
#define THERMOMETER_TOPIC "thermometer"

#define TEXT_BUFFER_SIZE 100

char text_buffer[TEXT_BUFFER_SIZE];

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

void handleDisplay(const char* topic, const byte* payload, uint16_t length)
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
    drawThermometer();
    publishDisplayState();
}

void updateLedState(JsonDocument& json, const char* key, uint8_t led)
{
    if (json.containsKey(key))
    {
        setLed(led, json[key]);
    }
}

void handleLeds(const char* topic, const byte* payload, uint16_t length) 
{
    // deserialize msgpack-encoded payload to JsonDocument 
    StaticJsonDocument<400> json;
    deserializeMsgPack(json, payload, (size_t) length);

    updateLedState(json, "u1", USER_1_LED);
    updateLedState(json, "u2", USER_2_LED);
    updateLedState(json, "u3", USER_3_LED);

    if (json.containsKey("g"))
    {
        setGlobalLedState(json["g"]);
    }
    publishLedState();
}

void publishDisplayState()
{
  StaticJsonDocument<400> json;
  if (strlen(displayState.image_name) > 0) 
  {
    json["i"] = displayState.image_name;
  }
  json["b"] = displayState.backlight;
  json["c"] = displayState.color;
  mqttPublish(DISPLAY_STATE_TOPIC, json);
}

void publishLedState()
{
  StaticJsonDocument<400> json;

  json["u1"] = getLed(USER_1_LED);
  json["u2"] = getLed(USER_2_LED);
  json["u3"] = getLed(USER_3_LED);
  json["g"] = getGlobalLedState();

  mqttPublish(LEDS_STATE_TOPIC, json);
}

void handleThermometer(const char* topic, const byte* payload, uint16_t length)
{
    // deserialize msgpack-encoded payload to JsonDocument 
    StaticJsonDocument<400> json;
    deserializeMsgPack(json, payload, (size_t) length);

    if (json.containsKey("l") && json.containsKey("t"))
    {
        Location location = json["l"];
        float temperature = json["t"];

        float minTemperature = TEMPERATURE_UNDEFINED;
        if (json.containsKey("min"))
        {
            minTemperature = json["min"];
        }
        float maxTemperature = TEMPERATURE_UNDEFINED;
        if (json.containsKey("max"))
        {
            maxTemperature = json["max"];
        }
        float delta = 0;
        if (json.containsKey("d"))
        {
            delta = json["d"];
        }

        setTemperature(location, temperature, minTemperature, maxTemperature, delta);
    }
    if (json.containsKey("r"))
    {
        JsonArray locations = json["r"].as<JsonArray>();
        for (JsonVariant location : locations)
        {
            resetMinMaxTemperature(location);
        }
    }
}

MqttSubscription subscriptions[] = 
{ 
    { MESSAGE_TOPIC, handleMessage },
    { DISPLAY_TOPIC, handleDisplay },
    { LEDS_TOPIC, handleLeds },
    { THERMOMETER_TOPIC, handleThermometer }
    // when you add a subscription here, also increase the SUBSCRIPTIONS_LEN in mqtthandler.h
};
