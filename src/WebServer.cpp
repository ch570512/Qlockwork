#include "Configuration.h"
#include "WebServer.h"
#include "Helpers.h"
#include "MeteoWeather.h"
#include "Events.h"
#include "Settings.h"
#include "Languages.h"
#include "Debug.h"

//=============================================================================
// Forward declarations of globals defined in main.cpp
//=============================================================================

// Mode
extern Mode mode;
extern Mode lastMode;

// Time
extern const char *dayOfWeek[];
extern const char *monthOfYear[];
extern uint8_t moonphase;
extern time_t upTime;

// Feed
extern String feedText;
extern uint8_t feedColor;
extern uint8_t feedPosition;

// MeteoWeather
#ifdef WEATHER
extern MeteoWeather outdoorWeather;
extern uint8_t errorCounterOutdoorWeather;
#endif

// DHT22
extern float roomTemperature;
extern float roomHumidity;
extern uint8_t errorCounterDHT;

// Brightness and LDR
extern uint8_t maxBrightness;
extern uint8_t brightness;

// Alarm
#ifdef BUZZER
extern bool alarmTimerSet;
extern uint8_t alarmTimer;
extern uint8_t alarmTimerSecond;
extern bool alarmOn;
#endif

// Misc
extern IPAddress myIP;

// Screenbuffer
extern bool screenBufferNeedsUpdate;

// Settings (from main.cpp)
extern Settings settings;

// Functions from main.cpp
void setMode(Mode newMode);
void buttonOnOffPressed();
void buttonModePressed();
void buttonTimePressed();

//=============================================================================
// Constructor
//=============================================================================

WebServerClass::WebServerClass()
    : server(80)
{
}

//=============================================================================
// Setup & Handle
//=============================================================================

void WebServerClass::setup()
{
    server.onNotFound([this]()
                      { handleNotFound(); });
    server.on("/", [this]()
              { handleRoot(); });
    server.on("/handleButtonOnOff", [this]()
              { handleButtonOnOff(); });
    server.on("/handleButtonSettings", [this]()
              { handleButtonSettings(); });
    server.on("/handleButtonMode", [this]()
              { handleButtonMode(); });
    server.on("/handleButtonTime", [this]()
              { handleButtonTime(); });
    server.on("/commitSettings", [this]()
              { handleCommitSettings(); });
    server.on("/reset", [this]()
              { handleReset(); });
    server.on("/setEvent", [this]()
              { handleSetEvent(); });
    server.on("/showText", [this]()
              { handleShowText(); });
    server.on("/control", [this]()
              { handleControl(); });
    server.begin();
}

void WebServerClass::handle()
{
    server.handleClient();
}

//=============================================================================
// Page helpers
//=============================================================================

void WebServerClass::callRoot()
{
    server.send(200, "text/html",
                "<!doctype html><html><head>"
                "<script>window.onload=function(){window.location.replace('/');}</script>"
                "</head></html>");
}

void WebServerClass::handleReset()
{
    server.send(200, "text/plain", "RESET. I'll be back!");
    ESP.restart();
}

//=============================================================================
// Page /root
//=============================================================================

void WebServerClass::handleRoot()
{
    server.send(200, "text/html", generateRootPage());
}

