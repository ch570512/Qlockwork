//*****************************************************************************
// Configuration.h
//*****************************************************************************

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

//*****************************************************************************
// Software settings
//*****************************************************************************

#define HOSTNAME "QLOCKWORK"                    // The hostname of the clock.
#define WEBSITE_TITLE "QLOCKWORKs page"         // Title on top of the clocks webpage.
//#define DEDICATION "The only reason for time is so that everything doesn't happen at once.<br>(Albert Einstein)"
                                                // Show text on the clocks webpage.

#define WEBSERVER                               // Enable the webserver.
#define NTP_SERVER "pool.ntp.org"               // NTP server to be queried.

#define WIFI_SETUP_TIMEOUT 120                  // Time in seconds the WiFiManager waits for your input.
                                                // If no WiFi is connected the clock enters accesspoint mode.
                                                // You can control the clock by connecting to this accesspoint.
#define WIFI_AP_PASS "12345678"                 // The password for the AP. At least 8 characters.
#define SHOW_IP                                 // Show local IP at startup. Use this to access the clocks menue.
//#define WIFI_BEEPS                            // Beep 3 times if WIFI is conneced, if not, beep one time on startup.

#define NONE_TECHNICAL_ZERO                     // Displays the digit zero without a diagonal line.

#define AUTO_MODECHANGE_TIME 60                 // Time in seconds between switching from time to temperature and back.

#define EVENT_TIME 300                          // Time in seconds between showing events. Comment to turn events off.

#define ALARM_LED_COLOR RED                     // Color of the alarm LED. If not defined the display color will be used.
                                                // Available colors are:
                                                // WHITE, RED, RED_25, RED_50, ORANGE, YELLOW, YELLOW_25, YELLOW_50, GREENYELLOW,
                                                // GREEN, GREEN_25, GREEN_50, MINTGREEN, CYAN, CYAN_25, CYAN_50, LIGHTBLUE, BLUE,
                                                // BLUE_25, BLUE_50, VIOLET, MAGENTA, MAGENTA_25, MAGENTA_50, PINK.
//#define ABUSE_CORNER_LED_FOR_ALARM            // Use the upper right minute LED as alarm LED. Only works if ALARM_LED_COLOR is defined.

//#define POWERON_SELFTEST                      // Test LEDs at startup. Order is: white, red, green, blue.
#define SHOW_MODE_AMPM                          // Show AM/PM.
#define SHOW_MODE_SECONDS                       // Show seconds.
#define SHOW_MODE_WEEKDAY                       // Show weekday.
#define SHOW_MODE_DATE                          // Show date.
#define SHOW_MODE_MOONPHASE                     // Show moonphase.
//#define SHOW_MODE_SUNRISE_SUNSET              // Show sunrise and sunset times.
#define SHOW_MODE_TEST                          // Show tests.

#define WEATHER                                 // Show weather data.
#define LATITUDE "56.2345678"                   // Set the location for which you want the current weather data displayed
#define LONGITUDE "12.123456789"                // as latitude and longitude and the time zone.
#define TIMEZONE "Europe/Berlin"                // Check out https://open-meteo.com/ 

//*****************************************************************************
// Frontcover of the clock.
// This also sets the language of the menu and website (where available).
//*****************************************************************************

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

//*****************************************************************************
// Timezone - The timezone in which the clock is located.
// Important for the UTC offset and the summer/winter time (DST) change.
//*****************************************************************************

//#define TIMEZONE_IDLW                         // IDLW  International Date Line West UTC-12
//#define TIMEZONE_SST                          // SST   Samoa Standard Time UTC-11
//#define TIMEZONE_HST                          // HST   Hawaiian Standard Time UTC-10
//#define TIMEZONE_AKST                         // AKST  Alaska Standard Time UTC-9
//#define TIMEZONE_USPST                        // USPST Pacific Standard Time (USA) UTC-8
//#define TIMEZONE_USMST                        // USMST Mountain Standard Time (USA) UTC-7
//#define TIMEZONE_USAZ                         // USAZ  Mountain Standard Time (USA) UTC-7 (no DST)
//#define TIMEZONE_USCST                        // USCST Central Standard Time (USA) UTC-6
//#define TIMEZONE_USEST                        // USEST Eastern Standard Time (USA) UTC-5
//#define TIMEZONE_AST                          // AST   Atlantic Standard Time UTC-4
//#define TIMEZONE_BST                          // BST   Eastern Brazil Standard Time UTC-3
//#define TIMEZONE_VTZ                          // VTZ   Greenland Eastern Standard Time UTC-2
//#define TIMEZONE_AZOT                         // AZOT  Azores Time UTC-1
//#define TIMEZONE_GMT                          // GMT   Greenwich Mean Time UTC (no DST)
#define TIMEZONE_CET                            // CET   Central Europe Time UTC+1
//#define TIMEZONE_EST                          // EST   Eastern Europe Time UTC+2
//#define TIMEZONE_MSK                          // MSK   Moscow Time UTC+3 (no DST)
//#define TIMEZONE_GST                          // GST   Gulf Standard Time UTC+4
//#define TIMEZONE_PKT                          // PKT   Pakistan Time UTC+5
//#define TIMEZONE_IST                          // IST   Indian Standard Time UTC+5.5 (no DST)
//#define TIMEZONE_BDT                          // BDT   Bangladesh Time UTC+6
//#define TIMEZONE_JT                           // JT    Java Time UTC+7
//#define TIMEZONE_CNST                         // CNST  China Standard Time UTC+8
//#define TIMEZONE_HKT                          // HKT   Hong Kong Time UTC+8
//#define TIMEZONE_PYT                          // PYT   Pyongyang Time (North Korea) UTC+8.5
//#define TIMEZONE_CWT                          // CWT   Central West Time (Australia) UTC+8.75
//#define TIMEZONE_JST                          // JST   Japan Standard Time UTC+9
//#define TIMEZONE_ACST                         // ACST  Australian Central Standard Time UTC+9.5
//#define TIMEZONE_AEST                         // AEST  Australian Eastern Standard Time UTC+10
//#define TIMEZONE_LHST                         // LHST  Lord Howe Standard Time UTC+10.5
//#define TIMEZONE_SBT                          // SBT   Solomon Islands Time UTC+11
//#define TIMEZONE_NZST                         // NZST  New Zealand Standard Time UTC+12

