#include "mqtthandler.h"

#include <math.h>
#include <Adafruit_ST7735.h>

#include "robotdisplay.h"

#define TEXT_BUFFER_SIZE 100

char text_buffer[TEXT_BUFFER_SIZE];

#define MESSAGE_TOPIC "message"

void handleMessage(const char* topic, const byte* payload, uint16_t length) 
{
    // copy the payload to a buffer and add a null terminator
    memcpy(text_buffer, payload, min(length, (uint16_t) (TEXT_BUFFER_SIZE - 1)));
    text_buffer[length] = 0;
    
    drawImage();
    drawText(text_buffer, ST77XX_BLACK);
}

MqttSubscription subscriptions[] = 
{ 
    { MESSAGE_TOPIC, handleMessage } 
};
