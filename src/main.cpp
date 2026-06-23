//=============================================================================
// QLOCKWORK
// @created 12.03.2017
// @mc ESP8266
//
// This source file is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This source file is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//=============================================================================

#if !defined(ESP8266)
#error This code is designed to run on ESP8266-based boards! Please check your Tools->Board setting.
#endif

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <IRremoteESP8266.h>
#include <IRutils.h>
#include <time.h>
#include "Debug.h"
#include "Colors.h"
#include "Configuration.h"
#include "Events.h"
#include "Helpers.h"
#include "LedDriver.h"
#include "MeteoWeather.h"
#include "Modes.h"
#include "Renderer.h"
#include "Settings.h"
#include "WebServer.h"

// Event definitions
event_t events[] = {
    {0, 0, "", 0, WHITE}, // Do not remove
    {1, 1, "Happy New Year!", 0, YELLOW_25},
    {3, 14, "Albert Einsteins birthday!", 1879, MAGENTA},
    {12, 24, "Merry Christmas!", 0, RED}};

void buttonModeInterrupt();
void buttonModePressed();
void buttonOnOffInterrupt();
void buttonOnOffPressed();
void buttonTimeInterrupt();
void buttonTimePressed();
uint8_t getBrightnessFromLDR();
void getRoomConditions();
void moveScreenBufferUp(uint16_t screenBufferOld[], uint16_t screenBufferNew[], uint8_t color, uint8_t brightness);
void setLedsOff();
void setLedsOn();
void setMode(Mode newMode);
void writeScreenBuffer(uint16_t screenBuffer[], uint8_t color, uint8_t brightness);
void writeScreenBufferFade(uint16_t screenBufferOld[], uint16_t screenBufferNew[], uint8_t color, uint8_t brightness);

//=============================================================================
// Init
//=============================================================================

// HTTP-Server
#ifdef WEBSERVER
WebServerClass webServer;
#endif

// DHT22
#ifdef SENSOR_DHT22
DHT dht(PIN_DHT22, DHT22);
#endif

// IR receiver
#ifdef IR_RECEIVER
IRrecv irrecv(PIN_IR_RECEIVER);
decode_results irDecodeResult;
#endif

// LED driver
LedDriver ledDriver;

// Renderer
Renderer renderer;

// Settings
Settings settings;

// Screenbuffer
uint16_t matrix[10] = {};
uint16_t matrixOld[10] = {};
bool screenBufferNeedsUpdate = true;

// Mode
Mode mode = MODE_TIME;
Mode lastMode = mode;
uint32_t modeTimeout = 0;
uint32_t autoModeChangeTimer = AUTO_MODECHANGE_TIME;
bool runTransitionOnce = false;
uint8_t autoMode = 0;

// Time
uint8_t lastDay = 0;
uint8_t lastMinute = 0;
uint8_t lastHour = 0;
uint8_t lastSecond = 0;
uint8_t randomHour = 0;
uint8_t randomMinute = 0;
uint8_t moonphase = 0;
time_t upTime = 0;
const char *dayOfWeek[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
const char *monthOfYear[] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"};

// Feed
String feedText = "";
uint8_t feedColor = WHITE;
uint8_t feedPosition = 0;

// MeteoWeather
#ifdef WEATHER
MeteoWeather outdoorWeather;
uint8_t errorCounterOutdoorWeather = 0;
#endif

// DHT22
float roomTemperature = 0;
float roomHumidity = 0;
uint8_t errorCounterDHT = 0;

// Brightness and LDR
uint8_t maxBrightness = 0;
uint8_t brightness = 0;
#ifdef LDR
uint8_t iTargetBrightness = 0;
unsigned long iBrightnessMillis = 0;
#endif

// Alarm
#ifdef BUZZER
bool alarmTimerSet = false;
uint8_t alarmTimer = 0;
uint8_t alarmTimerSecond = 0;
uint8_t alarmOn = false;
#endif

// Events
#ifdef EVENT_TIME
uint32_t showEventTimer = EVENT_TIME;
#endif

// Misc
IPAddress myIP = {0, 0, 0, 0};
uint32_t lastButtonPress = 0;
bool testFlag = false;
uint8_t testColumn = 0;
#ifdef DEBUG_FPS
int fps = 0;
#endif

//=============================================================================
// Setup()
//=============================================================================

void setup()
{
    // Init serial port
    Serial.begin(115200);
    delay(1000);

    // And the monkey flips the switch. (Akiva Goldsman)
    DEBUG_SERIAL_PRINTLN(F("*** QLOCKWORK ***"));
    DEBUG_SERIAL_PRINTLN("Firmware: " + String(FIRMWARE_VERSION));
    DEBUG_SERIAL_PRINTLN("Starting on " + String(ARDUINO_BOARD));
    DEBUG_SERIAL_PRINTLN("CPU Frequency = " + String(F_CPU / 1000000) + " MHz");

    // Load settings
    settings.loadFromEEPROM();
    maxBrightness = map(settings.mySettings.brightness, 0, 100, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
    brightness = maxBrightness;

#ifdef POWERON_SELFTEST
    renderer.setAllScreenBuffer(matrix);
    DEBUG_SERIAL_PRINTLN(F("Set all LEDs to red."));
    writeScreenBuffer(matrix, RED, brightness);
    delay(2500);
    DEBUG_SERIAL_PRINTLN(F("Set all LEDs to green."));
    writeScreenBuffer(matrix, GREEN, brightness);
    delay(2500);
    DEBUG_SERIAL_PRINTLN(F("Set all LEDs to blue."));
    writeScreenBuffer(matrix, BLUE, brightness);
    delay(2500);
    DEBUG_SERIAL_PRINTLN(F("Set all LEDs to white."));
    writeScreenBuffer(matrix, WHITE, brightness);
    delay(2500);
#endif

#ifdef ESP_LED
    DEBUG_SERIAL_PRINTLN(F("Setting up ESP-LED"));
    pinMode(PIN_LED, OUTPUT);
    digitalWrite(PIN_LED, HIGH);
#endif

#ifdef MODE_BUTTON
    DEBUG_SERIAL_PRINTLN(F("Setting up Mode-Button."));
    pinMode(PIN_MODE_BUTTON, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_MODE_BUTTON), buttonModeInterrupt, FALLING);
#endif

#ifdef ONOFF_BUTTON
    DEBUG_SERIAL_PRINTLN(F("Setting up Back-Button."));
    pinMode(PIN_ONOFF_BUTTON, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_ONOFF_BUTTON), buttonOnOffInterrupt, FALLING);
#endif

#ifdef TIME_BUTTON
    DEBUG_SERIAL_PRINTLN(F("Setting up Time-Button."));
    pinMode(PIN_TIME_BUTTON, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_TIME_BUTTON), buttonTimeInterrupt, FALLING);
