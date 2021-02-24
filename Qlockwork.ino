// ******************************************************************************
// QLOCKWORK
// An advanced firmware for a DIY "word-clock"
//
// @mc ESP8266
// @created 12.03.2017
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
//
// ******************************************************************************

#define FIRMWARE_VERSION 20210224

#include <Arduino.h>
#include <Arduino_JSON.h>
#include <ArduinoHttpClient.h>
#include <ArduinoOTA.h>
#include <DHT.h>
#include <DS3232RTC.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>
#include <TimeLib.h>

#include "Colors.h"
#include "Configuration.h"
#include "Events.h"
#include "Languages.h"
#include "LedDriver.h"
#include "Modes.h"
#include "Ntp.h"
#include "OpenWeather.h"
#include "Renderer.h"
#include "Settings.h"
#include "Syslog.h"
#include "Timezone.h"
#include "Timezones.h"
#include "WiFiManager.h"

//******************************************************************************
// Init
//******************************************************************************

// Servers
ESP8266WebServer webServer(80);
ESP8266HTTPUpdateServer httpUpdater;

// DHT22
#ifdef SENSOR_DHT22
DHT dht(PIN_DHT22, DHT22);
#endif

// IR receiver
#ifdef IR_RECEIVER
IRrecv irrecv(PIN_IR_RECEIVER);
decode_results irDecodeResult;
#endif

// Syslog
#ifdef SYSLOGSERVER_SERVER
WiFiUDP wifiUdp;
Syslog syslog(wifiUdp, SYSLOGSERVER_SERVER, SYSLOGSERVER_PORT, HOSTNAME, "QLOCKWORK", LOG_INFO);
#endif

// RTC
#ifdef RTC_BACKUP
DS3232RTC RTC(true);
#endif

// LED driver 
LedDriver ledDriver;

// Renderer
Renderer renderer;

// Settings
Settings settings;

// NTP
Ntp ntp;
char ntpServer[] = NTP_SERVER;
uint8_t errorCounterNTP = 0;

// Screenbuffer
uint16_t matrix[10] = {};
uint16_t matrixOld[10] = {};
boolean screenBufferNeedsUpdate = true;

// Mode
Mode mode = MODE_TIME;
Mode lastMode = mode;
uint32_t modeTimeout = 0;
uint32_t autoModeChangeTimer = AUTO_MODECHANGE_TIME;
boolean runTransitionOnce = false;
uint8_t autoMode = 0;

// Time
uint8_t lastDay = 0;
uint8_t lastMinute = 0;
uint8_t lastHour = 0;
uint8_t lastSecond = 0;
uint32_t last500Millis = 0;
uint32_t last50Millis = 0;
time_t upTime = 0;
uint8_t randomHour = 0;
uint8_t randomMinute = 0;
uint8_t randomSecond = 0;
uint8_t moonphase = 0;

// Feed
String feedText = "";
uint8_t feedColor = WHITE;
uint8_t feedPosition = 0;

// OpenWeather
#ifdef APIKEY
OpenWeather outdoorWeather;
uint8_t errorCounterOutdoorWeather = 0;
#endif

// DHT22
float roomTemperature = 0;
float roomHumidity = 0;
uint8_t errorCounterDHT = 0;