String WebServerClass::generateRootPage()
{
    String message = "<!doctype html>"
                     "<html>"
                     "<head>"
                     "<title>" +
                     String(WEBSITE_TITLE) +
                     "</title>"
                     "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
                     "<meta http-equiv=\"refresh\" content=\"60\" charset=\"UTF-8\">"
                     "<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css\">"
                     "<style>"
                     "body{background-color:#FFFFFF;text-align:center;color:#333333;font-family:Sans-serif;font-size:16px;}"
                     "button{background-color:#1FA3EC;text-align:center;color:#FFFFFF;width:200px;padding:10px;border:5px solid #FFFFFF;font-size:24px;border-radius:10px;}"
                     "</style>"
                     "</head>"
                     "<body>"
                     "<h1>" +
                     String(WEBSITE_TITLE) +
                     "</h1>";

#ifdef DEDICATION
    message += DEDICATION;
    message += "<br><br>";
#endif
    if (mode == MODE_BLANK)
        message += "<button title=\"Switch LEDs on\" onclick=\"window.location.href='/handleButtonOnOff'\"><i class=\"fa fa-toggle-off\"></i></button>";
    else
        message += "<button title=\"Switch LEDs off\" onclick=\"window.location.href='/handleButtonOnOff'\"><i class=\"fa fa-toggle-on\"></i></button>";
    message += "<button title=\"Settings\" onclick=\"window.location.href='/handleButtonSettings'\"><i class=\"fa fa-gear\"></i></button>"
               "<br><br>"
               "<button title=\"Switch modes\" onclick=\"window.location.href='/handleButtonMode'\"><i class=\"fa fa-bars\"></i></button>"
               "<button title=\"Return to time\" onclick=\"window.location.href='/handleButtonTime'\"><i class=\"fa fa-clock-o\"></i></button>";

#ifdef SENSOR_DHT22
    message += "<br><br><i class = \"fa fa-home\" style=\"font-size:20px;\"></i>";
    message += "<br><i class=\"fa fa-thermometer\" style=\"font-size:20px;\"></i> " + String(roomTemperature) + " &deg;C / " + String(roomTemperature * 1.8 + 32.0) + " &deg;F";
    message += "<br><i class=\"fa fa-tint\" style=\"font-size:20px;\"></i> " + String(roomHumidity) + " %rH"
                                                                                                      "<br><span style=\"font-size:20px;\">";
    if (roomHumidity < 20)
        message += "<i style=\"color:Red;\" class=\"fa fa-square\"\"></i>";
    else
        message += "<i style=\"color:Red;\" class=\"fa fa-square-o\"></i>";
    if ((roomHumidity >= 20) && (roomHumidity < 40))
        message += "&nbsp;<i style=\"color:Orange;\" class=\"fa fa-square\"></i>";
    else
        message += "&nbsp;<i style=\"color:Orange;\" class=\"fa fa-square-o\"></i>";
    if ((roomHumidity >= 40) && (roomHumidity <= 60))
        message += "&nbsp;<i style=\"color:MediumSeaGreen;\" class=\"fa fa-square\"></i>";
    else
        message += "&nbsp;<i style=\"color:MediumSeaGreen;\" class=\"fa fa-square-o\"></i>";
    if ((roomHumidity > 60) && (roomHumidity < 80))
        message += "&nbsp;<i style=\"color:Lightblue;\" class=\"fa fa-square\"></i>";
    else
        message += "&nbsp;<i style=\"color:Lightblue;\" class=\"fa fa-square-o\"></i>";
    if (roomHumidity >= 80)
        message += "&nbsp;<i style=\"color:Blue;\" class=\"fa fa-square\"></i>";
    else
        message += "&nbsp;<i style=\"color:Blue;\" class=\"fa fa-square-o\"></i>";
    message += "</span>";
#endif

#ifdef WEATHER
    message += "<br><br><i class = \"fa fa-tree\" style=\"font-size:20px;\"></i>"
               "<br><i class = \"fa fa-thermometer\" style=\"font-size:20px;\"></i> " +
               String(outdoorWeather.temperature) + " &deg;C / " + String(outdoorWeather.temperature * 1.8 + 32.0) + " &deg;F" +
               "<br><i class = \"fa fa-tint\" style=\"font-size:20px;\"></i> " + String(outdoorWeather.humidity) + " %rH" +
               "<br>" + String(outdoorWeather.pressure) + " hPa / " + String(outdoorWeather.pressure / 33.865) + " inHg";

    struct tm *sunriseTime = localtime(&outdoorWeather.sunrise);
    char sunriseBuf[6]; // "HH:MM" + null
    snprintf(sunriseBuf, sizeof(sunriseBuf), "%02d:%02d", sunriseTime->tm_hour, sunriseTime->tm_min);
    message += "<br><i class = \"fa fa-sun-o\" style=\"font-size:20px;\"></i> " + String(sunriseBuf);

    struct tm *sunsetTime = localtime(&outdoorWeather.sunset);
    char sunsetBuf[6];
    snprintf(sunsetBuf, sizeof(sunsetBuf), "%02d:%02d", sunsetTime->tm_hour, sunsetTime->tm_min);
    message += " <i class = \"fa fa-moon-o\" style=\"font-size:20px;\"></i> " + String(sunsetBuf);
#endif

    message += "<span style=\"font-size:12px;\">"
               "<br><br><a href=\"https://github.com/ch570512/Qlockwork\">Qlockwork</a> was <i class=\"fa fa-code\"></i> with <i class=\"fa fa-heart\"></i> by ch570512"
               "<br>Please donate if you find this useful."
               "<br><a href=\"https://www.buymeacoffee.com/ch570512\" target=\"_blank\"><img src=\"https://cdn.buymeacoffee.com/buttons/v2/default-yellow.png\" alt=\"Buy Me A Coffee\" style=\"height: 40px !important;width: 150px !important;\"></a>"
               "<br>Firmware: " +
               String(FIRMWARE_VERSION);

#ifdef DEBUG_WEB
    struct tm tmNow = getTime();
    char timeBuffer[20];
    snprintf(timeBuffer, sizeof(timeBuffer), "<br><br>Time: %02d:%02d",
             tmNow.tm_hour, tmNow.tm_min);
    message += String(timeBuffer);

    if (tmNow.tm_isdst)
        message += " (DST)";

    message += " up " + formatUptime(upTime);

    message += "<br>" + String(dayOfWeek[tmNow.tm_wday]) + ", " + String(monthOfYear[tmNow.tm_mon]) + " " + String(tmNow.tm_mday) + ". " + String(1900 + tmNow.tm_year);
    message += "<br>Moonphase: " + String(moonphase);
    message += "<br>Free Heap: " + String(ESP.getFreeHeap() / 1024.0f) + " kB";
    message += "<br>RSSI: " + String(WiFi.RSSI());
#ifdef LDR
    message += "<br>Brightness: " + String(brightness) + " (ABC: ";
    settings.mySettings.useAbc ? message += "enabled)" : message += "disabled)";
#endif
#ifdef SENSOR_DHT22
    message += "<br>[ERROR] DHT: " + String(errorCounterDHT);
#endif
#ifdef WEATHER
    message += "<br>[ERROR] MeteoWeather: " + String(errorCounterOutdoorWeather);
#endif
    message += "<br>Reset reason: " + ESP.getResetReason();
    message += "<br>Flags: ";
#endif

#ifdef SENSOR_DHT22
    message += "DHT22 ";
#else
    message += "<s>DHT22</s> ";
#endif
#ifdef LDR
    message += "LDR ";
#else
    message += "<s>LDR</s> ";
#endif
#ifdef BUZZER
    message += "BUZZER ";
#else
    message += "<s>BUZZER</s> ";
#endif
#ifdef IR_RECEIVER
    message += "IR_RECEIVER ";
#else
    message += "<s>IR_RECEIVER</s> ";
#endif
#ifdef ESP_LED
    message += "ESP_LED ";
#else
    message += "<s>ESP_LED</s> ";
#endif
#if defined(ONOFF_BUTTON) || defined(MODE_BUTTON) || defined(TIME_BUTTON)
    message += "BUTTONS ";
#else
    message += "<s>BUTTONS</s> ";
#endif
    message += "</span></body></html>";
    return message;
}

