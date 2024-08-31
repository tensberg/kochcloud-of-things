#include "mqtthandler.h"

#include <math.h>
#include <Adafruit_ST7735.h>

#include "robotdisplay.h"
#include "leds.h"
#include "thermometer.h"
#include "robot_to_gateway_message.pb.h"

#define TEXT_BUFFER_SIZE 100

char text_buffer[TEXT_BUFFER_SIZE];

void handleShowText(robobuf_ShowText& show_text) 
{
    uint16_t color = show_text.has_color ? show_text.color : ST77XX_BLACK;
    int16_t x = show_text.has_x ? show_text.x : -1;
    int16_t y = show_text.has_y ? show_text.y : -1;
    drawText(show_text.text, color, x, y);
}

void handleDisplay(robobuf_Display& display)
{
    if (display.has_background_color) 
    {
        fillScreen(display.background_color);
    }
    if (display.has_image) 
    {
        drawImage(display.image, display.x, display.y);
    }
    if (display.has_brightness) 
    {
        setBacklight(display.brightness);
    }
    drawThermometer();
    publishDisplayState();
}

void handleLeds(robobuf_Leds& leds) 
{
    if (leds.has_user1)
    {
        setLed(USER_1_LED, leds.user1);
    }
    if (leds.has_user2)
    {
        setLed(USER_2_LED, leds.user2);
    }
    if (leds.has_user3)
    {
        setLed(USER_3_LED, leds.user3);
    }

    if (leds.has_global)
    {
        setGlobalLedState(leds.global);
    }
    publishLedState();
}

void handleThermometer(robobuf_Thermometer& thermometer)
{
    Location location;
    switch (thermometer.location) {
        case robobuf_ThermometerLocation_LEFT:
            location = LEFT;
            break;
        
        case robobuf_ThermometerLocation_RIGHT:
            location = RIGHT;
            break;

        default:
            return;
    }

    float minTemperature = TEMPERATURE_UNDEFINED;
    if (thermometer.has_min_temperature)
    {
        minTemperature = thermometer.min_temperature;
    }
    float maxTemperature = TEMPERATURE_UNDEFINED;
    if (thermometer.has_max_temperature)
    {
        maxTemperature = thermometer.max_temperature;
    }

    setTemperature(location, thermometer.temperature, minTemperature, maxTemperature, thermometer.temperature_delta);
}

bool handleMessage(robobuf_GatewayToRobotMessage& message)
{
    switch (message.which_message) {
        case robobuf_GatewayToRobotMessage_show_text_tag:
            handleShowText(message.message.show_text);
            return true;
        
        case robobuf_GatewayToRobotMessage_display_tag:
            handleDisplay(message.message.display);
            return true;
        
        case robobuf_GatewayToRobotMessage_leds_tag:
            handleLeds(message.message.leds);
            return true;
        
        case robobuf_GatewayToRobotMessage_thermometer_tag:
            handleThermometer(message.message.thermometer);
            return true;
        
        default:
            return false;
    }
}

void publishDisplayState()
{
    robobuf_RobotToGatewayMessage message = robobuf_RobotToGatewayMessage_init_zero;
    message.which_message = robobuf_RobotToGatewayMessage_display_state_tag;
    message.message.display_state.brightness = displayState.brightness;
    message.message.display_state.background_color = displayState.background_color;
    if (displayState.image[0] != '\0') 
    {
        strlcpy(message.message.display_state.image, displayState.image, sizeof(message.message.display_state.image));
        message.message.display_state.has_image = true;
    }
    mqttPublish(message);
}

void publishLedState()
{
    robobuf_RobotToGatewayMessage message = robobuf_RobotToGatewayMessage_init_zero;
    message.which_message = robobuf_RobotToGatewayMessage_led_state_tag;
    message.message.led_state.global = getGlobalLedState();
    message.message.led_state.user1 = getLed(USER_1_LED);
    message.message.led_state.user2 = getLed(USER_2_LED);
    message.message.led_state.user3 = getLed(USER_3_LED);

    mqttPublish(message);
}