// Brightness and LDR
uint8_t maxBrightness = map(settings.mySettings.brightness, 0, 100, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
uint8_t brightness = maxBrightness;
uint16_t ldrValue = 0;
#ifdef LDR
uint16_t lastLdrValue = 0;
uint16_t minLdrValue = 511; // The ESP will crash if minLdrValue and maxLdrValue are equal due to an error in map()
uint16_t maxLdrValue = 512;
#endif

// Alarm
#ifdef BUZZER
boolean alarmTimerSet = false;
uint8_t alarmTimer = 0;
uint8_t alarmTimerSecond = 0;
uint8_t alarmOn = false;
#endif

// Events
#ifdef EVENT_TIME
uint32_t showEventTimer = EVENT_TIME;
#endif

// Misc
uint8_t testColumn = 0;
int updateInfo = 0;
IPAddress myIP = { 0,0,0,0 };
uint32_t lastButtonPress = 0;

//******************************************************************************
// Setup()
//******************************************************************************

void setup()
{
	// init serial port
	Serial.begin(SERIAL_SPEED);
	while (!Serial);
	delay(1000);

	// And the monkey flips the switch. (Akiva Goldsman)
	Serial.println();
	Serial.println("*** QLOCKWORK ***");
	Serial.println("Firmware: " + String(FIRMWARE_VERSION));

#ifdef POWERON_SELFTEST
	renderer.setAllScreenBuffer(matrix);
	Serial.println("Set all LEDs to red.");
	writeScreenBuffer(matrix, RED, TEST_BRIGHTNESS);
	delay(2500);
	Serial.println("Set all LEDs to green.");
	writeScreenBuffer(matrix, GREEN, TEST_BRIGHTNESS);
	delay(2500);
	Serial.println("Set all LEDs to blue.");
	writeScreenBuffer(matrix, BLUE, TEST_BRIGHTNESS);
	delay(2500);
	Serial.println("Set all LEDs to white.");
	writeScreenBuffer(matrix, WHITE, TEST_BRIGHTNESS);
	delay(2500);
#endif

#ifdef ESP_LED
	Serial.println("Setting up ESP-LED.");
	pinMode(PIN_LED, OUTPUT);
	digitalWrite(PIN_LED, HIGH);
#endif

#ifdef MODE_BUTTON
	Serial.println("Setting up Mode-Button.");
	pinMode(PIN_MODE_BUTTON, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(PIN_MODE_BUTTON), buttonModeInterrupt, FALLING);
#endif

#ifdef ONOFF_BUTTON
	Serial.println("Setting up Back-Button.");
	pinMode(PIN_ONOFF_BUTTON, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(PIN_ONOFF_BUTTON), buttonOnOffInterrupt, FALLING);
#endif

#ifdef TIME_BUTTON
	Serial.println("Setting up Time-Button.");
	pinMode(PIN_TIME_BUTTON, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(PIN_TIME_BUTTON), buttonTimeInterrupt, FALLING);
#endif

#ifdef BUZZER
	Serial.println("Setting up Buzzer.");
	pinMode(PIN_BUZZER, OUTPUT);
#endif

#ifdef SENSOR_DHT22
	Serial.println("Setting up DHT22.");
	dht.begin();
#endif

#ifdef LDR
	Serial.print("Setting up LDR. ABC: ");
	settings.mySettings.useAbc ? Serial.println("enabled") : Serial.println("disabled");
	pinMode(PIN_LDR, INPUT);
#endif

#ifdef IR_RECEIVER
	Serial.println("Setting up IR-Receiver.");
	irrecv.enableIRIn();
#endif

	// Start WiFi and services
	renderer.clearScreenBuffer(matrix);
	renderer.setSmallText("WI", TEXT_POS_TOP, matrix);
	renderer.setSmallText("FI", TEXT_POS_BOTTOM, matrix);
	writeScreenBuffer(matrix, WHITE, brightness);
	WiFiManager wifiManager;
	//wifiManager.resetSettings();
	wifiManager.setTimeout(WIFI_SETUP_TIMEOUT);
	wifiManager.autoConnect(HOSTNAME, WIFI_AP_PASS);
	WiFi.hostname(HOSTNAME);
	WiFi.setAutoReconnect(true);
	if (!WiFi.isConnected())
	{
		WiFi.mode(WIFI_AP);
		Serial.println("No WLAN connected. Staying in AP mode.");
		writeScreenBuffer(matrix, RED, brightness);
#if defined(BUZZER) && defined(WIFI_BEEPS)
		digitalWrite(PIN_BUZZER, HIGH);
		delay(1500);
		digitalWrite(PIN_BUZZER, LOW);
#endif
		delay(1000);
		myIP = WiFi.softAPIP();
	}
	else
	{
		WiFi.mode(WIFI_STA);
		Serial.println("WLAN connected. Switching to STA mode.");
		Serial.println("RSSI: " + String(WiFi.RSSI()));
		writeScreenBuffer(matrix, GREEN, brightness);
#if defined(BUZZER) && defined(WIFI_BEEPS)
		for (uint8_t i = 0; i <= 2; i++)
		{
#ifdef DEBUG
			Serial.println("Beep!");
#endif
			digitalWrite(PIN_BUZZER, HIGH);
			delay(100);
			digitalWrite(PIN_BUZZER, LOW);
			delay(100);
		}
#endif
		delay(1000);
		myIP = WiFi.localIP();

		// mDNS is needed to see HOSTNAME in Arduino IDE
		Serial.println("Starting mDNS responder.");
		MDNS.begin(HOSTNAME);
		//MDNS.addService("http", "tcp", 80);

		Serial.println("Starting OTA service.");
#ifdef DEBUG
		ArduinoOTA.onStart([]()
			{
				Serial.println("Start OTA update.");
			});
		ArduinoOTA.onError([](ota_error_t error)
			{
				Serial.println("OTA Error: " + String(error));
				if (error == OTA_AUTH_ERROR) Serial.println("Auth failed.");
				else if (error == OTA_BEGIN_ERROR) Serial.println("Begin failed.");
				else if (error == OTA_CONNECT_ERROR) Serial.println("Connect failed.");
				else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive failed.");
				else if (error == OTA_END_ERROR) Serial.println("End failed.");
			});
		ArduinoOTA.onEnd([]()
			{
				Serial.println("End OTA update.");
			});
#endif
		ArduinoOTA.setPassword(OTA_PASS);
		ArduinoOTA.begin();

#ifdef SYSLOGSERVER_SERVER
		Serial.println("Starting syslog.");
#ifdef APIKEY
		syslog.log(LOG_INFO, ";#;dateTime;roomTemperature;roomHumidity;outdoorTemperature;outdoorHumidity;ldrValue;errorCounterNTP;errorCounterDHT;errorCounterOutdoorWeather;freeHeapSize;upTime");
#else
		syslog.log(LOG_INFO, ";#;dateTime;roomTemperature;roomHumidity;ldrValue;errorCounterNTP;errorCounterDHT;freeHeapSize;upTime");
#endif
#endif

		// Get weather from OpenWeather
#ifdef APIKEY
#ifdef DEBUG
		Serial.println("Getting outdoor weather:");
#endif
		!outdoorWeather.getOutdoorConditions(LOCATION, APIKEY, LANGSTR) ? errorCounterOutdoorWeather++ : errorCounterOutdoorWeather = 0;
#ifdef DEBUG
		Serial.println(outdoorWeather.description);
		Serial.println(outdoorWeather.temperature);
		Serial.println(outdoorWeather.humidity);
		Serial.println(outdoorWeather.pressure);
#endif
#endif
	}

#ifdef SHOW_IP
	WiFi.isConnected() ? feedText = "  IP: " : feedText = "  AP-IP: ";
	feedText += String(myIP[0]) + '.' + String(myIP[1]) + '.' + String(myIP[2]) + '.' + String(myIP[3]) + "   ";
	feedPosition = 0;
	feedColor = WHITE;
	mode = MODE_FEED;
#endif

	Serial.println("Starting webserver.");
	setupWebServer();

	Serial.println("Starting updateserver.");
	httpUpdater.setup(&webServer);

	renderer.clearScreenBuffer(matrix);

#ifdef RTC_BACKUP
	setSyncProvider(RTC.get);
	Serial.print("RTC Sync.");
	if (timeStatus() != timeSet) Serial.print(" FAIL!");
	Serial.println();
#ifdef DEBUG
	time_t tempRtcTime = RTC.get();
	Serial.printf("Time (RTC): %02u:%02u:%02u %02u.%02u.%04u\r\n", hour(tempRtcTime), minute(tempRtcTime), second(tempRtcTime), day(tempRtcTime), month(tempRtcTime), year(tempRtcTime));
#endif
#endif

	// Get the time!
	if (WiFi.isConnected())
	{
		// Set ESP (and RTC) time from NTP
		time_t tempNtpTime = ntp.getTime(ntpServer);
		if (tempNtpTime)
		{
			errorCounterNTP = 0;
			setTime(timeZone.toLocal(tempNtpTime));
#ifdef DEBUG
			Serial.printf("Time (NTP): %02u:%02u:%02u %02u.%02u.%04u (UTC)\r\n", hour(tempNtpTime), minute(tempNtpTime), second(tempNtpTime), day(tempNtpTime), month(tempNtpTime), year(tempNtpTime));
			Serial.printf("Drift (ESP): %d sec.\r\n", tempNtpTime - timeZone.toUTC(now()));
#endif
#ifdef RTC_BACKUP
			RTC.set(timeZone.toLocal(tempNtpTime));
#ifdef DEBUG
			Serial.printf("Drift (RTC): %d sec.\r\n", tempNtpTime - timeZone.toUTC(RTC.get()));
#endif
#endif
		}
		else
		{
			if (errorCounterNTP < 255)
				errorCounterNTP++;
#ifdef DEBUG
			Serial.printf("Error (NTP): %u\r\n", errorCounterNTP);
#endif
		}
	}

	// Define a random time
	randomSeed(analogRead(A0));
	randomHour = random(0, 24);
	randomMinute = random(5, 56);
	randomSecond = random(5, 56);

	// Update room conditions
#if defined(RTC_BACKUP) || defined(SENSOR_DHT22)
	getRoomConditions();
#endif

	// print some infos
#ifdef DEBUG
	Serial.printf("Defined events: %u\r\n", sizeof(events) / sizeof(event_t));
	Serial.printf("Day on: %02u:%02u:00\r\n", hour(settings.mySettings.dayOnTime), minute(settings.mySettings.dayOnTime));
	Serial.printf("Night off: %02u:%02u:00\r\n", hour(settings.mySettings.nightOffTime), minute(settings.mySettings.nightOffTime));
	Serial.printf("Alarm1: %02u:%02u:00 ", hour(settings.mySettings.alarm1Time), minute(settings.mySettings.alarm1Time));
	settings.mySettings.alarm1 ? Serial.print("on ") : Serial.print("off ");
	Serial.println(settings.mySettings.alarm1Weekdays, BIN);
	Serial.printf("Alarm2: %02u:%02u:00 ", hour(settings.mySettings.alarm2Time), minute(settings.mySettings.alarm2Time));
	settings.mySettings.alarm2 ? Serial.print("on ") : Serial.print("off ");
	Serial.println(settings.mySettings.alarm2Weekdays, BIN);
	Serial.printf("Random time: %02u:%02u:%02u\r\n", randomHour, randomMinute, randomSecond);
	Serial.println("DEBUG is on.");
#else
	Serial.println("DEBUG is off.");
#endif

	lastDay = day() - 1;
	lastHour = hour();
	lastMinute = minute();
	lastSecond = second();

#ifdef FRONTCOVER_BINARY
	settings.setTransition(TRANSITION_NORMAL);
#endif
} // setup()

// ******************************************************************************
// Loop()
// ******************************************************************************

void loop()
{
	// ************************************************************************
	// Run once a day
	// ************************************************************************

	if (day() != lastDay)
	{
		lastDay = day();
		screenBufferNeedsUpdate = true;

#ifdef SHOW_MODE_MOONPHASE
		moonphase = getMoonphase(year(), month(), day());
#endif

		// Reset URL event 0
		events[0].day = 0;
		events[0].month = 0;

#ifdef DEBUG
		Serial.printf("Uptime: %u days, %02u:%02u\r\n", int(upTime / 86400), hour(upTime), minute(upTime));
		Serial.printf("Free RAM: %u bytes\r\n", system_get_free_heap_size());
		Serial.printf("Moonphase: %u\r\n", moonphase);
#endif

		// Change color
		if (settings.mySettings.colorChange == COLORCHANGE_DAY)
		{
			settings.mySettings.color = random(0, COLORCHANGE_COUNT + 1);
#ifdef DEBUG
			Serial.printf("Color changed to: %u\r\n", settings.mySettings.color);
#endif
		}
	}

	// ************************************************************************
	// Run once every hour
	// ************************************************************************

	if (hour() != lastHour)
	{
		lastHour = hour();
		screenBufferNeedsUpdate = true;

		// Change color
		if (settings.mySettings.colorChange == COLORCHANGE_HOUR)
		{
			settings.mySettings.color = random(0, COLOR_COUNT + 1);
#ifdef DEBUG
			Serial.printf("Color changed to: %u\r\n", settings.mySettings.color);
#endif
		}

		// Hourly beep
#ifdef BUZZER
		if ((settings.mySettings.hourBeep == true) && (mode == MODE_TIME))
		{
			digitalWrite(PIN_BUZZER, HIGH);
			delay(25);
			digitalWrite(PIN_BUZZER, LOW);
#ifdef DEBUG
			Serial.println("Beep!");
#endif
		}
#endif

		// ************************************************************************
		// Run once every random hour (once a day)
		// ************************************************************************

		if (hour() == randomHour)
		{
			// Get updateinfo
#ifdef UPDATE_INFOSERVER
			if (WiFi.isConnected())
				getUpdateInfo();
#endif
		}

	}

	// ************************************************************************
	// Run once every minute
	// ************************************************************************

	if (minute() != lastMinute)
	{
		lastMinute = minute();
		screenBufferNeedsUpdate = true;

#if defined(RTC_BACKUP) || defined(SENSOR_DHT22)
		// Update room conditions
		getRoomConditions();
#endif

#ifdef BUZZER
		// Switch on buzzer for alarm 1
		if (settings.mySettings.alarm1 && (hour() == hour(settings.mySettings.alarm1Time)) && (minute() == minute(settings.mySettings.alarm1Time)) && bitRead(settings.mySettings.alarm1Weekdays, weekday()))
		{
			alarmOn = BUZZTIME_ALARM_1;
#ifdef DEBUG
			Serial.println("Alarm1 on.");
#endif
		}

		// Switch on buzzer for alarm 2
		if (settings.mySettings.alarm2 && (hour() == hour(settings.mySettings.alarm2Time)) && (minute() == minute(settings.mySettings.alarm2Time)) && bitRead(settings.mySettings.alarm2Weekdays, weekday()))
		{
			alarmOn = BUZZTIME_ALARM_2;
#ifdef DEBUG
			Serial.println("Alarm2 on.");
#endif
		}
#endif

#ifdef DEBUG
		time_t tempEspTime = now();
		Serial.printf("Time (ESP): %02u:%02u:%02u %02u.%02u.%04u\r\n", hour(tempEspTime), minute(tempEspTime), second(tempEspTime), day(tempEspTime), month(tempEspTime), year(tempEspTime));
#ifdef RTC_BACKUP
		time_t tempRtcTime = RTC.get();
		Serial.printf("Time (RTC): %02u:%02u:%02u %02u.%02u.%04u\r\n", hour(tempRtcTime), minute(tempRtcTime), second(tempRtcTime), day(tempRtcTime), month(tempRtcTime), year(tempRtcTime));
#endif
#endif

		// Set night- and daymode
		if ((hour() == hour(settings.mySettings.nightOffTime)) && (minute() == minute(settings.mySettings.nightOffTime)))
		{
#ifdef DEBUG
			Serial.println("Night off.");
#endif
			setMode(MODE_BLANK);
		}
		if ((hour() == hour(settings.mySettings.dayOnTime)) && (minute() == minute(settings.mySettings.dayOnTime)))
		{
#ifdef DEBUG
			Serial.println("Day on.");
#endif
			setMode(lastMode);
		}

		// ************************************************************************
		// Run once every random minute (once an hour) or if NTP has an error
		// ************************************************************************

		if ((minute() == randomMinute) || ((errorCounterNTP > 0) && (errorCounterNTP < 10)))
		{
			if (WiFi.isConnected())
			{
				// Set ESP (and RTC) time from NTP
				time_t tempNtpTime = ntp.getTime(ntpServer);
				if (tempNtpTime)
				{
					errorCounterNTP = 0;
					setTime(timeZone.toLocal(tempNtpTime));
#ifdef DEBUG
					Serial.printf("Time (NTP): %02u:%02u:%02u %02u.%02u.%04u (UTC)\r\n", hour(tempNtpTime), minute(tempNtpTime), second(tempNtpTime), day(tempNtpTime), month(tempNtpTime), year(tempNtpTime));
					Serial.printf("Drift (ESP): %d sec.\r\n", tempNtpTime - timeZone.toUTC(now()));
#endif
#ifdef RTC_BACKUP
					RTC.set(timeZone.toLocal(tempNtpTime));
#ifdef DEBUG
					Serial.printf("Drift (RTC): %d sec.\r\n", tempNtpTime - timeZone.toUTC(RTC.get()));
#endif
#endif
				}
				else
				{
					if (errorCounterNTP < 255)
						errorCounterNTP++;
#ifdef DEBUG
					Serial.printf("Error (NTP): %u\r\n", errorCounterNTP);
#endif
				}
			}
		}

		if (minute() == randomMinute)
		{
			if (WiFi.isConnected())
			{
				// Get weather from OpenWeather
#ifdef APIKEY
				!outdoorWeather.getOutdoorConditions(LOCATION, APIKEY, LANGSTR) ? errorCounterOutdoorWeather++ : errorCounterOutdoorWeather = 0;
#ifdef DEBUG
				Serial.println(outdoorWeather.description);
				Serial.println(outdoorWeather.temperature);
				Serial.println(outdoorWeather.humidity);
				Serial.println(outdoorWeather.pressure);
#endif
#endif
			}
		}

		// ************************************************************************
		// Run once every 5 minutes
		// ************************************************************************

		if (minute() % 5 == 0)
		{
#ifdef SYSLOGSERVER_SERVER
			// Write some data to syslog
			if (WiFi.isConnected())
			{
				time_t tempEspTime = now();
#ifdef APIKEY
				syslog.log(LOG_INFO, ";D;" + String(tempEspTime) + ";" + String(roomTemperature) + ";" + String(roomHumidity) + ";" + String(outdoorWeather.temperature) + ";" + String(outdoorWeather.humidity) + ";" + String(ldrValue)\
					+ ";" + String(errorCounterNTP) + ";" + String(errorCounterDHT) + ";" + String(errorCounterOutdoorWeather) + ";" + String(ESP.getFreeHeap()) + ";" + String(upTime));
#else
				syslog.log(LOG_INFO, ";D;" + String(tempEspTime) + ";" + String(roomTemperature) + ";" + String(roomHumidity) + ";" + String(ldrValue)\
					+ ";" + String(errorCounterNTP) + ";" + String(errorCounterDHT) + ";" + String(ESP.getFreeHeap()) + ";" + String(upTime));
#endif
#ifdef DEBUG
				Serial.println("Data written to syslog.");
#endif
			}
#endif
			// Change color
			if (settings.mySettings.colorChange == COLORCHANGE_FIVE)
			{
				settings.mySettings.color = random(0, COLOR_COUNT + 1);
#ifdef DEBUG
				Serial.printf("Color changed to: %u\r\n", settings.mySettings.color);
#endif
			}
		}
	}

	// ************************************************************************
	// Run once every second
	// ************************************************************************

	if (second() != lastSecond)
	{
		lastSecond = second();

		upTime++;

#ifdef BUZZER
		// Make some noise
		if (alarmOn)
		{
			alarmOn--;
			digitalRead(PIN_BUZZER) ? digitalWrite(PIN_BUZZER, LOW) : digitalWrite(PIN_BUZZER, HIGH);
			if (!alarmOn)
			{
#ifdef DEBUG
				Serial.println("Alarm: off");
#endif
				digitalWrite(PIN_BUZZER, LOW);
				screenBufferNeedsUpdate = true;
			}
		}
#endif

		// Set brightness from LDR
#ifdef LDR
		if (settings.mySettings.useAbc)
			setBrightnessFromLdr();
#endif

#ifdef FRONTCOVER_BINARY
		if (mode != MODE_BLANK)
			screenBufferNeedsUpdate = true;
#else
		// Running displayupdate in MODE_TIME or MODE_BLANK every second will lock the ESP due to TRANSITION_FADE
		if ((mode != MODE_TIME) && (mode != MODE_BLANK))
			screenBufferNeedsUpdate = true;
#endif

		// Flash ESP LED
#ifdef ESP_LED
		digitalWrite(PIN_LED, !digitalRead(PIN_LED));
#endif

		// Countdown timeralarm by one minute in the second it was activated
#ifdef BUZZER
		if (alarmTimer && alarmTimerSet && (alarmTimerSecond == second()))
		{
			alarmTimer--;
#ifdef DEBUG
			if (alarmTimer) Serial.printf("Timeralarm in %u min.\r\n", alarmTimer);
#endif
		}
		// Switch on buzzer for timer
		if (!alarmTimer && alarmTimerSet)
		{
			alarmTimerSet = false;
			alarmOn = BUZZTIME_TIMER;
#ifdef DEBUG
			Serial.println("Timeralarm: on");
#endif
		}
#endif

		// Auto switch modes
		if (settings.mySettings.modeChange && (mode == MODE_TIME))
		{
			autoModeChangeTimer--;
			if (!autoModeChangeTimer)
			{
#ifdef DEBUG
				Serial.println("Auto modechange. (" + String(autoMode) + ")");
#endif
				autoModeChangeTimer = AUTO_MODECHANGE_TIME;
				switch (autoMode)
				{
				case 0:
#ifdef APIKEY
					if (WiFi.isConnected())
						setMode(MODE_EXT_TEMP);
#endif
					autoMode = 1;
					break;
				case 1:
#if defined(RTC_BACKUP) || defined(SENSOR_DHT22)
					setMode(MODE_TEMP);
#else
#ifdef APIKEY
					if (WiFi.isConnected())
						setMode(MODE_EXT_TEMP);
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
					if ((day() == events[i].day) && (month() == events[i].month))
					{
						if (events[i].year)
							feedText = "  " + events[i].text + " (" + String(year() - events[i].year) + ")   ";
						else
							feedText = "  " + events[i].text + "   ";
						feedPosition = 0;
						feedColor = events[i].color;
#ifdef DEBUG
						Serial.println("Event: \"" + feedText + "\"");
#endif
						setMode(MODE_FEED);
					}
				}
			}
		}
#endif
	}

	// ************************************************************************
	// Run always
	// ************************************************************************

	if (mode == MODE_FEED)
		screenBufferNeedsUpdate = true;

	// Call HTTP- and OTA-handle
	webServer.handleClient();
	ArduinoOTA.handle();

#ifdef IR_RECEIVER
	// Look for IR commands
	if (irrecv.decode(&irDecodeResult))
	{
#ifdef DEBUG_IR
		Serial.print("IR signal: 0x");
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

		// Save old screenbuffer
		for (uint8_t i = 0; i <= 9; i++) matrixOld[i] = matrix[i];

		switch (mode)
		{
		case MODE_TIME:
			renderer.clearScreenBuffer(matrix);

#ifdef FRONTCOVER_BINARY
			matrix[0] = 0b1111000000000000;
			matrix[1] = hour() << 5;
			matrix[2] = minute() << 5;
			matrix[3] = second() << 5;
			matrix[5] = 0b1111000000000000;
			matrix[6] = day() << 5;
			matrix[7] = month() << 5;
			matrix[8] = year() - 2000 << 5;
#else
			renderer.setTime(hour(), minute(), matrix);
			renderer.setCorners(minute(), matrix);
			if (!settings.mySettings.itIs && ((minute() / 5) % 6)) renderer.clearEntryWords(matrix);
#endif
#ifdef BUZZER
			if (settings.mySettings.alarm1 || settings.mySettings.alarm2 || alarmTimerSet) renderer.setAlarmLed(matrix);
#endif
			break;
#ifdef SHOW_MODE_AMPM
		case MODE_AMPM:
			renderer.clearScreenBuffer(matrix);
			isAM() ? renderer.setSmallText("AM", TEXT_POS_MIDDLE, matrix) : renderer.setSmallText("PM", TEXT_POS_MIDDLE, matrix);
			break;
#endif
#ifdef SHOW_MODE_SECONDS
		case MODE_SECONDS:
			renderer.clearScreenBuffer(matrix);
			renderer.setCorners(minute(), matrix);
			for (uint8_t i = 0; i <= 6; i++)
			{
				matrix[1 + i] |= numbersBig[second() / 10][i] << 11;
				matrix[1 + i] |= numbersBig[second() % 10][i] << 5;
			}
			break;
#endif
#ifdef SHOW_MODE_WEEKDAY
		case MODE_WEEKDAY:
			renderer.clearScreenBuffer(matrix);
			renderer.setSmallText(String(sWeekday[weekday()][0]) + String(sWeekday[weekday()][1]), TEXT_POS_MIDDLE, matrix);
			break;
#endif
#ifdef SHOW_MODE_DATE
		case MODE_DATE:
			renderer.clearScreenBuffer(matrix);
			if (day() < 10)
				renderer.setSmallText(("0" + String(day())), TEXT_POS_TOP, matrix);
			else
				renderer.setSmallText(String(day()), TEXT_POS_TOP, matrix);
			if (month() < 10)
				renderer.setSmallText(("0" + String(month())), TEXT_POS_BOTTOM, matrix);
			else
				renderer.setSmallText(String(month()), TEXT_POS_BOTTOM, matrix);
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
#if defined(RTC_BACKUP) || defined(SENSOR_DHT22)
		case MODE_TEMP:
#ifdef DEBUG
			Serial.println(String(roomTemperature));
#endif
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
#ifdef SENSOR_DHT22
		case MODE_HUMIDITY:
#ifdef DEBUG
			Serial.println(String(roomHumidity));
#endif
			renderer.clearScreenBuffer(matrix);
			renderer.setSmallText(String(int(roomHumidity + 0.5)), TEXT_POS_TOP, matrix);
			matrix[6] = 0b0100100001000000;
			matrix[7] = 0b0001000010100000;
			matrix[8] = 0b0010000010100000;
			matrix[9] = 0b0100100011100000;
			break;
#endif
#endif
#ifdef APIKEY
		case MODE_EXT_TEMP:
#ifdef DEBUG
			Serial.println(String(outdoorWeather.temperature));
#endif
			renderer.clearScreenBuffer(matrix);
			if (outdoorWeather.temperature > 0)
			{
				matrix[1] = 0b0100000000000000;
				matrix[2] = 0b1110000000000000;
				matrix[3] = 0b0100000000000000;
			}
			if (outdoorWeather.temperature < 0)
			{
				matrix[2] = 0b1110000000000000;
			}
			renderer.setSmallText(String(int(abs(outdoorWeather.temperature) + 0.5)), TEXT_POS_BOTTOM, matrix);
			break;
		case MODE_EXT_HUMIDITY:
#ifdef DEBUG
			Serial.println(String(outdoorWeather.humidity));
#endif
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
			break;
#endif
		case MODE_BLANK:
			renderer.clearScreenBuffer(matrix);
			break;
		case MODE_FEED:
			for (uint8_t y = 0; y <= 5; y++)
			{
				renderer.clearScreenBuffer(matrix);
				for (uint8_t z = 0; z <= 6; z++)
				{
					matrix[2 + z] |= (lettersBig[feedText[feedPosition] - 32][z] << 11 + y) & 0b1111111111100000;
					matrix[2 + z] |= (lettersBig[feedText[feedPosition + 1] - 32][z] << 5 + y) & 0b1111111111100000;
					matrix[2 + z] |= (lettersBig[feedText[feedPosition + 2] - 32][z] << y - 1) & 0b1111111111100000;
				}
				writeScreenBuffer(matrix, feedColor, brightness);
				delay(FEED_SPEED);
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

		//debugScreenBuffer(matrixOld);
#ifdef DEBUG_MATRIX
		debugScreenBuffer(matrix);
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
				if (minute() % 5 == 0)
					moveScreenBufferUp(matrixOld, matrix, settings.mySettings.color, brightness);
				else
					writeScreenBuffer(matrix, settings.mySettings.color, brightness);
			}
			break;
#ifdef SHOW_MODE_TEST
		case MODE_RED:
			writeScreenBuffer(matrix, RED, TEST_BRIGHTNESS);
			break;
		case MODE_GREEN:
			writeScreenBuffer(matrix, GREEN, TEST_BRIGHTNESS);
			break;
		case MODE_BLUE:
			writeScreenBuffer(matrix, BLUE, TEST_BRIGHTNESS);
			break;
		case MODE_WHITE:
			writeScreenBuffer(matrix, WHITE, TEST_BRIGHTNESS);
			break;
#endif
		case MODE_FEED:
			writeScreenBuffer(matrix, feedColor, brightness);
			break;
		default:
			if (runTransitionOnce)
			{
				if (settings.mySettings.transition == TRANSITION_NORMAL)
					writeScreenBuffer(matrix, settings.mySettings.color, brightness);
				if (settings.mySettings.transition == TRANSITION_FADE)
					writeScreenBufferFade(matrixOld, matrix, settings.mySettings.color, brightness);
				if (settings.mySettings.transition == TRANSITION_MOVEUP)
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
	if ((millis() > (modeTimeout + settings.mySettings.timeout * 1000)) && modeTimeout) setMode(MODE_TIME);

#ifdef DEBUG_FPS
	debugFps();
#endif
} // loop()

//******************************************************************************
// Transitions
//******************************************************************************

void moveScreenBufferUp(uint16_t screenBufferOld[], uint16_t screenBufferNew[], uint8_t color, uint8_t brightness)
{
	for (uint8_t z = 0; z <= 9; z++)
	{
		for (uint8_t i = 0; i <= 8; i++)
			screenBufferOld[i] = screenBufferOld[i + 1];
		screenBufferOld[9] = screenBufferNew[z];
		writeScreenBuffer(screenBufferOld, color, brightness);
		webServer.handleClient();
		delay(50);
	}
}

void writeScreenBuffer(uint16_t screenBuffer[], uint8_t color, uint8_t brightness)
{
	ledDriver.clear();
	for (uint8_t y = 0; y <= 9; y++)
	{
		for (uint8_t x = 0; x <= 10; x++)
		{
			if (bitRead(screenBuffer[y], 15 - x))
				ledDriver.setPixel(x, y, color, brightness);
		}
	}

	// Corner LEDs
	for (uint8_t y = 0; y <= 3; y++)
	{
		if (bitRead(screenBuffer[y], 4))
			ledDriver.setPixel(110 + y, color, brightness);
	}

	// Alarm LED
#ifdef BUZZER
	if (bitRead(screenBuffer[4], 4))
	{
#ifdef ALARM_LED_COLOR
#ifdef ABUSE_CORNER_LED_FOR_ALARM
		if (settings.mySettings.alarm1 || settings.mySettings.alarm2 || alarmTimerSet)
			ledDriver.setPixel(111, ALARM_LED_COLOR, brightness);
		else
			if (bitRead(screenBuffer[1], 4))
				ledDriver.setPixel(111, color, brightness);
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
				brightnessBuffer[y][x] = brightness;
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
					brightnessBuffer[y][x]++;
				if ((bitRead(screenBufferOld[y], 15 - x)) && !(bitRead(screenBufferNew[y], 15 - x)))
					brightnessBuffer[y][x]--;
				ledDriver.setPixel(x, y, color, brightnessBuffer[y][x]);
			}
		}

		// Corner LEDs
		for (uint8_t y = 0; y <= 3; y++)
			ledDriver.setPixel(110 + y, color, brightnessBuffer[y][11]);

		// Alarm LED
#ifdef BUZZER
#ifdef ALARM_LED_COLOR
#ifdef ABUSE_CORNER_LED_FOR_ALARM
		if (settings.mySettings.alarm1 || settings.mySettings.alarm2 || alarmTimerSet)
			ledDriver.setPixel(111, ALARM_LED_COLOR, brightnessBuffer[4][11]);
		else
			ledDriver.setPixel(111, color, brightnessBuffer[1][11]);
#else
		ledDriver.setPixel(114, ALARM_LED_COLOR, brightnessBuffer[4][11]);
#endif
#else
		ledDriver.setPixel(114, color, brightnessBuffer[4][11]);
#endif
#endif
		webServer.handleClient();
		ledDriver.show();
	}
}

//******************************************************************************
// "On/off" pressed
//******************************************************************************

void buttonOnOffPressed()
{
#ifdef DEBUG
	Serial.println("On/off pressed.");
#endif

	mode == MODE_BLANK ? setLedsOn() : setLedsOff();
}

//******************************************************************************
// "Time" pressed
//******************************************************************************

void buttonTimePressed()
{
#ifdef DEBUG
	Serial.println("Time pressed.");
#endif

	// Switch off alarm
#ifdef BUZZER
	if (alarmOn)
	{
#ifdef DEBUG
		Serial.println("Alarm: off");
#endif
		digitalWrite(PIN_BUZZER, LOW);
		alarmOn = false;
	}
#endif

	modeTimeout = 0;
	setMode(MODE_TIME);
}

//******************************************************************************
// "Mode" pressed
//******************************************************************************

void buttonModePressed()
{
#ifdef DEBUG
	Serial.println("Mode pressed.");
#endif

	// Switch off alarm
#ifdef BUZZER
	if (alarmOn)
	{
#ifdef DEBUG
		Serial.println("Alarm: off");
#endif
		digitalWrite(PIN_BUZZER, LOW);
		alarmOn = false;
		setMode(MODE_TIME);
		return;
	}
#endif

	setMode(mode++);
}

//******************************************************************************
// Set mode
//******************************************************************************

void setMode(Mode newMode)
{
	screenBufferNeedsUpdate = true;
	runTransitionOnce = true;
	lastMode = mode;
	mode = newMode;

	// set timeout
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
#if defined(RTC_BACKUP) && !defined(SENSOR_DHT22)
	case MODE_TEMP:
#endif
#ifdef SENSOR_DHT22
	case MODE_TEMP:
	case MODE_HUMIDITY:
#endif
#ifdef APIKEY
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

//******************************************************************************
// Get brightness from LDR
//******************************************************************************

#ifdef LDR
void setBrightnessFromLdr()
{
#ifdef LDR_IS_INVERSE
	ldrValue = 1024 - analogRead(PIN_LDR);
#else
	ldrValue = analogRead(PIN_LDR);
#endif
	if (ldrValue < minLdrValue)
		minLdrValue = ldrValue;
	if (ldrValue > maxLdrValue)
		maxLdrValue = ldrValue;
	if ((ldrValue >= (lastLdrValue + 40)) || (ldrValue <= (lastLdrValue - 40))) // Hysteresis is 40
	{
		lastLdrValue = ldrValue;
		brightness = map(ldrValue, minLdrValue, maxLdrValue, MIN_BRIGHTNESS, maxBrightness);
		screenBufferNeedsUpdate = true;
#ifdef DEBUG
		Serial.printf("Brightness: %u (min: %u, max: %u)\r\n", brightness, MIN_BRIGHTNESS, maxBrightness);
		Serial.printf("LDR: %u (min: %u, max: %u)\r\n", ldrValue, minLdrValue, maxLdrValue);
#endif
	}
}
#endif

//******************************************************************************
// Get update info from server
//******************************************************************************

#ifdef UPDATE_INFOSERVER
void getUpdateInfo()
{
	WiFiClient wifiClient;
	HttpClient httpClient = HttpClient(wifiClient, UPDATE_INFOSERVER, 80);
	httpClient.get(UPDATE_INFOFILE);
	if (httpClient.responseStatusCode() == 200)
	{
		String response = httpClient.responseBody();
		response.trim();
		JSONVar updateArray = JSON.parse(response);
		updateInfo = (int)updateArray["channel"]["unstable"]["version"];
	}
#ifdef DEBUG
	updateInfo > int(FIRMWARE_VERSION) ? Serial.println("Firmwareupdate available! (" + String(updateInfo) + ")") : Serial.println("Firmware is uptodate.");
#endif
}
#endif

//******************************************************************************
// Get room conditions
//******************************************************************************

#if defined(RTC_BACKUP) || defined(SENSOR_DHT22)
void getRoomConditions()
{
#if defined(RTC_BACKUP) && !defined(SENSOR_DHT22)
	roomTemperature = RTC.temperature() / 4.0 + RTC_TEMPERATURE_OFFSET;
#ifdef DEBUG
	Serial.println("Temperature (RTC): " + String(roomTemperature) + "C");
#endif
#endif
#ifdef SENSOR_DHT22
	float dhtTemperature = dht.readTemperature();
	float dhtHumidity = dht.readHumidity();
	if (!isnan(dhtTemperature) && !isnan(dhtHumidity))
	{
		errorCounterDHT = 0;
		roomTemperature = dhtTemperature + DHT_TEMPERATURE_OFFSET;
		roomHumidity = dhtHumidity + DHT_HUMIDITY_OFFSET;
#ifdef DEBUG
		Serial.println("Temperature (DHT): " + String(roomTemperature) + "C");
		Serial.println("Humidity (DHT): " + String(roomHumidity) + "%");
#endif
	}
	else
	{
		if (errorCounterDHT < 255)
			errorCounterDHT++;
#ifdef DEBUG
		Serial.printf("Error (DHT): %u\r\n", errorCounterDHT);
#endif
	}
#endif
}
#endif

//******************************************************************************
// Misc
//******************************************************************************

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

// Switch off LEDs
void setLedsOff()
{
#ifdef DEBUG
	Serial.println("LEDs: off");
#endif
	setMode(MODE_BLANK);
}

// Switch on LEDs
void setLedsOn()
{
#ifdef DEBUG
	Serial.println("LEDs: on");
#endif
	setMode(lastMode);
}

// Calculate moonphase
#ifdef SHOW_MODE_MOONPHASE
int getMoonphase(int y, int m, int d)
{
	int b;
	int c;
	int e;
	double jd;
	if (m < 3)
	{
		y--;
		m += 12;
	}
	++m;
	c = 365.25 * y;
	e = 30.6 * m;
	jd = c + e + d - 694039.09; // jd is total days elapsed
	jd /= 29.53;                // divide by the moon cycle (29.53 days)
	b = jd;                     // int(jd) -> b, take integer part of jd
	jd -= b;                    // subtract integer part to leave fractional part of original jd
	b = jd * 8 + 0.5;           // scale fraction from 0-8 and round by adding 0.5
	b = b & 7;                  // 0 and 8 are the same so turn 8 into 0
	return b;
}
#endif

// Write screenbuffer to console
#ifdef DEBUG_MATRIX
void debugScreenBuffer(uint16_t screenBuffer[])
{
	const char buchstabensalat[][12] =
	{
	  { 'E', 'S', 'K', 'I', 'S', 'T', 'A', 'F', 'U', 'N', 'F', '1' },
	  { 'Z', 'E', 'H', 'N', 'Z', 'W', 'A', 'N', 'Z', 'I', 'G', '2' },
	  { 'D', 'R', 'E', 'I', 'V', 'I', 'E', 'R', 'T', 'E', 'L', '3' },
	  { 'V', 'O', 'R', 'F', 'U', 'N', 'K', 'N', 'A', 'C', 'H', '4' },
	  { 'H', 'A', 'L', 'B', 'A', 'E', 'L', 'F', 'U', 'N', 'F', 'A' },
	  { 'E', 'I', 'N', 'S', 'X', 'A', 'M', 'Z', 'W', 'E', 'I', ' ' },
	  { 'D', 'R', 'E', 'I', 'P', 'M', 'J', 'V', 'I', 'E', 'R', ' ' },
	  { 'S', 'E', 'C', 'H', 'S', 'N', 'L', 'A', 'C', 'H', 'T', ' ' },
	  { 'S', 'I', 'E', 'B', 'E', 'N', 'Z', 'W', 'O', 'L', 'F', ' ' },
	  { 'Z', 'E', 'H', 'N', 'E', 'U', 'N', 'K', 'U', 'H', 'R', ' ' }
	};
	//Serial.println("\033[0;0H"); // set cursor to 0, 0 position
	Serial.println(" -----------");
	for (uint8_t y = 0; y <= 9; y++)
	{
		Serial.print('|');
		for (uint8_t x = 0; x <= 10; x++)
		{
			Serial.print((bitRead(screenBuffer[y], 15 - x) ? buchstabensalat[y][x] : ' '));
		}
		Serial.print('|');
		Serial.println((bitRead(screenBuffer[y], 4) ? buchstabensalat[y][11] : ' '));
	}
	Serial.println(" -----------");
}
#endif

// Write FPS to console
#ifdef DEBUG_FPS
void debugFps()
{
	static uint16_t frames;
	static uint32_t lastFpsCheck;
	frames++;
	if ((millis() % 1000 == 0) && (millis() != lastFpsCheck))
	{
		lastFpsCheck = millis();
		Serial.printf("FPS: %u\r\n", frames);
		frames = 0;
	}
}
#endif

//******************************************************************************
// Webserver
//******************************************************************************

void setupWebServer()
{
	webServer.onNotFound(handleNotFound);
	webServer.on("/", handleRoot);
	webServer.on("/handleButtonOnOff", []() { buttonOnOffPressed(); callRoot(); });
	webServer.on("/handleButtonSettings", handleButtonSettings);
	webServer.on("/handleButtonMode", []() { buttonModePressed(); callRoot(); });
	webServer.on("/handleButtonTime", []() {    buttonTimePressed(); callRoot(); });
	webServer.on("/commitSettings", handleCommitSettings);
	webServer.on("/reset", handleReset);
	webServer.on("/setEvent", handleSetEvent);
	webServer.on("/showText", handleShowText);
	webServer.on("/control", handleControl);
	webServer.begin();
}

void callRoot()
{
	webServer.send(200, "text/html", "<!doctype html><html><head><script>window.onload=function(){window.location.replace('/');}</script></head></html>");
}

// Page 404
void handleNotFound()
{
	webServer.send(404, "text/plain", "404 - File Not Found.");
}

// Page /
void handleRoot()
{
	String message = "<!doctype html>"
		"<html>"
		"<head>"
		"<title>" + String(WEBSITE_TITLE) + "</title>"
		"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"
		"<meta http-equiv=\"refresh\" content=\"60\" charset=\"UTF-8\">"
		"<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css\">"
		"<style>"
		"body{background-color:#FFFFFF;text-align:center;color:#333333;font-family:Sans-serif;font-size:16px;}"
		"button{background-color:#1FA3EC;text-align:center;color:#FFFFFF;width:200px;padding:10px;border:5px solid #FFFFFF;font-size:24px;border-radius:10px;}"
		"</style>"
		"</head>"
		"<body>"
		"<h1>" + String(WEBSITE_TITLE) + "</h1>";
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
#if defined(RTC_BACKUP) || defined(SENSOR_DHT22)
	message += "<br><br><i class = \"fa fa-home\" style=\"font-size:20px;\"></i>";
	message += "<br><i class=\"fa fa-thermometer\" style=\"font-size:20px;\"></i> " + String(roomTemperature) + "&deg;C / " + String(roomTemperature * 1.8 + 32.0) + "&deg;F";
#endif
#ifdef SENSOR_DHT22
	message += "<br><i class=\"fa fa-tint\" style=\"font-size:20px;\"></i> " + String(roomHumidity) + "% RH"
		"<br><span style=\"font-size:20px;\">";
	if (roomHumidity < 30)
		message += "<i style=\"color:Red;\" class=\"fa fa-square\"\"></i>";
	else
		message += "<i style=\"color:Red;\" class=\"fa fa-square-o\"></i>";
	if ((roomHumidity >= 30) && (roomHumidity < 40))
		message += "&nbsp;<i style=\"color:Orange;\" class=\"fa fa-square\"></i>";
	else
		message += "&nbsp;<i style=\"color:Orange;\" class=\"fa fa-square-o\"></i>";
	if ((roomHumidity >= 40) && (roomHumidity <= 50))
		message += "&nbsp;<i style=\"color:MediumSeaGreen;\" class=\"fa fa-square\"></i>";
	else
		message += "&nbsp;<i style=\"color:MediumSeaGreen;\" class=\"fa fa-square-o\"></i>";
	if ((roomHumidity > 50) && (roomHumidity < 60))
		message += "&nbsp;<i style=\"color:Lightblue;\" class=\"fa fa-square\"></i>";
	else
		message += "&nbsp;<i style=\"color:Lightblue;\" class=\"fa fa-square-o\"></i>";
	if (roomHumidity >= 60)
		message += "&nbsp;<i style=\"color:Blue;\" class=\"fa fa-square\"></i>";
	else
		message += "&nbsp;<i style=\"color:Blue;\" class=\"fa fa-square-o\"></i>";
	message += "</span>";
#endif
#ifdef APIKEY
	message += "<br><br><i class = \"fa fa-tree\" style=\"font-size:20px;\"></i>"
		"<br><i class = \"fa fa-thermometer\" style=\"font-size:20px;\"></i> " + String(outdoorWeather.temperature) + "&deg;C / " + String(outdoorWeather.temperature * 1.8 + 32.0) + "&deg;F"
		"<br><i class = \"fa fa-tint\" style=\"font-size:20px;\"></i> " + String(outdoorWeather.humidity) + "% RH"
		"<br>" + String(outdoorWeather.pressure) + " hPa / " + String(outdoorWeather.pressure / 33.865) + " inHg"
		"<br>" + outdoorWeather.description;
#endif
	message += "<span style=\"font-size:12px;\">"
		"<br><br><a href=\"http://tmw-it.ch/qlockwork/\">Qlockwork</a> was <i class=\"fa fa-code\"></i> with <i class=\"fa fa-heart\"></i> by ch570512"
		"<br>Firmware: " + String(FIRMWARE_VERSION);
#ifdef UPDATE_INFOSERVER
	if (updateInfo > int(FIRMWARE_VERSION))
		message += "<br><span style=\"color:red;\">Firmwareupdate available! (" + String(updateInfo) + ")</span>";
#endif
#ifdef DEBUG_WEB
	time_t tempEspTime = now();
	message += "<br><br>Time: " + String(hour(tempEspTime)) + ":";
	if (minute(tempEspTime) < 10)
		message += "0";
	message += String(minute(tempEspTime));
	if (timeZone.locIsDST(now()))
		message += " (DST)";
	message += " up " + String(int(upTime / 86400)) + " days, " + String(hour(upTime)) + ":";
	if (minute(upTime) < 10)
		message += "0";
	message += String(minute(upTime));
	message += "<br>" + String(dayStr(weekday(tempEspTime))) + ", " + String(monthStr(month(tempEspTime))) + " " + String(day(tempEspTime)) + ". " + String(year(tempEspTime));
	message += "<br>Moonphase: " + String(moonphase);
	message += "<br>Free RAM: " + String(ESP.getFreeHeap()) + " bytes";
	message += "<br>RSSI: " + String(WiFi.RSSI());
#ifdef LDR
	message += "<br>Brightness: " + String(brightness) + " (ABC: ";
	settings.mySettings.useAbc ? message += "enabled" : message += "disabled";
	message += ", min: " + String(MIN_BRIGHTNESS) + ", max : " + String(maxBrightness) + ")";
	message += "<br>LDR: " + String(ldrValue) + " (min: " + String(minLdrValue) + ", max: " + String(maxLdrValue) + ")";
#endif
	message += "<br>Error (NTP): " + String(errorCounterNTP);
#ifdef SENSOR_DHT22
	message += "<br>Error (DHT): " + String(errorCounterDHT);
#endif
#ifdef APIKEY
	message += "<br>Error (OpenWeather): " + String(errorCounterOutdoorWeather);
#endif
	message += "<br>Reset reason: " + ESP.getResetReason();
	message += "<br>Flags: ";
#ifdef RTC_BACKUP
	message += "RTC ";
#else
	message += "<s>RTC</s> ";
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
#endif
	message += "</span></body></html>";
	webServer.send(200, "text/html", message);
}

// Page settings
void handleButtonSettings()
{
#ifdef DEBUG
	Serial.println("Settings pressed.");
#endif
	String message = "<!doctype html>"
		"<html>"
		"<head>"
		"<title>" + String(HOSTNAME) + " " TXT_SETTINGS "</title>"
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
		"<h1>" + String(HOSTNAME) + " " TXT_SETTINGS "</h1>"
		"<form action=\"/commitSettings\">"
		"<table>";
	// ------------------------------------------------------------------------
#ifdef BUZZER
	message += "<tr><td>"
		TXT_ALARM
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
	if (hour(settings.mySettings.alarm1Time) < 10)
		message += "0";
	message += String(hour(settings.mySettings.alarm1Time)) + ":";
	if (minute(settings.mySettings.alarm1Time) < 10)
		message += "0";
	message += String(minute(settings.mySettings.alarm1Time)) + "\">"
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
	message += "<tr><td>"
		TXT_ALARM
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
	if (hour(settings.mySettings.alarm2Time) < 10)
		message += "0";
	message += String(hour(settings.mySettings.alarm2Time)) + ":";
	if (minute(settings.mySettings.alarm2Time) < 10)
		message += "0";
	message += String(minute(settings.mySettings.alarm2Time)) + "\">"
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
	message += "<tr><td>"
		TXT_TIMER
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
	message += "<tr><td>"
		TXT_HOURBEEP
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
#if defined(RTC_BACKUP) || defined(SENSOR_DHT22)
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
	if (settings.mySettings.color == 0) message += " selected";
	message += ">"
		"White</option>"
		"<option value=\"1\"";
	if (settings.mySettings.color == 1) message += " selected";
	message += ">"
		"Red</option>"
		"<option value=\"2\"";
	if (settings.mySettings.color == 2) message += " selected";
	message += ">"
		"Red 75%</option>"
		"<option value=\"3\"";
	if (settings.mySettings.color == 3) message += " selected";
	message += ">"
		"Red 50%</option>"
		"<option value=\"4\"";
	if (settings.mySettings.color == 4) message += " selected";
	message += ">"
		"Orange</option>"
		"<option value=\"5\"";
	if (settings.mySettings.color == 5) message += " selected";
	message += ">"
		"Yellow</option>"
		"<option value=\"6\"";
	if (settings.mySettings.color == 6) message += " selected";
	message += ">"
		"Yellow 75%</option>"
		"<option value=\"7\"";
	if (settings.mySettings.color == 7) message += " selected";
	message += ">"
		"Yellow 50%</option>"
		"<option value=\"8\"";
	if (settings.mySettings.color == 8) message += " selected";
	message += ">"
		"Green-Yellow</option>"
		"<option value=\"9\"";
	if (settings.mySettings.color == 9) message += " selected";
	message += ">"
		"Green</option>"
		"<option value=\"10\"";
	if (settings.mySettings.color == 10) message += " selected";
	message += ">"
		"Green 75%</option>"
		"<option value=\"11\"";
	if (settings.mySettings.color == 11) message += " selected";
	message += ">"
		"Green 50%</option>"
		"<option value=\"12\"";
	if (settings.mySettings.color == 12) message += " selected";
	message += ">"
		"Mintgreen</option>"
		"<option value=\"13\"";
	if (settings.mySettings.color == 13) message += " selected";
	message += ">"
		"Cyan</option>"
		"<option value=\"14\"";
	if (settings.mySettings.color == 14) message += " selected";
	message += ">"
		"Cyan 75%</option>"
		"<option value=\"15\"";
	if (settings.mySettings.color == 15) message += " selected";
	message += ">"
		"Cyan 50%</option>"
		"<option value=\"16\"";
	if (settings.mySettings.color == 16) message += " selected";
	message += ">"
		"Light Blue</option>"
		"<option value=\"17\"";
	if (settings.mySettings.color == 17) message += " selected";
	message += ">"
		"Blue</option>"
		"<option value=\"18\"";
	if (settings.mySettings.color == 18) message += " selected";
	message += ">"
		"Blue 75%</option>"
		"<option value=\"19\"";
	if (settings.mySettings.color == 19) message += " selected";
	message += ">"
		"Blue 50%</option>"
		"<option value=\"20\"";
	if (settings.mySettings.color == 20) message += " selected";
	message += ">"
		"Violet</option>"
		"<option value=\"21\"";
	if (settings.mySettings.color == 21) message += " selected";
	message += ">"
		"Magenta</option>"
		"<option value=\"22\"";
	if (settings.mySettings.color == 22) message += " selected";
	message += ">"
		"Magenta 75%</option>"
		"<option value=\"23\"";
	if (settings.mySettings.color == 23) message += " selected";
	message += ">"
		"Magenta 50%</option>"
		"<option value=\"24\"";
	if (settings.mySettings.color == 24) message += " selected";
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
	if (hour(settings.mySettings.nightOffTime) < 10)
		message += "0";
	message += String(hour(settings.mySettings.nightOffTime)) + ":";
	if (minute(settings.mySettings.nightOffTime) < 10)
		message += "0";
	message += String(minute(settings.mySettings.nightOffTime)) + "\">"
		" h"
		"</td></tr>";
	// ------------------------------------------------------------------------
	message += "<tr><td>"
		"Day on"
		"</td><td>"
		"<input type=\"time\" name=\"do\" value=\"";
	if (hour(settings.mySettings.dayOnTime) < 10)
		message += "0";
	message += String(hour(settings.mySettings.dayOnTime)) + ":";
	if (minute(settings.mySettings.dayOnTime) < 10)
		message += "0";
	message += String(minute(settings.mySettings.dayOnTime)) + "\">"
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
	webServer.send(200, "text/html", message);
}

void handleCommitSettings()
{
#ifdef DEBUG
	Serial.println("Commit settings pressed.");
#endif
	// ------------------------------------------------------------------------
#ifdef BUZZER
	time_t alarmTimeFromWeb = 0;
	webServer.arg("a1") == "0" ? settings.mySettings.alarm1 = false : settings.mySettings.alarm1 = true;
	alarmTimeFromWeb = webServer.arg("a1t").substring(0, 2).toInt() * 3600 + webServer.arg("a1t").substring(3, 5).toInt() * 60;
	if (settings.mySettings.alarm1Time != alarmTimeFromWeb)
		settings.mySettings.alarm1 = true;
	settings.mySettings.alarm1Time = alarmTimeFromWeb;
	settings.mySettings.alarm1Weekdays =
		webServer.arg("a1w1").toInt() +
		webServer.arg("a1w2").toInt() +
		webServer.arg("a1w3").toInt() +
		webServer.arg("a1w4").toInt() +
		webServer.arg("a1w5").toInt() +
		webServer.arg("a1w6").toInt() +
		webServer.arg("a1w7").toInt();
	// ------------------------------------------------------------------------
	webServer.arg("a2") == "0" ? settings.mySettings.alarm2 = false : settings.mySettings.alarm2 = true;
	alarmTimeFromWeb = webServer.arg("a2t").substring(0, 2).toInt() * 3600 + webServer.arg("a2t").substring(3, 5).toInt() * 60;
	if (settings.mySettings.alarm2Time != alarmTimeFromWeb)
		settings.mySettings.alarm2 = true;
	settings.mySettings.alarm2Time = alarmTimeFromWeb;
	settings.mySettings.alarm2Weekdays =
		webServer.arg("a2w1").toInt() +
		webServer.arg("a2w2").toInt() +
		webServer.arg("a2w3").toInt() +
		webServer.arg("a2w4").toInt() +
		webServer.arg("a2w5").toInt() +
		webServer.arg("a2w6").toInt() +
		webServer.arg("a2w7").toInt();
	// ------------------------------------------------------------------------
	webServer.arg("hb") == "0" ? settings.mySettings.hourBeep = false : settings.mySettings.hourBeep = true;
	// ------------------------------------------------------------------------
	if (webServer.arg("ti").toInt())
	{
		alarmTimer = webServer.arg("ti").toInt();
		alarmTimerSecond = second();
		alarmTimerSet = true;
		setMode(MODE_TIMER);
#ifdef DEBUG
		Serial.println("Timer started.");
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
			Serial.println("Timer stopped.");
#endif
		}
	}
#endif
	// ------------------------------------------------------------------------
#if defined(RTC_BACKUP) || defined(SENSOR_DHT22)
	webServer.arg("mc") == "0" ? settings.mySettings.modeChange = false : settings.mySettings.modeChange = true;
#endif
	// ------------------------------------------------------------------------
#ifdef LDR
	if (webServer.arg("ab") == "0")
	{
		settings.mySettings.useAbc = false;
		brightness = maxBrightness;
	}
	else
		settings.mySettings.useAbc = true;
#endif
	// ------------------------------------------------------------------------
	settings.mySettings.brightness = webServer.arg("br").toInt();
	maxBrightness = map(settings.mySettings.brightness, 0, 100, MIN_BRIGHTNESS, MAX_BRIGHTNESS);
	brightness = maxBrightness;
	// ------------------------------------------------------------------------
	settings.mySettings.color = webServer.arg("co").toInt();
	// ------------------------------------------------------------------------
	switch (webServer.arg("cc").toInt())
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
	switch (webServer.arg("tr").toInt())
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
	settings.mySettings.timeout = webServer.arg("to").toInt();
	// ------------------------------------------------------------------------
	settings.mySettings.nightOffTime = webServer.arg("no").substring(0, 2).toInt() * 3600 + webServer.arg("no").substring(3, 5).toInt() * 60;
	// ------------------------------------------------------------------------
	settings.mySettings.dayOnTime = webServer.arg("do").substring(0, 2).toInt() * 3600 + webServer.arg("do").substring(3, 5).toInt() * 60;
	// ------------------------------------------------------------------------
	webServer.arg("ii") == "0" ? settings.mySettings.itIs = false : settings.mySettings.itIs = true;
	// ------------------------------------------------------------------------
	if (webServer.arg("st").length())
	{
		Serial.println(webServer.arg("st"));
		setTime(webServer.arg("st").substring(11, 13).toInt(), webServer.arg("st").substring(14, 16).toInt(), 0, webServer.arg("st").substring(8, 10).toInt(), webServer.arg("st").substring(5, 7).toInt(), webServer.arg("st").substring(0, 4).toInt());
#ifdef RTC_BACKUP
		RTC.set(timeZone.toUTC(now()));
#endif
	}
	// ------------------------------------------------------------------------
	settings.saveToEEPROM();
	callRoot();
	screenBufferNeedsUpdate = true;
}

// Page reset
void handleReset()
{
	webServer.send(200, "text/plain", "OK. I'll be back!");
	ESP.restart();
}

// Page setEvent
void handleSetEvent()
{
	events[0].day = webServer.arg("day").toInt();
	events[0].month = webServer.arg("month").toInt();
	events[0].text = webServer.arg("text").substring(0, 40);
	events[0].color = (eColor)webServer.arg("color").toInt();;
	webServer.send(200, "text/plain", "OK.");

#ifdef DEBUG
	Serial.println("Event set: " + String(events[0].day) + "." + String(events[0].month) + ". " + events[0].text);
#endif
}

// Page showText
void handleShowText()
{
	uint8_t feedBuzzer = webServer.arg("buzzer").toInt();
	feedColor = webServer.arg("color").toInt();
	feedText = "  " + webServer.arg("text").substring(0, 80) + "   ";
	feedPosition = 0;
	webServer.send(200, "text/plain", "OK.");

#ifdef DEBUG
	Serial.println("Show text: " + webServer.arg("text").substring(0, 80));
#endif

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

void handleControl()
{
	setMode((Mode)webServer.arg("mode").toInt());
	webServer.send(200, "text/plain", "OK.");
}