#endif

#ifdef BUZZER
    DEBUG_SERIAL_PRINTLN(F("Setting up Buzzer"));
    pinMode(PIN_BUZZER, OUTPUT);
#endif

#ifdef SENSOR_DHT22
    DEBUG_SERIAL_PRINTLN(F("Setting up DHT22"));
    dht.begin();
#endif

#ifdef LDR
    DEBUG_SERIAL_PRINTLN(F("Setting up LDR."));
    pinMode(PIN_LDR, INPUT);
#endif

#ifdef IR_RECEIVER
    DEBUG_SERIAL_PRINTLN(F("Setting up IR-Receiver"));
    irrecv.enableIRIn();
#endif

    // Start WiFi
    renderer.clearScreenBuffer(matrix);
    renderer.setSmallText("WI", TEXT_POS_TOP, matrix);
    renderer.setSmallText("FI", TEXT_POS_BOTTOM, matrix);
    writeScreenBuffer(matrix, WHITE, brightness);
    WiFiManager wifiManager;
    wifiManager.setConnectTimeout(30);
    wifiManager.setTimeout(120);
    wifiManager.autoConnect(HOSTNAME, WIFI_AP_PASS);
    WiFi.hostname(HOSTNAME);
    WiFi.setAutoConnect(true);
    WiFi.setAutoReconnect(true);
    if (!WiFi.isConnected())
    {
        WiFi.mode(WIFI_AP);
        DEBUG_SERIAL_PRINTLN(F("[ERROR] No WiFi"));
        writeScreenBuffer(matrix, RED, brightness);
        delay(1000);
        if (WiFi.softAP(String(HOSTNAME) + " AP", WIFI_AP_PASS))
        {
            DEBUG_SERIAL_PRINTLN(F("AP started with IP: "));
            myIP = WiFi.softAPIP();
            DEBUG_SERIAL_PRINTLN(myIP);
        }
        else
        {
            DEBUG_SERIAL_PRINTLN("[ERROR] No AP");
        }
    }
    else
    {
        WiFi.mode(WIFI_STA);
        DEBUG_SERIAL_PRINTLN("Hostname: " + String(HOSTNAME));
        DEBUG_SERIAL_PRINTLN("RSSI: " + String(WiFi.RSSI()));
        writeScreenBuffer(matrix, GREEN, brightness);
        delay(1000);
        myIP = WiFi.localIP();

        // Configure NTP-Client
        configTime(NTP_TIMEZONE, NTP_SERVER);
        DEBUG_SERIAL_PRINTLN("NTP Server: " + String(NTP_SERVER));
        DEBUG_SERIAL_PRINTLN("NTP Timezone: " + String(NTP_TIMEZONE));
        time_t now_time_t = time(nullptr);
        while (now_time_t < 8 * 3600 * 2)
        {
            delay(500);
            now_time_t = time(nullptr);
            DEBUG_SERIAL_PRINTLN(".");
        }

#ifdef ARDUINO_OTA
        DEBUG_SERIAL_PRINTLN(F("Starting OTA"));
        ArduinoOTA.begin();
#endif

        // Get weather from MeteoWeather
#ifdef WEATHER
        !outdoorWeather.getOutdoorConditions(LATITUDE, LONGITUDE, TIMEZONE) ? errorCounterOutdoorWeather++ : errorCounterOutdoorWeather = 0;
#endif
    }

#ifdef WEBSERVER
    DEBUG_SERIAL_PRINTLN(F("Starting webserver"));
    webServer.setup();
#endif

#ifdef SHOW_IP
    // WiFi.isConnected() ? feedText = "  IP: " : feedText = "  AP-IP: ";
    feedText = "  IP: ";
    feedText += String(myIP[0]) + "." + String(myIP[1]) + "." + String(myIP[2]) + "." + String(myIP[3]) + "   ";
    feedPosition = 0;
    feedColor = WHITE;
    mode = MODE_FEED;
#endif

    renderer.clearScreenBuffer(matrix);

    // Define a random time
    randomSeed(analogRead(A0));
    randomHour = random(0, 24);
    randomMinute = random(5, 56);

    // Print some infos
#ifdef DEBUG
#ifdef EVENT_TIME
    Serial.printf("Events: %u\n", sizeof(events) / sizeof(event_t) - 1);
#endif
    Serial.printf("Day on: %02u:%02u\n", getHour(settings.mySettings.dayOnTime), getMinute(settings.mySettings.dayOnTime));
    Serial.printf("Night off: %02u:%02u\n", getHour(settings.mySettings.nightOffTime), getMinute(settings.mySettings.nightOffTime));
    Serial.printf("Alarm1: %02u:%02u\n", getHour(settings.mySettings.alarm1Time), getMinute(settings.mySettings.alarm1Time));
    settings.mySettings.alarm1 ? Serial.print("on ") : Serial.print("off ");
    Serial.println(settings.mySettings.alarm1Weekdays, BIN);
    Serial.printf("Alarm2: %02u:%02u\n", getHour(settings.mySettings.alarm2Time), getMinute(settings.mySettings.alarm2Time));
    settings.mySettings.alarm2 ? Serial.print("on ") : Serial.print("off ");
    Serial.println(settings.mySettings.alarm2Weekdays, BIN);
    Serial.printf("Random time: %02u:%02u\n", randomHour, randomMinute);
    Serial.println(F("DEBUG enabled"));
#else
    Serial.println(F("DEBUG disabled"));
#endif

#ifdef FRONTCOVER_BINARY
    settings.mySettings.transition = TRANSITION_NORMAL;
#endif
}

