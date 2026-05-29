#pragma once

//*****************************************************************************
// Configuration_CH570512.h
//*****************************************************************************

//*****************************************************************************
// Software settings
//*****************************************************************************

#define HOSTNAME "QLOCKWORK"
#define WEBSITE_TITLE "QLOCKWORK"
#define WEBSERVER
#define ARDUINO_OTA
#define WIFI_AP_PASS "12345678" 

#define NONE_TECHNICAL_ZERO
#define AUTO_MODECHANGE_TIME 60
// #define EVENT_TIME 300
// #define ALARM_LED_COLOR RED
// #define ABUSE_CORNER_LED_FOR_ALARM

// #define SHOW_IP
#define POWERON_SELFTEST
#define SHOW_MODE_AMPM
#define SHOW_MODE_SECONDS
#define SHOW_MODE_WEEKDAY
#define SHOW_MODE_DATE
#define SHOW_MODE_MOONPHASE
#define SHOW_MODE_TEST

#define WEATHER
#define LATITUDE "47.309371"
#define LONGITUDE "8.243804"
#define TIMEZONE "Europe/Berlin"

// #define FRONTCOVER_EN
#define FRONTCOVER_DE_DE
// #define FRONTCOVER_DE_SW
// #define FRONTCOVER_DE_BA
// #define FRONTCOVER_DE_SA
// #define FRONTCOVER_DE_MKF_DE
// #define FRONTCOVER_DE_MKF_SW
// #define FRONTCOVER_DE_MKF_BA
// #define FRONTCOVER_DE_MKF_SA
// #define FRONTCOVER_D3
// #define FRONTCOVER_CH
// #define FRONTCOVER_CH_GS
// #define FRONTCOVER_ES
// #define FRONTCOVER_FR
// #define FRONTCOVER_IT
// #define FRONTCOVER_NL
// #define FRONTCOVER_BINARY

//*****************************************************************************
// NTP and Timezone - The timezone in which the clock is located.
// Important for the UTC offset and the summer/winter time (DST) change.
// https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
//*****************************************************************************

#define NTP_SERVER "pool.ntp.org"
#define NTP_TIMEZONE "CET-1CEST,M3.5.0,M10.5.0/3"

//*****************************************************************************
// Hardware settings
//*****************************************************************************

#define SERIAL_SPEED 115200
#define NUMPIXELS 114
// #define ONOFF_BUTTON
// #define MODE_BUTTON
// #define TIME_BUTTON
// #define ESP_LED
#define MIN_BRIGHTNESS 20
#define MAX_BRIGHTNESS 220

#define SENSOR_DHT22
#define DHT_TEMPERATURE_OFFSET -0.8
#define DHT_HUMIDITY_OFFSET +6.0

#define LDR
// #define LDR_IS_INVERSE

#define BUZZER
#define BUZZTIME_ALARM_1 30
#define BUZZTIME_ALARM_2 30
#define BUZZTIME_TIMER 30

#define IR_RECEIVER
// #define IR_CODE_ONOFF 16769565 // HX1838 Remote CH+
// #define IR_CODE_TIME  16753245 // HX1838 Remote CH-
// #define IR_CODE_MODE  16736925 // HX1838 Remote CH
#define IR_CODE_ONOFF 0xFFE01F // CLT2 V1.1 Remote Power
#define IR_CODE_TIME  0xFFA05F // CLT2 V1.1 Remote Time
#define IR_CODE_MODE  0xFF20DF // CLT2 V1.1 Remote Region

// #define IR_LETTER_OFF
#define IR_LETTER_X 8
#define IR_LETTER_Y 10

// #define LED_LAYOUT_HORIZONTAL_1
// #define LED_LAYOUT_VERTICAL_1
// #define LED_LAYOUT_VERTICAL_2
#define LED_LAYOUT_VERTICAL_3

// #define NEOPIXEL_RGB
#define NEOPIXEL_RGBW

// #define NEOPIXEL_TYPE NEO_GRB + NEO_KHZ800
// #define NEOPIXEL_TYPE NEO_WRGB + NEO_KHZ800
#define NEOPIXEL_TYPE NEO_GRBW + NEO_KHZ800

//*****************************************************************************
// Misc
//*****************************************************************************

// #define DEBUG
#define DEBUG_WEB
// #define DEBUG_MATRIX
// #define DEBUG_IR
// #define DEBUG_LDR
// #define DEBUG_FPS

// ESP8266
#define PIN_IR_RECEIVER  16 // D0 (no interrupt)
// #define PIN_WIRE_SCL     05 // D1 SCL
// #define PIN_WIRE_SDA     04 // D2 SDA
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