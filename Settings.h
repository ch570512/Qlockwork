//******************************************************************************
// Settings.h
//******************************************************************************

#ifndef SETTINGS_H
#define SETTINGS_H

#include <Arduino.h>
#include <EEPROM.h>
#include "Colors.h"
#include "Configuration.h"
#include "Languages.h"
#include "Modes.h"

#define SETTINGS_MAGIC_NUMBER 0x2A
#define SETTINGS_VERSION 25

class Settings
{
public:
    Settings();
    ~Settings();

    struct MySettings
    {
        uint8_t magicNumber;
        uint8_t version;
        boolean useAbc;
        int16_t brightness;
        uint8_t color;
        uint8_t colorChange;
        uint8_t transition;
        uint8_t timeout;
        boolean modeChange;
        boolean itIs;
        boolean alarm1;
        time_t  alarm1Time;
        uint8_t alarm1Weekdays;
        boolean alarm2;
        time_t  alarm2Time;
        uint8_t alarm2Weekdays;
        time_t  nightOffTime;
        time_t  dayOnTime;
        boolean hourBeep;
    } mySettings;

    void saveToEEPROM();

private:
    void resetToDefault();
    void loadFromEEPROM();
};

#endif