//=============================================================================
// Loop()
//=============================================================================

void loop()
{
    // Call HTTP-handle
#ifdef WEBSERVER
    webServer.handle();
#endif

    // Call OTA-handle
#ifdef ARDUINO_OTA
    ArduinoOTA.handle();
#endif

    // Get the time from ESP
    struct tm tmNow = getTime();

    //=============================================================================
    // Run once a day
    //=============================================================================

    if (tmNow.tm_mday != lastDay)
    {
        lastDay = tmNow.tm_mday;
        screenBufferNeedsUpdate = true;

#ifdef SHOW_MODE_MOONPHASE
        int currentYear = tmNow.tm_year + 1900; // Convert 124 -> 2024
        int currentMonth = tmNow.tm_mon + 1;    // Convert 9 -> 10 (October)
        int currentDay = tmNow.tm_mday;         // This is already 1-31
        moonphase = getMoonphase(currentYear, currentMonth, currentDay);
#endif

        // Reset URL event 0
        events[0].day = 0;
        events[0].month = 0;

        // Change color
        if (settings.mySettings.colorChange == COLORCHANGE_DAY)
        {
            settings.mySettings.color = random(0, COLORCHANGE_COUNT + 1);
#ifdef DEBUG
            Serial.printf("Color changed to: %u\n", settings.mySettings.color);
#endif
        }
    }

    //=============================================================================
    // Run once every hour
    //=============================================================================

    if (tmNow.tm_hour != lastHour)
    {
        lastHour = tmNow.tm_hour;
        screenBufferNeedsUpdate = true;

        // Change color
        if (settings.mySettings.colorChange == COLORCHANGE_HOUR)
        {
            settings.mySettings.color = random(0, COLOR_COUNT + 1);
#ifdef DEBUG
            Serial.printf("Color changed to: %u\n", (int)settings.mySettings.color);
#endif
        }

        // Hourly beep
#ifdef BUZZER
        if ((settings.mySettings.hourBeep == true) && (mode == MODE_TIME))
        {
            digitalWrite(PIN_BUZZER, HIGH);
            delay(25);
            digitalWrite(PIN_BUZZER, LOW);
            DEBUG_SERIAL_PRINTLN("Beep!");
        }
#endif

        //=============================================================================
        // Run once every random hour (therefore once a day)
        //=============================================================================

        if (tmNow.tm_hour == randomHour)
        {
            randomHour = tmNow.tm_hour;
        }
    }

    //=============================================================================
    // Run once every minute
    //=============================================================================

    if (tmNow.tm_min != lastMinute)
    {
        lastMinute = tmNow.tm_min;
        screenBufferNeedsUpdate = true;

#if defined(SENSOR_DHT22)
        // Update room conditions
        getRoomConditions();
#endif

#ifdef BUZZER
        // Switch on buzzer for alarm 1
        if (settings.mySettings.alarm1 && (tmNow.tm_hour == getHour(settings.mySettings.alarm1Time)) && (tmNow.tm_min == getMinute(settings.mySettings.alarm1Time)) && bitRead(settings.mySettings.alarm1Weekdays, tmNow.tm_wday))
        {
            alarmOn = BUZZTIME_ALARM_1;
            DEBUG_SERIAL_PRINTLN(F("Alarm1 on"));
        }

        // Switch on buzzer for alarm 2
        if (settings.mySettings.alarm2 && (tmNow.tm_hour == getHour(settings.mySettings.alarm2Time)) && (tmNow.tm_min == getMinute(settings.mySettings.alarm1Time)) && bitRead(settings.mySettings.alarm2Weekdays, tmNow.tm_wday))
        {
            alarmOn = BUZZTIME_ALARM_2;
            DEBUG_SERIAL_PRINTLN(F("Alarm2 on"));
        }
#endif

        // Set night- and daymode
        if ((tmNow.tm_hour == getHour(settings.mySettings.nightOffTime)) && (tmNow.tm_min == getMinute(settings.mySettings.nightOffTime)))
        {
            DEBUG_SERIAL_PRINTLN(F("Night off"));
            setMode(MODE_BLANK);
        }
        if ((tmNow.tm_hour == getHour(settings.mySettings.dayOnTime)) && (tmNow.tm_min == getMinute(settings.mySettings.dayOnTime)))
        {
            DEBUG_SERIAL_PRINTLN(F("Day on"));
            setMode(lastMode);
        }

        //=============================================================================
        // Run once every random minute (once an hour)
        //=============================================================================

        if (tmNow.tm_min == randomMinute)
        {
#ifdef WEATHER
            if (WiFi.isConnected())
            {
                // Get weather from MeteoWeather
                !outdoorWeather.getOutdoorConditions(LATITUDE, LONGITUDE, TIMEZONE) ? errorCounterOutdoorWeather++ : errorCounterOutdoorWeather = 0;
            }
#endif
        }

        //=============================================================================
        // Run once every 5 minutes
        //=============================================================================

        if (tmNow.tm_min % 5 == 0)
        {
            // Change color
            if (settings.mySettings.colorChange == COLORCHANGE_FIVE)
            {
                settings.mySettings.color = random(0, COLOR_COUNT + 1);
#ifdef DEBUG
                Serial.printf("Color changed to: %u\n", settings.mySettings.color);
#endif
            }
        }
    }

    //=============================================================================
    // Run once every second
    //=============================================================================

    if (tmNow.tm_sec != lastSecond)
    {
        lastSecond = tmNow.tm_sec;
        upTime++;

#ifdef DEBUG_FPS
        Serial.printf("FPS: %u\n", fps);
        fps = 0;
#endif
#ifdef DEBUG_LDR
        // Serial.printf("LDR:        min: %d max: %d actual: %d\n", minLdrValue, maxLdrValue, ldrValue);
        Serial.printf("Brightness: min: %d max: %d target: %d actual: %d\n", MIN_BRIGHTNESS, maxBrightness, iTargetBrightness, brightness);
#endif

#ifdef BUZZER
        // Make some noise
        if (alarmOn)
        {
            alarmOn--;
            digitalRead(PIN_BUZZER) ? digitalWrite(PIN_BUZZER, LOW) : digitalWrite(PIN_BUZZER, HIGH);
            if (!alarmOn)
            {
                DEBUG_SERIAL_PRINTLN(F("Alarm: off"));
                digitalWrite(PIN_BUZZER, LOW);
                screenBufferNeedsUpdate = true;
            }
        }
#endif

#ifdef FRONTCOVER_BINARY
        if (mode != MODE_BLANK)
            screenBufferNeedsUpdate = true;
#else
        // General Screenbuffer-Update every second.
        // (not in MODE_TIME or MODE_BLANK because it will lock the ESP due to TRANSITION_FADE)
        if ((mode != MODE_TIME) && (mode != MODE_BLANK))
        {
            screenBufferNeedsUpdate = true;
        }
#endif

        // Flash ESP LED
#ifdef ESP_LED
        digitalWrite(PIN_LED, !digitalRead(PIN_LED));
#endif

        // Countdown timeralarm by one minute in the second it was activated
#ifdef BUZZER
        if (alarmTimer && alarmTimerSet && (alarmTimerSecond == tmNow.tm_sec))
        {
            alarmTimer--;
#ifdef DEBUG
            if (alarmTimer)
            {
                Serial.printf("Timeralarm in %u min.\n", alarmTimer);
            }
#endif
        }
        // Switch on buzzer for timer
        if (!alarmTimer && alarmTimerSet)
        {
            alarmTimerSet = false;
            alarmOn = BUZZTIME_TIMER;
            DEBUG_SERIAL_PRINTLN(F("Timeralarm: on"));
        }
#endif

        // Auto switch modes
        if (settings.mySettings.modeChange && (mode == MODE_TIME))
        {
            autoModeChangeTimer--;
            if (!autoModeChangeTimer)
            {
                DEBUG_SERIAL_PRINTLN(F("Auto modechange (") + String(autoMode) + F(")"));
                autoModeChangeTimer = AUTO_MODECHANGE_TIME;
                switch (autoMode)
                {
                case 0:
#ifdef WEATHER
                    if (WiFi.isConnected())
                    {
                        setMode(MODE_EXT_TEMP);
                    }
                    else
                    {
                        WiFi.reconnect();
                        setMode(MODE_EXT_TEMP);
                    }
#endif
                    autoMode = 1;
                    break;
                case 1:
#if defined(SENSOR_DHT22)
                    setMode(MODE_TEMP);
#else
#ifdef WEATHER
                    if (WiFi.isConnected())
                    {
                        setMode(MODE_EXT_TEMP);
                    }
                    else
                    {
                        WiFi.reconnect();
                        setMode(MODE_EXT_TEMP);
                    }
#endif
#endif
                    autoMode = 0;
                    break;
                }
            }
        }

        // Show event in feed
#ifdef EVENT_TIME
        if (mode == MODE_TIME)
        {
            showEventTimer--;
            if (!showEventTimer)
            {
                showEventTimer = EVENT_TIME;
                for (uint8_t i = 0; i < (sizeof(events) / sizeof(event_t)); i++)
                {
                    if ((tmNow.tm_mday == events[i].day) && (tmNow.tm_mon == events[i].month))
                    {
                        if (events[i].year)
                        {
                            feedText = "  " + events[i].text + " (" + String(tmNow.tm_year - events[i].year) + ")   ";
                        }
                        else
                        {
                            feedText = "  " + events[i].text + "   ";
                        }
                        feedPosition = 0;
                        feedColor = events[i].color;
                        DEBUG_SERIAL_PRINTLN("Event: \"" + feedText + "\"");
                        setMode(MODE_FEED);
                    }
                }
            }
        }
#endif
    }

    //=============================================================================
    // Run always
    //=============================================================================

    // Make sure the textfeed is updated
    if (mode == MODE_FEED)
        screenBufferNeedsUpdate = true;

    // Set brightness from LDR and update display at 25Hz
#ifdef LDR
    if (settings.mySettings.useAbc)
    {
        if ((millis() - iBrightnessMillis >= 40) && !testFlag)
        {
            iBrightnessMillis = millis();
            iTargetBrightness = getBrightnessFromLDR();
            if (brightness < iTargetBrightness)
            {
                brightness++;
                writeScreenBuffer(matrix, settings.mySettings.color, brightness);
            }
            if (brightness > iTargetBrightness)
            {
                brightness--;
                writeScreenBuffer(matrix, settings.mySettings.color, brightness);
            }
        }
    }
#endif

    // Watch out for IR commands and handle them accordingly
#ifdef IR_RECEIVER
    if (irrecv.decode(&irDecodeResult))
    {
#ifdef DEBUG_IR
        Serial.print(F("IR signal: 0x"));
        serialPrintUint64(irDecodeResult.value, HEX);
        Serial.println();
#endif
        switch (irDecodeResult.value)
        {
        case IR_CODE_ONOFF:
            buttonOnOffPressed();
            break;
        case IR_CODE_TIME:
            buttonTimePressed();
            break;
        case IR_CODE_MODE:
            buttonModePressed();
            break;
        }
        irrecv.resume();
    }
#endif

    // Render a new screenbuffer if needed
    if (screenBufferNeedsUpdate)
    {
        screenBufferNeedsUpdate = false;

        // Save old screenbuffer (or not if it's the test pattern)
        if (testFlag)
        {
            for (uint8_t i = 0; i <= 9; i++)
            {
                matrixOld[i] = 0;
            }
            testFlag = false;
        }
        else
            for (uint8_t i = 0; i <= 9; i++)
            {
                matrixOld[i] = matrix[i];
            }

        switch (mode)
        {
        case MODE_TIME:
            renderer.clearScreenBuffer(matrix);

#ifdef FRONTCOVER_BINARY
            matrix[0] = 0b1111000000000000;
            matrix[1] = tmNow.tm_hour << 5;
            matrix[2] = tmNow.tm_min << 5;
            matrix[3] = tmNow.tm_sec << 5;
            matrix[5] = 0b1111000000000000;
            matrix[6] = tmNow.tm_mday << 5;
            matrix[7] = tmNow.tm_mon + 1 << 5;
            matrix[8] = tmNow.tm_year + 1900 << 5;
#else
            renderer.setTime(tmNow.tm_hour, tmNow.tm_min, matrix);
            renderer.setCorners(tmNow.tm_min, matrix);
            if (!settings.mySettings.itIs && ((tmNow.tm_min / 5) % 6))
                renderer.clearEntryWords(matrix);
#endif
#ifdef BUZZER
            if (settings.mySettings.alarm1 || settings.mySettings.alarm2 || alarmTimerSet)
                renderer.setAlarmLed(matrix);
#endif
            break;

#ifdef SHOW_MODE_AMPM
        case MODE_AMPM:
            renderer.clearScreenBuffer(matrix);
            tmNow.tm_hour < 12 ? renderer.setSmallText("AM", TEXT_POS_MIDDLE, matrix) : renderer.setSmallText("PM", TEXT_POS_MIDDLE, matrix);
            break;
#endif

#ifdef SHOW_MODE_SECONDS
        case MODE_SECONDS:
            renderer.clearScreenBuffer(matrix);
            renderer.setCorners(tmNow.tm_min, matrix);
            for (uint8_t i = 0; i <= 6; i++)
            {
                matrix[1 + i] |= numbersBig[tmNow.tm_sec / 10][i] << 11;
                matrix[1 + i] |= numbersBig[tmNow.tm_sec % 10][i] << 5;
            }
            break;
#endif

#ifdef SHOW_MODE_WEEKDAY
        case MODE_WEEKDAY:
            renderer.clearScreenBuffer(matrix);
            renderer.setSmallText(String(sWeekday[tmNow.tm_wday][0]) + String(sWeekday[tmNow.tm_wday][1]), TEXT_POS_MIDDLE, matrix);
            break;
#endif

#ifdef SHOW_MODE_DATE
        case MODE_DATE:
            renderer.clearScreenBuffer(matrix);

            // Create buffers to hold the formatted strings (max 3 chars: "01")
            char dayBuf[4];
            char monBuf[4];

            // Use snprintf for safe, heap-free formatting
            // %02d means: integer, at least 2 digits wide, pad with '0'
            snprintf(dayBuf, sizeof(dayBuf), "%02d", (uint8_t)tmNow.tm_mday);
            snprintf(monBuf, sizeof(monBuf), "%02d", (uint8_t)tmNow.tm_mon + 1);

            // Render the pre-formatted strings
            renderer.setSmallText(dayBuf, TEXT_POS_TOP, matrix);
            renderer.setSmallText(monBuf, TEXT_POS_BOTTOM, matrix);

            // Manual pixel overrides
            renderer.setPixelInScreenBuffer(5, 4, matrix);
            renderer.setPixelInScreenBuffer(5, 9, matrix);
            break;

#endif

#ifdef SHOW_MODE_MOONPHASE
        case MODE_MOONPHASE:
            renderer.clearScreenBuffer(matrix);
            switch (moonphase)
            {
            case 0:
                matrix[0] = 0b0000111000000000;
                matrix[1] = 0b0011000110000000;
                matrix[2] = 0b0100000001000000;
                matrix[3] = 0b0100000001000000;
                matrix[4] = 0b1000000000100000;
                matrix[5] = 0b1000000000100000;
                matrix[6] = 0b0100000001000000;
                matrix[7] = 0b0100000001000000;
                matrix[8] = 0b0011000110000000;
                matrix[9] = 0b0000111000000000;
                break;
            case 1:
                matrix[0] = 0b0000111000000000;
                matrix[1] = 0b0000001110000000;
                matrix[2] = 0b0000000111000000;
                matrix[3] = 0b0000000111000000;
                matrix[4] = 0b0000000111100000;
                matrix[5] = 0b0000000111100000;
                matrix[6] = 0b0000000111000000;
                matrix[7] = 0b0000000111000000;
                matrix[8] = 0b0000001110000000;
                matrix[9] = 0b0000111000000000;
                break;
            case 2:
                matrix[0] = 0b0000011000000000;
                matrix[1] = 0b0000011110000000;
                matrix[2] = 0b0000011111000000;
                matrix[3] = 0b0000011111000000;
                matrix[4] = 0b0000011111100000;
                matrix[5] = 0b0000011111100000;
                matrix[6] = 0b0000011111000000;
                matrix[7] = 0b0000011111000000;
                matrix[8] = 0b0000011110000000;
                matrix[9] = 0b0000011000000000;
                break;
            case 3:
                matrix[0] = 0b0000111000000000;
                matrix[1] = 0b0001111110000000;
                matrix[2] = 0b0001111111000000;
                matrix[3] = 0b0001111111000000;
                matrix[4] = 0b0001111111100000;
                matrix[5] = 0b0001111111100000;
                matrix[6] = 0b0001111111000000;
                matrix[7] = 0b0001111111000000;
                matrix[8] = 0b0001111110000000;
                matrix[9] = 0b0000111000000000;
                break;
            case 4:
                matrix[0] = 0b0000111000000000;
                matrix[1] = 0b0011111110000000;
                matrix[2] = 0b0111111111000000;
                matrix[3] = 0b0111111111000000;
                matrix[4] = 0b1111111111100000;
                matrix[5] = 0b1111111111100000;
                matrix[6] = 0b0111111111000000;
                matrix[7] = 0b0111111111000000;
                matrix[8] = 0b0011111110000000;
                matrix[9] = 0b0000111000000000;
                break;
            case 5:
                matrix[0] = 0b0000111000000000;
                matrix[1] = 0b0011111100000000;
                matrix[2] = 0b0111111100000000;
                matrix[3] = 0b0111111100000000;
                matrix[4] = 0b1111111100000000;
                matrix[5] = 0b1111111100000000;
                matrix[6] = 0b0111111100000000;
                matrix[7] = 0b0111111100000000;
                matrix[8] = 0b0011111100000000;
                matrix[9] = 0b0000111000000000;
                break;
            case 6:
                matrix[0] = 0b0000110000000000;
                matrix[1] = 0b0011110000000000;
                matrix[2] = 0b0111110000000000;
                matrix[3] = 0b0111110000000000;
                matrix[4] = 0b1111110000000000;
                matrix[5] = 0b1111110000000000;
                matrix[6] = 0b0111110000000000;
                matrix[7] = 0b0111110000000000;
                matrix[8] = 0b0011110000000000;
                matrix[9] = 0b0000110000000000;
                break;
            case 7:
                matrix[0] = 0b0000111000000000;
                matrix[1] = 0b0011100000000000;
                matrix[2] = 0b0111000000000000;
                matrix[3] = 0b0111000000000000;
                matrix[4] = 0b1111000000000000;
                matrix[5] = 0b1111000000000000;
                matrix[6] = 0b0111000000000000;
                matrix[7] = 0b0111000000000000;
                matrix[8] = 0b0011100000000000;
                matrix[9] = 0b0000111000000000;
                break;
            }
            break;
#endif

#ifdef SENSOR_DHT22
        case MODE_TEMP:
            DEBUG_SERIAL_PRINTLN(F("Room Temperature: ") + String(roomTemperature) + F(" °C"));
            renderer.clearScreenBuffer(matrix);
            if (roomTemperature == 0)
            {
                matrix[0] = 0b0000000001000000;
                matrix[1] = 0b0000000010100000;
                matrix[2] = 0b0000000010100000;
                matrix[3] = 0b0000000011100000;
            }
            if (roomTemperature > 0)
            {
                matrix[0] = 0b0000000001000000;
                matrix[1] = 0b0100000010100000;
                matrix[2] = 0b1110000010100000;
                matrix[3] = 0b0100000011100000;
            }
            if (roomTemperature < 0)
            {
                matrix[0] = 0b0000000001000000;
                matrix[1] = 0b0000000010100000;
                matrix[2] = 0b1110000010100000;
                matrix[3] = 0b0000000011100000;
            }
            renderer.setSmallText(String(int(abs(roomTemperature) + 0.5)), TEXT_POS_BOTTOM, matrix);
            break;

        case MODE_HUMIDITY:
            DEBUG_SERIAL_PRINTLN(F("Room Humidity: ") + String(roomHumidity) + F(" %rH"));
            renderer.clearScreenBuffer(matrix);
            renderer.setSmallText(String(int(roomHumidity + 0.5)), TEXT_POS_TOP, matrix);
            matrix[6] = 0b0100100001000000;
            matrix[7] = 0b0001000010100000;
            matrix[8] = 0b0010000010100000;
            matrix[9] = 0b0100100011100000;
            break;
#endif

#ifdef WEATHER
        case MODE_EXT_TEMP:
            DEBUG_SERIAL_PRINTLN(F("Outdoor temperature: ") + String(outdoorWeather.temperature) + F(" °C"));
            renderer.clearScreenBuffer(matrix);
            if (outdoorWeather.temperature > 0)
            {
                matrix[1] = 0b0100000000000000;
                matrix[2] = 0b1110000000000000;
                matrix[3] = 0b0100000000000000;
            }
            if (outdoorWeather.temperature < 0)
                matrix[2] = 0b1110000000000000;
            renderer.setSmallText(String(int(abs(outdoorWeather.temperature) + 0.5)), TEXT_POS_BOTTOM, matrix);
            break;
        case MODE_EXT_HUMIDITY:
            DEBUG_SERIAL_PRINTLN(F("Outdoor humidity: ") + String(outdoorWeather.humidity) + F(" %rH"));
            renderer.clearScreenBuffer(matrix);
            if (outdoorWeather.humidity < 100)
                renderer.setSmallText(String(outdoorWeather.humidity), TEXT_POS_TOP, matrix);
            else
            {
                matrix[0] = 0b0010111011100000;
                matrix[1] = 0b0110101010100000;
                matrix[2] = 0b0010101010100000;
                matrix[3] = 0b0010101010100000;
                matrix[4] = 0b0010111011100000;
            }
            matrix[6] = 0b0100100000000000;
            matrix[7] = 0b0001000000000000;
            matrix[8] = 0b0010000000000000;
            matrix[9] = 0b0100100000000000;
            break;
#endif

#ifdef BUZZER
        case MODE_TIMER:
            renderer.clearScreenBuffer(matrix);
            renderer.setSmallText("TI", TEXT_POS_TOP, matrix);
            renderer.setSmallText(String(alarmTimer), TEXT_POS_BOTTOM, matrix);
            break;
#endif

#ifdef SHOW_MODE_TEST
        case MODE_TEST:
            renderer.clearScreenBuffer(matrix);
            if (testColumn == 10)
                testColumn = 0;
            matrix[testColumn] = 0b1111111111110000;
            testColumn++;
            break;
        case MODE_RED:
        case MODE_GREEN:
        case MODE_BLUE:
        case MODE_WHITE:
            renderer.setAllScreenBuffer(matrix);
            testFlag = true;
            break;
#endif

        case MODE_COUNT:
        {
        }

        case MODE_BLANK:
            renderer.clearScreenBuffer(matrix);
            break;

        case MODE_FEED:
            for (uint8_t y = 0; y <= 5; y++)
            {
                renderer.clearScreenBuffer(matrix);
                for (uint8_t z = 0; z <= 6; z++)
                {
                    matrix[2 + z] |= (lettersBig[feedText[feedPosition] - 32][z] << (11 + y)) & 0b1111111111100000;
                    matrix[2 + z] |= (lettersBig[feedText[feedPosition + 1] - 32][z] << (5 + y)) & 0b1111111111100000;
                    matrix[2 + z] |= (lettersBig[feedText[feedPosition + 2] - 32][z] << (y - 1)) & 0b1111111111100000;
                }
                writeScreenBuffer(matrix, feedColor, brightness);
                delay(120);
            }
            feedPosition++;
            if (feedPosition == feedText.length() - 2)
            {
                feedPosition = 0;
                setMode(MODE_TIME);
            }
            break;
        }

        // turn off LED behind IR-sensor
#ifdef IR_LETTER_OFF
        renderer.unsetPixelInScreenBuffer(IR_LETTER_X, IR_LETTER_Y - 1, matrix);
#endif

#ifdef DEBUG_MATRIX
        // debugScreenBuffer(matrixOld);
        debugScreenBuffer(matrix);
#endif

#ifdef DEBUG
        struct tm tmNow = getTime();
        Serial.printf("Time (ESP): %02d:%02d:%02d %s, %s %02d. %02d \n", tmNow.tm_hour, tmNow.tm_min, tmNow.tm_sec, dayOfWeek[tmNow.tm_wday], monthOfYear[tmNow.tm_mon], tmNow.tm_mday, tmNow.tm_year + 1900);
        DEBUG_SERIAL_PRINTLN(F("Total Free Heap: ") + String(ESP.getFreeHeap() / 1024.0f) + F(" kB"));
#endif

        // write screenbuffer to display
        switch (mode)
        {
        case MODE_TIME:
        case MODE_BLANK:
            if (settings.mySettings.transition == TRANSITION_NORMAL)
                writeScreenBuffer(matrix, settings.mySettings.color, brightness);
            if (settings.mySettings.transition == TRANSITION_FADE)
                writeScreenBufferFade(matrixOld, matrix, settings.mySettings.color, brightness);
            if (settings.mySettings.transition == TRANSITION_MOVEUP)
            {
                if (tmNow.tm_min % 5 == 0)
                    moveScreenBufferUp(matrixOld, matrix, settings.mySettings.color, brightness);
                else
                    writeScreenBuffer(matrix, settings.mySettings.color, brightness);
            }
            break;
#ifdef SHOW_MODE_TEST
        case MODE_RED:
            writeScreenBuffer(matrix, RED, brightness);
            break;
        case MODE_GREEN:
            writeScreenBuffer(matrix, GREEN, brightness);
            break;
        case MODE_BLUE:
            writeScreenBuffer(matrix, BLUE, brightness);
            break;
        case MODE_WHITE:
            writeScreenBuffer(matrix, WHITE, brightness);
            break;
#endif
        case MODE_FEED:
            writeScreenBuffer(matrix, feedColor, brightness);
            break;
        default:
            if (runTransitionOnce)
            {
                // if (settings.mySettings.transition == TRANSITION_NORMAL)
                //     writeScreenBuffer(matrix, settings.mySettings.color, brightness);
                // if (settings.mySettings.transition == TRANSITION_FADE)
                //     writeScreenBufferFade(matrixOld, matrix, settings.mySettings.color, brightness);
                // if (settings.mySettings.transition == TRANSITION_MOVEUP)
                //     moveScreenBufferUp(matrixOld, matrix, settings.mySettings.color, brightness);
                moveScreenBufferUp(matrixOld, matrix, settings.mySettings.color, brightness);
                runTransitionOnce = false;
                testColumn = 0;
            }
            else
                writeScreenBuffer(matrix, settings.mySettings.color, brightness);
            break;
        }
    }

    // Wait for mode timeout then switch back to time
    if ((millis() > (modeTimeout + settings.mySettings.timeout * 1000)) && modeTimeout)
    {
        setMode(MODE_TIME);
    }

#ifdef DEBUG_FPS
    // debugFps();
    fps++;
#endif
}