void WebServerClass::handleNotFound()
{
    server.send(404, "text/plain", "404 - File Not Found.");
}

//=============================================================================
// Button handlers
//=============================================================================

void WebServerClass::handleButtonOnOff()
{
    buttonOnOffPressed();
    callRoot();
}

void WebServerClass::handleButtonMode()
{
    buttonModePressed();
    callRoot();
}

void WebServerClass::handleButtonTime()
{
    buttonTimePressed();
    callRoot();
}

//=============================================================================
// Page /handleButtonSettings
//=============================================================================

void WebServerClass::handleButtonSettings()
{
    server.send(200, "text/html", generateSettingsPage());
}

String WebServerClass::generateSettingsPage()
{
    String message = "<!doctype html>"
                     "<html>"
                     "<head>"
                     "<title>" +
                     String(WEBSITE_TITLE) + " " TXT_SETTINGS "</title>"
                                             "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
                                             "<meta charset=\"UTF-8\">"
                                             "<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css\">"
                                             "<style>"
                                             "body{background-color:#FFFFFF;text-align:center;color:#333333;font-family:Sans-serif;font-size:16px;}"
                                             "input[type=submit]{background-color:#1FA3EC;text-align:center;color:#FFFFFF;width:200px;padding:12px;border:5px solid #FFFFFF;font-size:20px;border-radius:10px;}"
                                             "table{border-collapse:collapse;margin:0px auto;} td{padding:12px;border-bottom:1px solid #ddd;} tr:first-child{border-top:1px solid #ddd;} td:first-child{text-align:right;} td:last-child{text-align:left;}"
                                             "select{font-size:16px;}"
                                             "button{background-color:#1FA3EC;text-align:center;color:#FFFFFF;width:200px;padding:10px;border:5px solid #FFFFFF;font-size:24px;border-radius:10px;}"
                                             "</style>"
                                             "</head>"
                                             "<body>"
                                             "<h1>" +
                     String(WEBSITE_TITLE) + " " TXT_SETTINGS "</h1>"
                                             "<form action=\"/commitSettings\">"
                                             "<table>";

    // ------------------------------------------------------------------------
#ifdef BUZZER
    message += "<tr><td>" TXT_ALARM
               " 1</td><td>"
               "<input type=\"radio\" name=\"a1\" value=\"1\"";
    if (settings.mySettings.alarm1)
        message += " checked";
    message += "> " TXT_ON
               "<input type=\"radio\" name=\"a1\" value=\"0\"";
    if (!settings.mySettings.alarm1)
        message += " checked";
    message += "> " TXT_OFF "&nbsp;&nbsp;&nbsp;"
               "<input type=\"time\" name=\"a1t\" value=\"";
    if (getHour(settings.mySettings.alarm1Time) < 10)
        message += "0";
    message += String(getHour(settings.mySettings.alarm1Time)) + ":";
    if (getMinute(settings.mySettings.alarm1Time) < 10)
        message += "0";
    message += String(getMinute(settings.mySettings.alarm1Time)) + "\">"
                                                                   " h<br><br>"
                                                                   "<input type=\"checkbox\" name=\"a1w2\" value=\"4\"";
    if (bitRead(settings.mySettings.alarm1Weekdays, 2))
        message += " checked";
    message += "> Mo. "
               "<input type=\"checkbox\" name=\"a1w3\" value=\"8\"";
    if (bitRead(settings.mySettings.alarm1Weekdays, 3))
        message += " checked";
    message += "> Tu. "
               "<input type=\"checkbox\" name=\"a1w4\" value=\"16\"";
    if (bitRead(settings.mySettings.alarm1Weekdays, 4))
        message += " checked";
    message += "> We. "
               "<input type=\"checkbox\" name=\"a1w5\" value=\"32\"";
    if (bitRead(settings.mySettings.alarm1Weekdays, 5))
        message += " checked";
    message += "> Th. "
               "<input type=\"checkbox\" name=\"a1w6\" value=\"64\"";
    if (bitRead(settings.mySettings.alarm1Weekdays, 6))
        message += " checked";
    message += "> Fr. "
               "<input type=\"checkbox\" name=\"a1w7\" value=\"128\"";
    if (bitRead(settings.mySettings.alarm1Weekdays, 7))
        message += " checked";
    message += "> Sa. "
               "<input type=\"checkbox\" name=\"a1w1\" value=\"2\"";
    if (bitRead(settings.mySettings.alarm1Weekdays, 1))
        message += " checked";
    message += "> Su. "
               "</td></tr>";
    // ------------------------------------------------------------------------
    message += "<tr><td>" TXT_ALARM
               " 2</td><td>"
               "<input type=\"radio\" name=\"a2\" value=\"1\"";
    if (settings.mySettings.alarm2)
        message += " checked";
    message += "> " TXT_ON
               "<input type=\"radio\" name=\"a2\" value=\"0\"";
    if (!settings.mySettings.alarm2)
        message += " checked";
    message += "> " TXT_OFF "&nbsp;&nbsp;&nbsp;"
               "<input type=\"time\" name=\"a2t\" value=\"";
    if (getHour(settings.mySettings.alarm2Time) < 10)
        message += "0";
    message += String(getHour(settings.mySettings.alarm2Time)) + ":";
    if (getMinute(settings.mySettings.alarm2Time) < 10)
        message += "0";
    message += String(getMinute(settings.mySettings.alarm2Time)) + "\">"
                                                                   " h<br><br>"
                                                                   "<input type=\"checkbox\" name=\"a2w2\" value=\"4\"";
    if (bitRead(settings.mySettings.alarm2Weekdays, 2))
        message += " checked";
    message += "> Mo. "
               "<input type=\"checkbox\" name=\"a2w3\" value=\"8\"";
    if (bitRead(settings.mySettings.alarm2Weekdays, 3))
        message += " checked";
    message += "> Tu. "
               "<input type=\"checkbox\" name=\"a2w4\" value=\"16\"";
    if (bitRead(settings.mySettings.alarm2Weekdays, 4))
        message += " checked";
    message += "> We. "
               "<input type=\"checkbox\" name=\"a2w5\" value=\"32\"";
    if (bitRead(settings.mySettings.alarm2Weekdays, 5))
        message += " checked";
    message += "> Th. "
               "<input type=\"checkbox\" name=\"a2w6\" value=\"64\"";
    if (bitRead(settings.mySettings.alarm2Weekdays, 6))
        message += " checked";
    message += "> Fr. "
               "<input type=\"checkbox\" name=\"a2w7\" value=\"128\"";
    if (bitRead(settings.mySettings.alarm2Weekdays, 7))
        message += " checked";
    message += "> Sa. "
               "<input type=\"checkbox\" name=\"a2w1\" value=\"2\"";
    if (bitRead(settings.mySettings.alarm2Weekdays, 1))
        message += " checked";
    message += "> Su. "
               "</td></tr>";
    // ------------------------------------------------------------------------
    message += "<tr><td>" TXT_TIMER
               "</td><td>"
               "<select name=\"ti\">";
    for (int i = 0; i <= 10; i++)
    {
        message += "<option value=\"" + String(i) + "\">";
        if (i < 10)
            message += "0";
        message += String(i) + "</option>";
    }
    message += "<option value=\"15\">15</option>"
               "<option value=\"20\">20</option>"
               "<option value=\"25\">25</option>"
               "<option value=\"30\">30</option>"
               "<option value=\"45\">45</option>"
               "<option value=\"60\">60</option>"
               "</select> " TXT_MINUTES
               "</td></tr>";
    // ------------------------------------------------------------------------
    message += "<tr><td>" TXT_HOURBEEP
               "</td><td>"
               "<input type=\"radio\" name=\"hb\" value=\"1\"";
    if (settings.mySettings.hourBeep)
        message += " checked";
    message += "> " TXT_ON
               "<input type=\"radio\" name=\"hb\" value=\"0\"";
    if (!settings.mySettings.hourBeep)
        message += " checked";
    message += "> " TXT_OFF
               "</td></tr>";
#endif
    // ------------------------------------------------------------------------
#ifdef SENSOR_DHT22
    message += "<tr><td>"
               "Show temperature"
               "</td><td>"
               "<input type=\"radio\" name=\"mc\" value=\"1\"";
    if (settings.mySettings.modeChange)
        message += " checked";
    message += "> " TXT_ON
               "<input type=\"radio\" name=\"mc\" value=\"0\"";
    if (!settings.mySettings.modeChange)
        message += " checked";
    message += "> " TXT_OFF
               "</td></tr>";
#endif
    // ------------------------------------------------------------------------
#ifdef LDR
    message += "<tr><td>"
               "ABC"
               "</td><td>"
               "<input type=\"radio\" name=\"ab\" value=\"1\"";
    if (settings.mySettings.useAbc)
        message += " checked";
    message += "> " TXT_ON
               "<input type=\"radio\" name=\"ab\" value=\"0\"";
    if (!settings.mySettings.useAbc)
        message += " checked";
    message += "> " TXT_OFF
               "</td></tr>";
#endif
    // ------------------------------------------------------------------------
    message += "<tr><td>"
               "Brightness"
               "</td><td>"
               "<select name=\"br\">";
    for (int i = 10; i <= 100; i += 10)
    {
        message += "<option value=\"" + String(i) + "\"";
        if (i == settings.mySettings.brightness)
            message += " selected";
        message += ">";
        message += String(i) + "</option>";
    }
    message += "</select> %"
               "</td></tr>";
    // ------------------------------------------------------------------------
    message += "<tr><td>"
               "Color"
               "</td><td>"
               "<select name=\"co\">"
               "<option value=\"0\"";
    if (settings.mySettings.color == 0)
        message += " selected";
    message += ">"
               "White</option>"
               "<option value=\"1\"";
    if (settings.mySettings.color == 1)
        message += " selected";
    message += ">"
               "Red</option>"
               "<option value=\"2\"";
    if (settings.mySettings.color == 2)
        message += " selected";
    message += ">"
               "Red 75%</option>"
               "<option value=\"3\"";
    if (settings.mySettings.color == 3)
        message += " selected";
    message += ">"
               "Red 50%</option>"
               "<option value=\"4\"";
    if (settings.mySettings.color == 4)
        message += " selected";
    message += ">"
               "Orange</option>"
               "<option value=\"5\"";
    if (settings.mySettings.color == 5)
        message += " selected";
    message += ">"
               "Yellow</option>"
               "<option value=\"6\"";
    if (settings.mySettings.color == 6)
        message += " selected";
    message += ">"
               "Yellow 75%</option>"
               "<option value=\"7\"";
    if (settings.mySettings.color == 7)
        message += " selected";
    message += ">"
               "Yellow 50%</option>"
               "<option value=\"8\"";
    if (settings.mySettings.color == 8)
        message += " selected";
    message += ">"
               "Green-Yellow</option>"
               "<option value=\"9\"";
    if (settings.mySettings.color == 9)
        message += " selected";
    message += ">"
               "Green</option>"
               "<option value=\"10\"";
    if (settings.mySettings.color == 10)
        message += " selected";
    message += ">"
               "Green 75%</option>"
               "<option value=\"11\"";
    if (settings.mySettings.color == 11)
        message += " selected";
    message += ">"
               "Green 50%</option>"
               "<option value=\"12\"";
    if (settings.mySettings.color == 12)
        message += " selected";
    message += ">"
               "Mintgreen</option>"
               "<option value=\"13\"";
    if (settings.mySettings.color == 13)
        message += " selected";
    message += ">"
               "Cyan</option>"
               "<option value=\"14\"";
    if (settings.mySettings.color == 14)
        message += " selected";
    message += ">"
               "Cyan 75%</option>"
               "<option value=\"15\"";
    if (settings.mySettings.color == 15)
        message += " selected";
    message += ">"
               "Cyan 50%</option>"
               "<option value=\"16\"";
    if (settings.mySettings.color == 16)
        message += " selected";
    message += ">"
               "Light Blue</option>"
               "<option value=\"17\"";
    if (settings.mySettings.color == 17)
        message += " selected";
    message += ">"
               "Blue</option>"
               "<option value=\"18\"";
    if (settings.mySettings.color == 18)
        message += " selected";
    message += ">"
               "Blue 75%</option>"
               "<option value=\"19\"";
    if (settings.mySettings.color == 19)
        message += " selected";
    message += ">"
               "Blue 50%</option>"
               "<option value=\"20\"";
    if (settings.mySettings.color == 20)
        message += " selected";
    message += ">"
               "Violet</option>"
               "<option value=\"21\"";
    if (settings.mySettings.color == 21)
        message += " selected";
    message += ">"
               "Magenta</option>"
               "<option value=\"22\"";
    if (settings.mySettings.color == 22)
        message += " selected";
    message += ">"
               "Magenta 75%</option>"
               "<option value=\"23\"";
    if (settings.mySettings.color == 23)
        message += " selected";
    message += ">"
               "Magenta 50%</option>"
               "<option value=\"24\"";
    if (settings.mySettings.color == 24)
        message += " selected";
    message += ">"
               "Pink</option>"
               "</select>"
               "</td></tr>";
    // ------------------------------------------------------------------------
    message += "<tr><td>"
               "Colorchange"
               "</td><td>"
               "<input type=\"radio\" name=\"cc\" value=\"3\"";
    if (settings.mySettings.colorChange == 3)
        message += " checked";
    message += "> day "
               "<input type=\"radio\" name=\"cc\" value=\"2\"";
    if (settings.mySettings.colorChange == 2)
        message += " checked";
    message += "> hour "
               "<input type=\"radio\" name=\"cc\" value=\"1\"";
    if (settings.mySettings.colorChange == 1)
        message += " checked";
    message += "> five "
               "<input type=\"radio\" name=\"cc\" value=\"0\"";
    if (settings.mySettings.colorChange == 0)
        message += " checked";
    message += "> off"
               "</td></tr>";
    // ------------------------------------------------------------------------
#ifndef FRONTCOVER_BINARY
    message += "<tr><td>"
               "Transition"
               "</td><td>"
               "<input type=\"radio\" name=\"tr\" value=\"2\"";
    if (settings.mySettings.transition == 2)
        message += " checked";
    message += "> fade "
               "<input type=\"radio\" name=\"tr\" value=\"1\"";
    if (settings.mySettings.transition == 1)
        message += " checked";
    message += "> move "
               "<input type=\"radio\" name=\"tr\" value=\"0\"";
    if (settings.mySettings.transition == 0)
        message += " checked";
    message += "> none"
               "</td></tr>";
#endif
    // ------------------------------------------------------------------------
    message += "<tr><td>"
               "Timeout"
               "</td><td>"
               "<select name=\"to\">";
    for (int i = 0; i <= 60; i += 5)
    {
        message += "<option value=\"" + String(i) + "\"";
        if (i == settings.mySettings.timeout)
            message += " selected";
        message += ">";
        if (i < 10)
            message += "0";
        message += String(i) + "</option>";
    }
    message += "</select> sec."
               "</td></tr>";
    // ------------------------------------------------------------------------
    message += "<tr><td>"
               "Night off"
               "</td><td>"
               "<input type=\"time\" name=\"no\" value=\"";
    if (getHour(settings.mySettings.nightOffTime) < 10)
        message += "0";
    message += String(getHour(settings.mySettings.nightOffTime)) + ":";
    if (getMinute(settings.mySettings.nightOffTime) < 10)
        message += "0";
    message += String(getMinute(settings.mySettings.nightOffTime)) + "\">"
                                                                     " h"
                                                                     "</td></tr>";
    // ------------------------------------------------------------------------
    message += "<tr><td>"
               "Day on"
               "</td><td>"
               "<input type=\"time\" name=\"do\" value=\"";
    if (getHour(settings.mySettings.dayOnTime) < 10)
        message += "0";
    message += String(getHour(settings.mySettings.dayOnTime)) + ":";
    if (getMinute(settings.mySettings.dayOnTime) < 10)
        message += "0";
    message += String(getMinute(settings.mySettings.dayOnTime)) + "\">"
                                                                  " h"
                                                                  "</td></tr>";
    // ------------------------------------------------------------------------
    message += "<tr><td>"
               "Show \"It is\""
               "</td><td>"
               "<input type=\"radio\" name=\"ii\" value=\"1\"";
    if (settings.mySettings.itIs)
        message += " checked";
    message += "> " TXT_ON
               "<input type=\"radio\" name=\"ii\" value=\"0\"";
    if (!settings.mySettings.itIs)
        message += " checked";
    message += "> " TXT_OFF
               "</td></tr>";
    // ------------------------------------------------------------------------
    message += "<tr><td>"
               "Set date/time"
               "</td><td>"
               "<input type=\"datetime-local\" name=\"st\">"
               "</td></tr>";
    // ------------------------------------------------------------------------
    message += "</table>"
               "<br><button title=\"Save Settings.\"><i class=\"fa fa-check\"></i></button>"
               "</form></body></html>";
    return message;
}