//*****************************************************************************
// Hardware settings
//*****************************************************************************

#define SERIAL_SPEED 115200                     // Serial port speed for the console.
#define ESP_LED                                 // Flash ESP LED once a second.
#define NUMPIXELS 115                           // Number of LEDs in the strip.
#define MIN_BRIGHTNESS 20                       // Minimum brightness of LEDs from 0 to 255.
#define MAX_BRIGHTNESS 255                      // Maximum brightness of LEDs from 0 to 255.
#define TEST_BRIGHTNESS 80                      // Brightness of the LEDs while in testmode to not overload the powersupply.

//#define ONOFF_BUTTON                          // Use a hardware on/off-button.
//#define MODE_BUTTON                           // Use a hardware mode-button.
//#define TIME_BUTTON                           // Use a hardware time-button. Debug to serial will not work if defined.

//#define SENSOR_DHT22                          // Use a DHT22 sensor module (not the plain sensor) for room temperature and humidity.
#define DHT_TEMPERATURE_OFFSET 0.5              // Sets how many degrees the measured room temperature (+ or -) should be corrected.
#define DHT_HUMIDITY_OFFSET -2.0                // Sets how many degrees the measured room humidity (+ or -) should be corrected.

//#define RTC_BACKUP                            // Use an RTC as backup and room temperature.
#define RTC_TEMPERATURE_OFFSET -1.15            // Sets how many degrees the measured room temperature (+ or -) should be corrected.

//#define LDR                                   // Use an LDR for adaptive brightness control (ABC).
//#define LDR_IS_INVERSE                        // Inverses the value read from the LDR.

//#define BUZZER                                // Use a buzzer to make noise for alarms and timer. If not defined alarms are disabled.
#define BUZZTIME_ALARM_1 30                     // Maximum time in seconds for alarm 1 to be active if not turned off manually.
#define BUZZTIME_ALARM_2 30                     // Maximum time in seconds for alarm 2 to be active if not turned off manually.
#define BUZZTIME_TIMER 30                       // Maximum time in seconds for the timer alarm to be active if not turned off manually.

#define NEOPIXEL_RGB                            // Enable if your LEDs are RGB only.
//#define NEOPIXEL_RGBW                         // Enable if your LEDs have a distinct white channel (RGBW).

#define NEOPIXEL_TYPE NEO_GRB + NEO_KHZ800      // Specifies the NeoPixel driver. 400kHz, 800kHz, GRB, RGB, GRBW, RGBW...
//#define NEOPIXEL_TYPE NEO_WRGB + NEO_KHZ800   // Check out "Adafruit_NeoPixel.h" for more information.
//#define NEOPIXEL_TYPE NEO_GRBW + NEO_KHZ800

//*****************************************************************************
// IR remote control
// Press a button on the remote control in front of the clock.
// Then write down the code displayed in the serial console.
// DEBUG has to be defined to show you the code.
//*****************************************************************************

//#define IR_RECEIVER                           // Use an IR remote control.
//#define IR_CODE_ONOFF 16769565                // HX1838 Remote CH+
//#define IR_CODE_TIME  16753245                // HX1838 Remote CH-
//#define IR_CODE_MODE  16736925                // HX1838 Remote CH
#define IR_CODE_ONOFF 0xFFE01F                  // CLT2 V1.1 Remote Power
#define IR_CODE_TIME  0xFFA05F                  // CLT2 V1.1 Remote Time
#define IR_CODE_MODE  0xFF20DF                  // CLT2 V1.1 Remote Region

//#define IR_LETTER_OFF                         // Turns off the LED behind the IR sensor permanently.
#define IR_LETTER_X 8                           // This improves IR reception.
#define IR_LETTER_Y 10                          // X any Y coordinates