//=============================================================================
// Transitions
//=============================================================================

void writeScreenBuffer(uint16_t screenBuffer[], uint8_t color, uint8_t brightness)
{
    ledDriver.clear();
    for (uint8_t y = 0; y <= 9; y++)
    {
        for (uint8_t x = 0; x <= 10; x++)
        {
            if (bitRead(screenBuffer[y], 15 - x))
            {
                ledDriver.setPixel(x, y, color, brightness);
            }
        }
    }

    // Corner LEDs
    for (uint8_t y = 0; y <= 3; y++)
    {
        if (bitRead(screenBuffer[y], 4))
        {
            ledDriver.setPixel(110 + y, color, brightness);
        }
    }

    // Alarm LED
#ifdef BUZZER
    if (bitRead(screenBuffer[4], 4))
    {
#ifdef ALARM_LED_COLOR
#ifdef ABUSE_CORNER_LED_FOR_ALARM
        if (settings.mySettings.alarm1 || settings.mySettings.alarm2 || alarmTimerSet)
        {
            ledDriver.setPixel(111, ALARM_LED_COLOR, brightness);
        }
        else if (bitRead(screenBuffer[1], 4))
        {
            ledDriver.setPixel(111, color, brightness);
        }
#else
        ledDriver.setPixel(114, ALARM_LED_COLOR, brightness);
#endif
#else
        ledDriver.setPixel(114, color, brightness);
#endif
    }
#endif

    ledDriver.show();
}