//=============================================================================
// Page /commitSettings
//=============================================================================

void WebServerClass::handleCommitSettings()
{
#ifdef DEBUG
    Serial.println(F("Commit settings pressed"));
#endif
    // ------------------------------------------------------------------------
#ifdef BUZZER
    time_t alarmTimeFromWeb = 0;
    server.arg("a1") == "0" ? settings.mySettings.alarm1 = false : settings.mySettings.alarm1 = true;
    alarmTimeFromWeb = server.arg("a1t").substring(0, 2).toInt() * 3600 + server.arg("a1t").substring(3, 5).toInt() * 60;
    if (settings.mySettings.alarm1Time != alarmTimeFromWeb)
        settings.mySettings.alarm1 = true;
    settings.mySettings.alarm1Time = alarmTimeFromWeb;
    settings.mySettings.alarm1Weekdays =
        server.arg("a1w1").toInt() +
        server.arg("a1w2").toInt() +
        server.arg("a1w3").toInt() +
        server.arg("a1w4").toInt() +
        server.arg("a1w5").toInt() +
        server.arg("a1w6").toInt() +
        server.arg("a1w7").toInt();
    // ------------------------------------------------------------------------
    server.arg("a2") == "0" ? settings.mySettings.alarm2 = false : settings.mySettings.alarm2 = true;
    alarmTimeFromWeb = server.arg("a2t").substring(0, 2).toInt() * 3600 + server.arg("a2t").substring(3, 5).toInt() * 60;
    if (settings.mySettings.alarm2Time != alarmTimeFromWeb)
        settings.mySettings.alarm2 = true;
    settings.mySettings.alarm2Time = alarmTimeFromWeb;
    settings.mySettings.alarm2Weekdays =
        server.arg("a2w1").toInt() +
        server.arg("a2w2").toInt() +
        server.arg("a2w3").toInt() +
        server.arg("a2w4").toInt() +
        server.arg("a2w5").toInt() +
        server.arg("a2w6").toInt() +
        server.arg("a2w7").toInt();
    // ------------------------------------------------------------------------
    server.arg("hb") == "0" ? settings.mySettings.hourBeep = false : settings.mySettings.hourBeep = true;
    // ------------------------------------------------------------------------
    if (server.arg("ti").toInt())
    {
        alarmTimer = server.arg("ti").toInt();
        alarmTimerSecond = 0;
        alarmTimerSet = true;
        setMode(MODE_TIMER);
#ifdef DEBUG
        Serial.println(F("Timer started"));
#endif
    }
    else
    {
        if (alarmTimerSet)
        {
            alarmTimer = 0;
            alarmTimerSecond = 0;
            alarmTimerSet = false;
            setMode(MODE_TIME);
#ifdef DEBUG
            Serial.println(F("Timer stopped"));
#endif
        }
    }
#endif
    // ------------------------------------------------------------------------
#ifdef SENSOR_DHT22
    server.arg("mc") == "0" ? settings.mySettings.modeChange = false : settings.mySettings.modeChange = true;
#endif
    // ------------------------------------------------------------------------
#ifdef LDR
    if (server.arg("ab") == "0")
    {
        settings.mySettings.useAbc = false;
        brightness = maxBrightness;
    }
    else
        settings.mySettings.useAbc = true;
#endif
    // ------------------------------------------------------------------------
    settings.mySettings.brightness = server.arg("br").toInt();
    maxBrightness = map(settings.mySettings.brightness, 0, 100, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
    brightness = maxBrightness;
    // ------------------------------------------------------------------------
    settings.mySettings.color = server.arg("co").toInt();
    // ------------------------------------------------------------------------
    switch (server.arg("cc").toInt())
    {
    case 0:
        settings.mySettings.colorChange = COLORCHANGE_NO;
        break;
    case 1:
        settings.mySettings.colorChange = COLORCHANGE_FIVE;
        break;
    case 2:
        settings.mySettings.colorChange = COLORCHANGE_HOUR;
        break;
    case 3:
        settings.mySettings.colorChange = COLORCHANGE_DAY;
        break;
    }
    // ------------------------------------------------------------------------
    switch (server.arg("tr").toInt())
    {
    case 0:
        settings.mySettings.transition = TRANSITION_NORMAL;
        break;
    case 1:
        settings.mySettings.transition = TRANSITION_MOVEUP;
        break;
    case 2:
        settings.mySettings.transition = TRANSITION_FADE;
        break;
    }
    // ------------------------------------------------------------------------
    settings.mySettings.timeout = server.arg("to").toInt();
    // ------------------------------------------------------------------------
    settings.mySettings.nightOffTime = server.arg("no").substring(0, 2).toInt() * 3600 + server.arg("no").substring(3, 5).toInt() * 60;
    // ------------------------------------------------------------------------
    settings.mySettings.dayOnTime = server.arg("do").substring(0, 2).toInt() * 3600 + server.arg("do").substring(3, 5).toInt() * 60;
    // ------------------------------------------------------------------------
    server.arg("ii") == "0" ? settings.mySettings.itIs = false : settings.mySettings.itIs = true;
    // ------------------------------------------------------------------------
    if (server.arg("st").length())
    {
        handleTimeSetting(server.arg("st"));
    }
    // ------------------------------------------------------------------------
    settings.saveToEEPROM();
    callRoot();
    screenBufferNeedsUpdate = true;
}

//=============================================================================
// Page /setEvent
//=============================================================================

void WebServerClass::handleSetEvent()
{
    events[0].day = server.arg("day").toInt();
    events[0].month = server.arg("month").toInt();
    events[0].text = server.arg("text").substring(0, 40);
    events[0].color = (eColor)server.arg("color").toInt();
    server.send(200, "text/plain", "OK.");
    DEBUG_SERIAL_PRINTLN(F("Event set: ") + String(events[0].day) + "." + String(events[0].month) + F(". ") + events[0].text);
}

//=============================================================================
// Page /showText
//=============================================================================

void WebServerClass::handleShowText()
{
#ifdef BUZZER
    uint8_t feedBuzzer = server.arg("buzzer").toInt();
#endif
    feedColor = server.arg("color").toInt();
    feedText = "  " + server.arg("text").substring(0, 80) + "   ";
    feedPosition = 0;
    server.send(200, "text/plain", "OK.");
    DEBUG_SERIAL_PRINTLN(F("Show text: ") + server.arg("text").substring(0, 80));

#ifdef BUZZER
    for (uint8_t i = 0; i < feedBuzzer; i++)
    {
        digitalWrite(PIN_BUZZER, HIGH);
        delay(75);
        digitalWrite(PIN_BUZZER, LOW);
        delay(100);
    }
#endif

    setMode(MODE_FEED);
}

//=============================================================================
// Page /control
//=============================================================================

void WebServerClass::handleControl()
{
    setMode((Mode)server.arg("mode").toInt());
    server.send(200, "text/plain", "OK.");
}
