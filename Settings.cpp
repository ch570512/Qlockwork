//******************************************************************************
// Settings.cpp
//******************************************************************************

#include "Settings.h"

Settings::Settings()
{
    loadFromEEPROM();
}

Settings::~Settings()
{
}

// Set all defaults
void Settings::resetToDefault()
{
    mySettings.magicNumber = SETTINGS_MAGIC_NUMBER;
    mySettings.version = SETTINGS_VERSION;
    mySettings.useAbc = false;
    mySettings.brightness = 50;
    mySettings.color = WHITE;
    mySettings.colorChange = COLORCHANGE_NO;
    mySettings.transition = TRANSITION_FADE;
    mySettings.timeout = 10;
    mySettings.modeChange = false;
    mySettings.itIs = true;
    mySettings.alarm1 = false;
    mySettings.alarm1Time = 0;
    mySettings.alarm1Weekdays = 0b11111110;
    mySettings.alarm2 = false;
    mySettings.alarm2Time = 0;
    mySettings.alarm2Weekdays = 0b11111110;
    mySettings.nightOffTime = 3600;
    mySettings.dayOnTime = 18000;
    mySettings.hourBeep = false;

    saveToEEPROM();
}

// Load settings from EEPROM
void Settings::loadFromEEPROM()
{
    EEPROM.begin(512);
    EEPROM.get(0, mySettings);
    if ((mySettings.magicNumber != SETTINGS_MAGIC_NUMBER) || (mySettings.version != SETTINGS_VERSION))
        resetToDefault();
    EEPROM.end();
}

// Write settings to EEPROM
void Settings::saveToEEPROM()
{
    Serial.println("Settings saved to EEPROM.");
    EEPROM.begin(512);
    EEPROM.put(0, mySettings);
    //EEPROM.commit();
    EEPROM.end();
}