void moveScreenBufferUp(uint16_t screenBufferOld[], uint16_t screenBufferNew[], uint8_t color, uint8_t brightness)
{
    for (uint8_t z = 0; z <= 9; z++)
    {
        for (uint8_t i = 0; i <= 8; i++)
        {
            screenBufferOld[i] = screenBufferOld[i + 1];
        }
        screenBufferOld[9] = screenBufferNew[z];
        writeScreenBuffer(screenBufferOld, color, brightness);
        delay(50);
    }
}

void writeScreenBufferFade(uint16_t screenBufferOld[], uint16_t screenBufferNew[], uint8_t color, uint8_t brightness)
{
    ledDriver.clear();
    uint8_t brightnessBuffer[10][12] = {};

    // Copy old matrix to buffer
    for (uint8_t y = 0; y <= 9; y++)
    {
        for (uint8_t x = 0; x <= 11; x++)
        {
            if (bitRead(screenBufferOld[y], 15 - x))
            {
                brightnessBuffer[y][x] = brightness;
            }
        }
    }

    // Fade old to new matrix
    for (uint8_t i = 0; i < brightness; i++)
    {
        for (uint8_t y = 0; y <= 9; y++)
        {
            for (uint8_t x = 0; x <= 11; x++)
            {
                if (!(bitRead(screenBufferOld[y], 15 - x)) && (bitRead(screenBufferNew[y], 15 - x)))
                {
                    brightnessBuffer[y][x]++;
                }
                if ((bitRead(screenBufferOld[y], 15 - x)) && !(bitRead(screenBufferNew[y], 15 - x)))
                {
                    brightnessBuffer[y][x]--;
                }
                ledDriver.setPixel(x, y, color, brightnessBuffer[y][x]);
            }
        }

        // Corner LEDs
        for (uint8_t y = 0; y <= 3; y++)
        {
            ledDriver.setPixel(110 + y, color, brightnessBuffer[y][11]);
        }

        // Alarm LED
#ifdef BUZZER
#ifdef ALARM_LED_COLOR
#ifdef ABUSE_CORNER_LED_FOR_ALARM
        if (settings.mySettings.alarm1 || settings.mySettings.alarm2 || alarmTimerSet)
        {
            ledDriver.setPixel(111, ALARM_LED_COLOR, brightnessBuffer[4][11]);
        }
        else
        {
            ledDriver.setPixel(111, color, brightnessBuffer[1][11]);
        }
#else
        ledDriver.setPixel(114, ALARM_LED_COLOR, brightnessBuffer[4][11]);
#endif
#else
        ledDriver.setPixel(114, color, brightnessBuffer[4][11]);
#endif
#endif
        ledDriver.show();
    }
}

