//******************************************************************************
// Configuration.h
// See README.txt for help
//******************************************************************************

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

//******************************************************************************
// Software settings
//******************************************************************************

#define HOSTNAME "QLOCKWORK"
#define WEBSITE_TITLE "QLOCKWORKs configuration page"
//#define DEDICATION "The only reason for time is so that everything doesn't happen at once.<br>(Albert Einstein)"
#define WIFI_SETUP_TIMEOUT 120
#define WIFI_AP_PASS "12345678"
#define OTA_PASS "1234"
#define NTP_SERVER "pool.ntp.org"
#define SHOW_IP
//#define WIFI_BEEPS

//#define NONE_TECHNICAL_ZERO
#define AUTO_MODECHANGE_TIME 60
#define FEED_SPEED 120
#define EVENT_TIME 300
#define ALARM_LED_COLOR RED
//#define ABUSE_CORNER_LED_FOR_ALARM

//#define POWERON_SELFTEST
#define SHOW_MODE_AMPM
#define SHOW_MODE_SECONDS
#define SHOW_MODE_WEEKDAY
#define SHOW_MODE_DATE
#define SHOW_MODE_MOONPHASE
#define SHOW_MODE_TEST

//#define APIKEY "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
#define LOCATION "Berlin, DE"
//#define LOCATION "Zurich, CH"
//#define LOCATION "Hong Kong, HK"

//#define FRONTCOVER_EN
#define FRONTCOVER_DE_DE
//#define FRONTCOVER_DE_SW
//#define FRONTCOVER_DE_BA
//#define FRONTCOVER_DE_SA
//#define FRONTCOVER_DE_MKF_DE
//#define FRONTCOVER_DE_MKF_SW
//#define FRONTCOVER_DE_MKF_BA
//#define FRONTCOVER_DE_MKF_SA
//#define FRONTCOVER_D3
//#define FRONTCOVER_CH
//#define FRONTCOVER_CH_GS
//#define FRONTCOVER_ES
//#define FRONTCOVER_FR
//#define FRONTCOVER_IT
//#define FRONTCOVER_NL
//#define FRONTCOVER_BINARY

//******************************************************************************
// Timezone
//******************************************************************************

//#define TIMEZONE_IDLW  // IDLW  International Date Line West UTC-12
//#define TIMEZONE_SST   // SST   Samoa Standard Time UTC-11
//#define TIMEZONE_HST   // HST   Hawaiian Standard Time UTC-10
//#define TIMEZONE_AKST  // AKST  Alaska Standard Time UTC-9
//#define TIMEZONE_USPST // USPST Pacific Standard Time (USA) UTC-8
//#define TIMEZONE_USMST // USMST Mountain Standard Time (USA) UTC-7
//#define TIMEZONE_USAZ  // USAZ  Mountain Standard Time (USA) UTC-7 (no DST)
//#define TIMEZONE_USCST // USCST Central Standard Time (USA) UTC-6
//#define TIMEZONE_USEST // USEST Eastern Standard Time (USA) UTC-5
//#define TIMEZONE_AST   // AST   Atlantic Standard Time UTC-4
//#define TIMEZONE_BST   // BST   Eastern Brazil Standard Time UTC-3
//#define TIMEZONE_VTZ   // VTZ   Greenland Eastern Standard Time UTC-2
//#define TIMEZONE_AZOT  // AZOT  Azores Time UTC-1
//#define TIMEZONE_GMT   // GMT   Greenwich Mean Time UTC
#define TIMEZONE_CET     // CET   Central Europe Time UTC+1
//#define TIMEZONE_EST   // EST   Eastern Europe Time UTC+2
//#define TIMEZONE_MSK   // MSK   Moscow Time UTC+3 (no DST)
//#define TIMEZONE_GST   // GST   Gulf Standard Time UTC+4
//#define TIMEZONE_PKT   // PKT   Pakistan Time UTC+5
//#define TIMEZONE_BDT   // BDT   Bangladesh Time UTC+6
//#define TIMEZONE_JT    // JT    Java Time UTC+7
//#define TIMEZONE_CNST  // CNST  China Standard Time UTC+8
//#define TIMEZONE_HKT   // HKT   Hong Kong Time UTC+8
//#define TIMEZONE_PYT   // PYT   Pyongyang Time (North Korea) UTC+8.5
//#define TIMEZONE_CWT   // CWT   Central West Time (Australia) UTC+8.75
//#define TIMEZONE_JST   // JST   Japan Standard Time UTC+9
//#define TIMEZONE_ACST  // ACST  Australian Central Standard Time UTC+9.5
//#define TIMEZONE_AEST  // AEST  Australian Eastern Standard Time UTC+10
//#define TIMEZONE_LHST  // LHST  Lord Howe Standard Time UTC+10.5
//#define TIMEZONE_SBT   // SBT   Solomon Islands Time UTC+11
//#define TIMEZONE_NZST  // NZST  New Zealand Standard Time UTC+12

