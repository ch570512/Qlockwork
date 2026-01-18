//*****************************************************************************
// Settings.cpp
//*****************************************************************************

#include "Settings.h"

Settings::Settings() {
    loadFromEEPROM();
}

Settings::~Settings() {
}

// Set defaults
void Settings::resetToDefault() {
    mySettings.magicNumber = SETTINGS_MAGIC_NUMBER;
    mySettings.version = SETTINGS_VERSION;
    mySettings.useAbc = true;
    mySettings.brightness = 100;
    mySettings.color = CYAN;
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
    mySettings.nightOffTime = 82800; // 23:00 * 3600
    mySettings.dayOnTime = 21600; // 06:00 * 3600
    mySettings.hourBeep = true;
#ifdef DEBUG
    Serial.println("Default settings set.");
#endif
    saveToEEPROM();
}

// Load settings from EEPROM
void Settings::loadFromEEPROM() {
    EEPROM.begin(512);
    EEPROM.get(0, mySettings);
    EEPROM.end();
#ifdef DEBUG
    Serial.println("Settings loaded.");
#endif
    if ((mySettings.magicNumber != SETTINGS_MAGIC_NUMBER) || (mySettings.version != SETTINGS_VERSION))
        resetToDefault();
}

// Save settings to EEPROM
void Settings::saveToEEPROM() {
    EEPROM.begin(512);
    EEPROM.put(0, mySettings);
    //EEPROM.commit();
    EEPROM.end();
#ifdef DEBUG
    Serial.println("Settings saved.");
#endif
}