//=============================================================================
// "On/off" pressed
//=============================================================================

#ifdef ONOFF_BUTTON
ICACHE_RAM_ATTR void buttonOnOffInterrupt()
{
    if (millis() > lastButtonPress + 250)
    {
        lastButtonPress = millis();
        buttonOnOffPressed();
    }
}
#endif

void buttonOnOffPressed()
{
    DEBUG_SERIAL_PRINTLN(F("On/off pressed"));
    mode == MODE_BLANK ? setLedsOn() : setLedsOff();
}

//=============================================================================
// "Time" pressed
//=============================================================================

#ifdef TIME_BUTTON
ICACHE_RAM_ATTR void buttonTimeInterrupt()
{
    if (millis() > lastButtonPress + 250)
    {
        lastButtonPress = millis();
        buttonTimePressed();
    }
}
#endif

void buttonTimePressed()
{
    DEBUG_SERIAL_PRINTLN(F("Time pressed"));

    // Switch off alarm
#ifdef BUZZER
    if (alarmOn)
    {
        DEBUG_SERIAL_PRINTLN(F("Alarm: off"));
        digitalWrite(PIN_BUZZER, LOW);
        alarmOn = false;
    }
#endif
    modeTimeout = 0;
    setMode(MODE_TIME);
}

//=============================================================================
// "Mode" pressed
//=============================================================================

