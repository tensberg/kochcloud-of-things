#include "Arduino.h"

#include "features.h"
#include "serialstatus.h"

#ifdef ENABLE_SERIAL_STATUS
void prettyPrintTime(long seconds)
{
  Serial.print(seconds / 60 / 60 / 24);
  Serial.print("D");
  Serial.print(seconds / 60 / 60 % 24);
  Serial.print(":");
  Serial.print(seconds / 60 % 60);
  Serial.print(":");
  Serial.print(seconds % 60);
}
#endif

void printStatus(DeviceStatus &zsStatus) {
#ifdef ENABLE_SERIAL_STATUS
  Serial.print("iteration:");
  Serial.println(zsStatus.iteration);
  Serial.println("sleep time: ");
  prettyPrintTime(zsStatus.sleepTimeS);
  Serial.println();
  Serial.println("active time: ");
  prettyPrintTime(zsStatus.activeTimeS);
  Serial.print(".");
  Serial.println(zsStatus.activeTimeMicros);
  Serial.print("send success: ");
  Serial.println(zsStatus.sendSuccessCount);
  Serial.print("send failed: ");
  Serial.println(zsStatus.sendFailedCount);
#endif
}

void printData(ZisternensensorData &zsData)
{
#ifdef ENABLE_SERIAL_STATUS
    Serial.print("Temperature = ");
    Serial.print(zsData.temperature, 1);
    Serial.println(" *C");
    Serial.print("Pressure = ");
    Serial.print(zsData.pressure, 0);
    Serial.println(" hPa");
    Serial.print("Humidity = ");
    Serial.print(zsData.humidity, 0);
    Serial.println(" %");
    Serial.print("Distance = ");
    Serial.print(zsData.distance);
    Serial.println(" cm");
#endif
}