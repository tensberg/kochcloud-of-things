#include <RTCMemory.h>

#include "persistentstatus.h"
#include "log.h"

// pause between measurements
#define MICROS 1000000
#define SLEEP_TIME_MICROS 5 * 60 * MICROS

RTCMemory<DeviceStatus> deviceStatusPersistence("/etc/device_status.bin");
DeviceStatus *persistedDeviceStatus;

unsigned long lastUpdateMicros = micros();

void initPersistentDeviceStatus(DeviceStatus &deviceStatus)
{
    if (LittleFS.begin())
    {
        log("Done!");
    }
    else
    {
        log("Error");
    }

    if (deviceStatusPersistence.begin())
    {
        log("Initialization done! Previous data found.");
    }
    else
    {
        log("Initialization done! No previous data found. The buffer is cleared.");
    }

    persistedDeviceStatus = deviceStatusPersistence.getData();
    memcpy(&deviceStatus, persistedDeviceStatus, sizeof(DeviceStatus));
}

void updateAndSaveStatus(DeviceStatus &deviceStatus, boolean lastSendSuccess, unsigned long lastSleepTimeMicros)
{
    deviceStatus.iteration++;
    if (lastSendSuccess)
    {
        deviceStatus.sendSuccessCount++;
        log("packet sent successfully");
    }
    else
    {
        deviceStatus.sendFailedCount++;
        log("Packet send failed");
    }

    deviceStatus.sleepTimeS += lastSleepTimeMicros / MICROS;
    unsigned long currentUpdateMicros = micros();
    unsigned long activeTimeMicros = currentUpdateMicros - lastUpdateMicros;
    lastUpdateMicros = currentUpdateMicros;
    deviceStatus.activeTimeMicros += activeTimeMicros;
    deviceStatus.activeTimeS += deviceStatus.activeTimeMicros / MICROS;
    deviceStatus.activeTimeMicros %= MICROS;

    memcpy(persistedDeviceStatus, &deviceStatus, sizeof(DeviceStatus));
    deviceStatusPersistence.save();

    // don't backup on every iteration to preserve battery
    if (deviceStatus.iteration % 12 == 0)
    {
        deviceStatusPersistence.backup();
    }
}