#ifdef MODE_BUTTON
ICACHE_RAM_ATTR void buttonModeInterrupt()
{
    if (millis() > lastButtonPress + 250)
    {
        lastButtonPress = millis();
        buttonModePressed();
    }
}
#endif

void buttonModePressed()
{
    DEBUG_SERIAL_PRINTLN(F("Mode pressed"));

    // Switch off alarm
#ifdef BUZZER
    if (alarmOn)
    {
        DEBUG_SERIAL_PRINTLN(F("Alarm: off"));
        digitalWrite(PIN_BUZZER, LOW);
        alarmOn = false;
        setMode(MODE_TIME);
        return;
    }
#endif
    setMode(mode++);
}

//=============================================================================
// Set mode
//=============================================================================

void setMode(Mode newMode)
{
    screenBufferNeedsUpdate = true;
    runTransitionOnce = true;
    lastMode = mode;
    mode = newMode;

    // set timeout for selected mode
    switch (mode)
    {
#ifdef SHOW_MODE_AMPM
    case MODE_AMPM:
#endif
#ifdef SHOW_MODE_SECONDS
    case MODE_SECONDS:
#endif
#ifdef SHOW_MODE_WEEKDAY
    case MODE_WEEKDAY:
#endif
#ifdef SHOW_MODE_DATE
    case MODE_DATE:
#endif
#ifdef SHOW_MODE_MOONPHASE
    case MODE_MOONPHASE:
#endif
#ifdef SENSOR_DHT22
    case MODE_TEMP:
    case MODE_HUMIDITY:
#endif
#ifdef WEATHER
    case MODE_EXT_TEMP:
    case MODE_EXT_HUMIDITY:
#endif
        modeTimeout = millis();
        break;
    default:
        modeTimeout = 0;
        break;
    }
}