//******************************************************************************
// Hardware settings
//******************************************************************************

#define SERIAL_SPEED 115200

#define NUMPIXELS 115

//#define ONOFF_BUTTON
//#define MODE_BUTTON
//#define TIME_BUTTON

#define ESP_LED

#define MIN_BRIGHTNESS 20
#define MAX_BRIGHTNESS 255
#define TEST_BRIGHTNESS 80

//#define SENSOR_DHT22
#define DHT_TEMPERATURE_OFFSET 0.5
#define DHT_HUMIDITY_OFFSET -2.0

//#define RTC_BACKUP
#define RTC_TEMPERATURE_OFFSET -1.15

//#define LDR
//#define LDR_IS_INVERSE

//#define BUZZER
#define BUZZTIME_ALARM_1 30
#define BUZZTIME_ALARM_2 30
#define BUZZTIME_TIMER 30

//#define IR_RECEIVER
//#define IR_CODE_ONOFF 16769565 // HX1838 Remote CH+
//#define IR_CODE_TIME  16753245 // HX1838 Remote CH-
//#define IR_CODE_MODE  16736925 // HX1838 Remote CH
#define IR_CODE_ONOFF 0xFFE01F // CLT2 V1.1 Remote Power
#define IR_CODE_TIME  0xFFA05F // CLT2 V1.1 Remote Time
#define IR_CODE_MODE  0xFF20DF // CLT2 V1.1 Remote Region

//#define IR_LETTER_OFF
#define IR_LETTER_X 8
#define IR_LETTER_Y 10

#define LED_LAYOUT_HORIZONTAL_1
//#define LED_LAYOUT_VERTICAL_1
//#define LED_LAYOUT_VERTICAL_2

#define NEOPIXEL_TYPE NEO_GRB + NEO_KHZ800     // see Adafruit_NeoPixel.h for help
//#define NEOPIXEL_TYPE NEO_GRBW + NEO_KHZ800

//******************************************************************************
// Misc
//******************************************************************************

//#define DEBUG
//#define DEBUG_WEB
//#define DEBUG_IR
//#define DEBUG_MATRIX
//#define DEBUG_FPS

//#define SYSLOGSERVER_SERVER "192.168.0.1"
//#define SYSLOGSERVER_PORT 514

#define UPDATE_INFOSERVER "tmw-it.ch"
#define UPDATE_INFOFILE "/qlockwork/updateinfo.json"

// ESP8266
#define PIN_IR_RECEIVER  16 // D0 (no interrupt)
//#define PIN_WIRE_SCL     05 // D1 SCL
//#define PIN_WIRE_SDA     04 // D2 SDA
#define PIN_MODE_BUTTON  00 // D3 LOW_Flash
#define PIN_LED          02 // D4 ESP8266_LED
#define PIN_BUZZER       14 // D5
#define PIN_DHT22        12 // D6
#define PIN_LEDS_CLOCK   13 // D7
#define PIN_LEDS_DATA    15 // D8
#define PIN_LDR          A0 // ADC
#define PIN_TIME_BUTTON  01 // TXD0
#define PIN_ONOFF_BUTTON 03 // RXD0
// GPIO 06 to GPIO 11 are
// used for flash memory databus

#endif