//*****************************************************************************
// LED layouts
//*****************************************************************************

#define LED_LAYOUT_HORIZONTAL_1                 // Horizontal, corner and alarm LEDs at the end of the strip. (As seen from the front.)

                                                // 111                    114                    112
                                                //    000 001 002 003 004 005 006 007 008 009 010
                                                //    021 020 019 018 017 016 015 014 013 012 011
                                                //    022 023 024 025 026 027 028 029 030 031 032
                                                //    043 042 041 040 039 038 037 036 035 034 033
                                                //    044 045 046 047 048 049 050 051 052 053 054
                                                //    065 064 063 062 061 060 059 058 057 056 055
                                                //    066 067 068 069 070 071 072 073 074 075 076
                                                //    087 086 085 084 083 082 081 080 079 078 077
                                                //    088 089 090 091 092 093 094 095 096 097 098
                                                //    109 108 107 106 105 104 103 102 101 100 099
                                                // 110                                           113

//#define LED_LAYOUT_VERTICAL_1                 // Vertical, corner and alarm LEDs (almost) within the strip. (As seen from the front.)

                                                // 000                    114                    102
                                                //    001 021 022 041 042 061 062 081 082 101 103
                                                //    002 020 023 040 043 060 063 080 083 100 104
                                                //    003 019 024 039 044 059 064 079 084 099 105
                                                //    004 018 025 038 045 058 065 078 085 098 106
                                                //    005 017 026 037 046 057 066 077 086 097 107
                                                //    006 016 027 036 047 056 067 076 087 096 108
                                                //    007 015 028 035 048 055 068 075 088 095 109
                                                //    008 014 029 034 049 054 069 074 089 094 110
                                                //    009 013 030 033 050 053 070 073 090 093 111
                                                //    010 012 031 032 051 052 071 072 091 092 112
                                                // 011                                           113

//#define LED_LAYOUT_VERTICAL_2                 // Vertical, corner and alarm LEDs at the end of the strip. (As seen from the front.)

                                                // 112                    111                    110
                                                //    009 010 029 030 049 050 069 070 089 090 109
                                                //    008 011 028 031 048 051 068 071 088 091 108
                                                //    007 012 027 032 047 052 067 072 087 092 107
                                                //    006 013 026 033 046 053 066 073 086 093 106
                                                //    005 014 025 034 045 054 065 074 085 094 105
                                                //    004 015 024 035 044 055 064 075 084 095 104
                                                //    003 016 023 036 043 056 063 076 083 096 103
                                                //    002 017 022 037 042 057 062 077 082 097 102
                                                //    001 018 021 038 041 058 061 078 081 098 101
                                                //    000 019 020 039 040 059 060 079 080 099 100
                                                // 113                                           114

//#define LED_LAYOUT_VERTICAL_3                 // Vertical, corner and alarm LEDs at the end of the strip. (As seen from the front.)

                                                // 111                    114                    110
                                                //    009 010 029 030 049 050 069 070 089 090 109
                                                //    008 011 028 031 048 051 068 071 088 091 108
                                                //    007 012 027 032 047 052 067 072 087 092 107
                                                //    006 013 026 033 046 053 066 073 086 093 106
                                                //    005 014 025 034 045 054 065 074 085 094 105
                                                //    004 015 024 035 044 055 064 075 084 095 104
                                                //    003 016 023 036 043 056 063 076 083 096 103
                                                //    002 017 022 037 042 057 062 077 082 097 102
                                                //    001 018 021 038 041 058 061 078 081 098 101
                                                //    000 019 020 039 040 059 060 079 080 099 100
                                                // 112                                           113

//*****************************************************************************
// Misc
//*****************************************************************************

//#define DEBUG                                 // Show debug infos in the serial console.
//#define DEBUG_WEB                             // Show debug infos on the web page.
//#define DEBUG_IR                              // Show debug infos from the IR remote.
//#define DEBUG_MATRIX                          // Renders the LED matrix to serial console. German only.
//#define DEBUG_FPS                             // Show number of loops per second in the serial console.

//#define SYSLOGSERVER_SERVER "192.168.0.1"     // Address and port of a syslogserver.
//#define SYSLOGSERVER_PORT 514                 // Log temperature and more...

// WeMos D1 R2 & mini (Espressif 8266)
#define PIN_IR_RECEIVER  16                     // D0 (no interrupt)
//#define PIN_WIRE_SCL     05                   // D1 SCL
//#define PIN_WIRE_SDA     04                   // D2 SDA
#define PIN_MODE_BUTTON  00                     // D3 LOW_Flash
#define PIN_LED          02                     // D4 ESP8266_LED
#define PIN_BUZZER       14                     // D5
#define PIN_DHT22        12                     // D6
#define PIN_LEDS_CLOCK   13                     // D7
#define PIN_LEDS_DATA    15                     // D8
#define PIN_LDR          A0                     // ADC
#define PIN_TIME_BUTTON  01                     // TXD0
#define PIN_ONOFF_BUTTON 03                     // RXD0
// GPIO 06 to GPIO 11 are
// used for flash memory databus

#endif