//=============================================================================
// Get reading from LDR
//=============================================================================

#ifdef LDR
uint8_t getBrightnessFromLDR()
{
    uint16_t ldrValue = 0;
    static uint16_t lastLdrValue = 0;
    static uint16_t minLdrValue = 511;
    static uint16_t maxLdrValue = 512;

#ifdef LDR_IS_INVERSE
    ldrValue = 1023 - analogRead(PIN_LDR);
#else
    ldrValue = analogRead(PIN_LDR);
#endif

    if (ldrValue < minLdrValue)
    {
        minLdrValue = ldrValue;
    }
    if (ldrValue > maxLdrValue)
    {
        maxLdrValue = ldrValue;
    }
    if ((ldrValue >= (lastLdrValue + 30)) || (ldrValue <= (lastLdrValue - 30))) // Hysteresis
    {
        lastLdrValue = ldrValue;
        return map(ldrValue, minLdrValue, maxLdrValue, MIN_BRIGHTNESS, maxBrightness);
    }
    return iTargetBrightness;
}
#endif

//=============================================================================
// Get room conditions
//=============================================================================

#if defined(SENSOR_DHT22)
void getRoomConditions()
{
    float dhtTemperature = dht.readTemperature();
    float dhtHumidity = dht.readHumidity();
    if (!isnan(dhtTemperature) && !isnan(dhtHumidity))
    {
        errorCounterDHT = 0;
        roomTemperature = dhtTemperature + DHT_TEMPERATURE_OFFSET;
        roomHumidity = dhtHumidity + DHT_HUMIDITY_OFFSET;
        DEBUG_SERIAL_PRINTLN(F("Temperature (DHT): ") + String(roomTemperature) + F(" °C"));
        DEBUG_SERIAL_PRINTLN(F("Humidity (DHT): ") + String(roomHumidity) + F(" %rH"));
    }
    else
    {
        errorCounterDHT++;
    }
}
#endif

//=============================================================================
// Misc
//=============================================================================

// Switch off LEDs
void setLedsOff()
{
    DEBUG_SERIAL_PRINTLN(F("LEDs: off"));
    setMode(MODE_BLANK);
}

// Switch on LEDs
void setLedsOn()
{
    DEBUG_SERIAL_PRINTLN(F("LEDs: on"));
    setMode(lastMode);
}
