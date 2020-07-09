******************************************************************************
QLOCKWORK
An advanced firmware for a DIY "word-clock".
******************************************************************************

Qlockwork is an ESP8266 (NodeMCU or WeMos D1 mini) firmware (under GPL license) for a so called "word-clock".

The clock adjusts the time and date once every hour via NTP with a time server on the Internet.
If an RTC is installed, the time of the ESP is also set from the RTC every minute.

At startup the clock performs a short self test.
The sequence of the colors should be: red, green, blue and white. If not, your LED driver setup is wrong.
The clock also shows the local IP address it received via DHCP.
Use this address in a browser to access the clocks web site to set it up.

WiFi manager: If the clock can not connect to any WLAN at startup, it turns on an access point.
Connect a mobile phone or tablet to the AP and enter the WLAN credentials. A white "WiFi" is shown on the clock.
On success there are three short beeps and "WiFi" will be green.
If no WLAN is connected or the timeout has expired, there is a long beep and "WiFi" turns red.
After the WLAN timeout the clock works without NTP but you can still control it via its AP.
Without WLAN the clock uses the optional RTC to set the time or if no RTC is present has to be set manually.
In either ways the clocks IP is shown as a textfeed.

Events can be shown every five minutes on a particular day of the year as a textfeed.
You can set them in "Events.h". Expand the array with events as shown in the default values.
You can set a color for every event. Do not change the first entry.
There is no comma behind the last entry.

Updates of the firmware could be uploaded via USB, OTA or the clocks webserver.
You will find more help and information on how to configure and compile the firmware in "Readme.md" in the zip-archive.
You will also find a circuit diagram, a partslist and some pictures in the "/misc" directory.
All sensors, the RTC and the buzzer are optional.
The clock will run with the ESP8266 module only. No PCB needed.

Warning:    Do not power up the clock from USB only.
            This would blow up the ESP or even the USB port because of high power demand of the LED stripe.
            Always use an external 5V powersupply with at least 4A.

Disclaimer: Qlockwork uses lots of third party libraries.
            I can not guarantee the integrity of these libraries.
            You use the Qlockwork firmware at your own risk.

You can find the QLOCKWORK web-site here:
http://tmw-it.ch/qlockwork/

You can download the latest version of the firmware here:
https://github.com/ch570512/Qlockwork

Visit this forum for comments, suggestions and bug reports:
http://diskussion.christians-bastel-laden.de/viewtopic.php?f=23&t=2843

******************************************************************************
Top features:
******************************************************************************

Almost no electronics needed. Only an ESP32 and an LED-stripe.
Optional support for LDR, Buzzer, temperature and humidity sensor, IR-remote and buttons.
Support for NeoPixel (RGB and RGBW) LED-stripes.
Support for various horizontal and vertical LED layouts. 3 layouts included.
Webpage to control and configure the clock via WiFi.
Adaptive brightness control when using an LDR.
3 transitions for timechange.
Indoor temperature from RTC or temperature and humidity from DHT sensor.
Outdoor temperature and humidity from OpenWeather. You need an APIKey from OpenWeather to use this feature.
Visualisation of moonphase.
Textfeed for events and infos, local and over the web.
Support for 16 frontcovers (Original and DIY) in 6 languages.
25 Colors.
99 minute timer.
2 Alarmtimes for every day of the week.
NTP timesync with timezone support.
Automatic adjustment of daylight saving time.
USB and Over-the-air firmware updates.

******************************************************************************
Pages:
******************************************************************************

Time
AM/PM
Seconds
Weekday
Date
Moonphase
Room temperature (RTX or DHT22)
Room humidity (DHT22)
Outdoor temperature (OpenWeather)
Outdoor humidity (OpenWeather)
Timer
LED-test
Red
Green
Blue
White

******************************************************************************
Needed libraries: (recommended/tested versions in brackets)
******************************************************************************

Arduino IDE for Windows (1.8.12)
esp8266 by ESP8266 Community (2.7.2)
Arduino_JSON by Arduino (0.1.0)
Adafruit NeoPixel by Adafruit (1.5.0)
Adafruit Unified Sensor by Adafruit (1.1.4)
ArduinoHttpClient by Arduino (0.4.0)
ArduinoOTA by Juraj Andressy (1.0.4)
DHT sensor library by Adafruit (1.3.10)
DS3232RTC by Jack Christensen (1.2.12)
IRremoteESP8266 by Sebastien Warin (2.7.8)
Time by Michael Margolis (1.6.0)

Included in source is the Timezone library from Jack Christensen
and WiFiManager by AlexT.

******************************************************************************
Compiler-Options: (recommended/tested)
******************************************************************************

Board: "LOLIN(WEMOS) D1 R2 & mini"
CPU Frequency: "80 MHz"
Flash Size: "4M (3M SPIFFS)"
Debug port: "Disabled"
Debug Level: "None"
IwIP Variant: "v2 Lower Memory"
VTables: "Flash"
Exceptions: "Disabled"
Erase Flash: "Only Sketch"
SSL Support: "All SSL ciphers"

******************************************************************************
Misc:
******************************************************************************

For OTA and web-server updates check out:
http://esp8266.github.io/Arduino/versions/2.3.0/doc/ota_updates/readme.html
Don't forget to install Python 2.7 and to select "Add python.exe to path" while installing.

Call "http://your_clocks_ip/update" to upload a new firmware via webbrowser.
Call "http://your_clocks_ip/reset" to restart the ESP.

******************************************************************************
Operation manual:
******************************************************************************

Press "on/off" to switch the LEDs on and off.
Press "Settings" to configure the clock via web-site.
Press "Mode" to jump to the next page.
Press "Time" to always jump back to the time page.

*** Modes: *******************************************************************

Time:                               The default mode of the clock. Shows the actual time. :)
Display AM/PM:                      Indicates whether it is AM or PM.
Seconds:                            Shows the seconds.
Weekday:                            Shows the weekday in local language.
Date:                               Shows day and month.
Moonphase:                          Shows the moonphase.
Room temperature:                   Display of the measured temperature in the room (only with RTC or DHT22).
Room humidity:                      Display of the measured humidity in the room (only with DHT22).
Outdoor temperature:                Display the temperature for your location from OpenWeather.
Outdoor humidity:                   Display the humidity for your location from OpenWeather.
Timer:                              Display of the remaining time if a timer is set.
LED-Test:                           Moves a horizontal bar across the display.
Red:                                Set all LEDs to red.
Green:                              Set all LEDs to green.
Blue:                               Set all LEDs to blue.
White:                              Set all LEDs to white.

*** Settings: ****************************************************************

Alarm 1:                            Enable (on) or disable (off) alarm 1.
                                    Time for alarm 1.
                                    Weekdays on which alarm 1 is active.
Alarm 2:                            Enable (on) or disable (off) alarm 2.
                                    Time for alarm 2.
                                    Weekdays on which alarm 2 is active.
Hourly beep:                        Short beep every full hour.
Timer:                              Sets the minute timer. Set to zero to disable a running timer.
Show temperature:                   Enable (on) or disable (off) showing the temperature in time view.
ABC:                                Enable (on) or disable (off) adaptive brightness control.
                                    Brightness will adjust itself in the range of MIN_BRIGHTNESS and brightness.
Brightness:                         Brightness of the LEDs in percent. The range is MIN_BRIGHTNESS to MAX_BRIGHTNESS.
                                    If ABC is enabled this is the maximum achievable brightness.
Color:                              Choose one of 25 colors for the LEDs.
Colorchange:                        Change the color in intervals.
                                    Do not change (off), every 5 minutes (five), every hour (hour), every day (day).
Transition:                         Choose between fast, move or fade mode transition.
Timeout:                            Time in seconds to change mode back to time. (0: disabled)
Night off:                          Set the time the clocks turns itself off at night.
Day on:                             Set the time the clocks turns itself on at day.
Show "It is":                       Enable (on) or disable (off) "It is". It will be shown every half and full hour anyway.
Set date/time:                      Date and time of the clock. The seconds are set to zero if you press save.

******************************************************************************
Configuration.h - Software settings:
******************************************************************************

#define HOSTNAME                    The name of the clock.
#define WIFI_SETUP_TIMEOUT          Time in seconds set up the WiFiManager or search for a WLAN.
                                    If no WLAN is connected the clock enters AP mode.
                                    You can control the clock if you connect your phone or tablet to this accesspoint.
                                    On Android you have to tell the phone that it's ok to have no internet.
#define WIFI_AP_PASS                The password for the AP. At least 8 characters. Default is "12345678".
#define OTA_PASS                    Password for "Over the Air" updates. Default is "1234".
#define NTP_SERVER                  NTP server to be queried.
#define NTP_TIMEOUT                 Milliseconds to wait for NTP server to answer.
#define WIFI_BEEPS                  Beep 3 times if WIFI is conneced, if not, beep one time on startup.
#define SHOW_IP                     Show local IP at startup. Use this in the browser to access the clocks menue.
#define NONE_TECHNICAL_ZERO         Displays the zero without the diagonal line.
#define AUTO_MODECHANGE_TIME        Time in seconds to wait between switching from time to temperature.
#define FEED_SPEED                  Feed delay in milliseconds. 120 is a good start.
#define EVENT_TIME                  Time in seconds to wait between showing events. Comment to turn off events.
#define ALARM_LED_COLOR             Color of the alarm LED. If not defined the display color will be used.
                                    The possible colors are:
                                    WHITE, RED, RED_25, RED_50, ORANGE, YELLOW, YELLOW_25, YELLOW_50, GREENYELLOW,
                                    GREEN, GREEN_25, GREEN_50, MINTGREEN, CYAN, CYAN_25, CYAN_50, LIGHTBLUE, BLUE,
                                    BLUE_25, BLUE_50, VIOLET, MAGENTA, MAGENTA_25, MAGENTA_50, PINK.
#define ABUSE_CORNER_LED_FOR_ALARM  Use the upper right minute LED as alarm LED. Only works if ALARM_LED_COLOR is defined.
                                    If no alarm or timer is set the LED is used as expected.
#define DEDICATION                  Show a text on the clocks webpage.
#define POWERON_SELFTEST            Test LEDs at startup. Colors are: white, red, green, blue. In this order.
#define SHOW_MODE_AMPM
#define SHOW_MODE_SECONDS
#define SHOW_MODE_WEEKDAY
#define SHOW_MODE_DATE
#define SHOW_MODE_MOONPHASE
#define SHOW_MODE_TEST
#define APIKEY                      Your OpenWeather API key.
#define LOCATION                    Your location for OpenWeather.
#define TIMEZONE_*                  The time zone in which the clock is located. Important for the UTC offset and the
                                    summer/winter time change.
#define FRONTCOVER_*                Frontcover of the clock. This also sets the language of the menu and the website.

******************************************************************************
Configuration.h - Hardware settings:
******************************************************************************

#define ESP_LED                     Displays the function using the LED on the ESP. It flashes once a second.

#define ONOFF_BUTTON               Use a hardware on/off-button.
#define MODE_BUTTON                Use a hardware mode-button.
#define TIME_BUTTON                Use a hardware time-button. Debug to serial will not work if defined.

#define SENSOR_DHT22                Use a DHT22 sensor module (not the plain sensor) for room temperature and humidity.
#define DHT_TEMPERATURE_OFFSET      Sets how many degrees the measured room temperature (+ or -) should be corrected.
#define DHT_HUMIDITY_OFFSET         Sets how many degrees the measured room humidity (+ or -) should be corrected.

#define RTC_BACKUP                  Use an RTC as backup and room temperature.
#define RTC_TEMPERATURE_OFFSET      Sets how many degrees the measured room temperature (+ or -) should be corrected.

#define LDR                         Use an LDR for adaptive brightness control (ABC).
#define LDR_IS_INVERSE              Combined with #define LDR inverses the value of the LDR.
#define LDR_HYSTERESIS              Brightness control from a deviation in the range from 0 to 1023. Default: 40.
                                    If your display is flickering increase this value.
#define MIN_BRIGHTNESS              Minimum brightness of LEDs ranging from 0 to 255. Default: 20.
#define MAX_BRIGHTNESS              Maximum brightness of LEDs ranging from 0 to 255. Default 255.
                                    Your powersupply has to support this brightness.
#define BRIGHTNESS_SELFTEST         Brightness of the LEDs while in testmode to not overload the powersupply.

#define BUZZER                      Use a buzzer to make noise for alarmtime and timer.
                                    If not defined all alarmfunctions are disabled.
#define BUZZTIME_ALARM_1            Maximum time in seconds for alarm 1 to be active when not turned off manually.
#define BUZZTIME_ALARM_2            Maximum time in seconds for alarm 2 to be active when not turned off manually.
#define BUZZTIME_TIMER              Maximum time in seconds for the timer alarm to be active when not turned off manually.

#define IR_REMOTE                   Use an IR remote control.
#define IR_LETTER_OFF               Turns off the LED behind the IR sensor permanently. This improves IR reception.
#define IR_CODE_*                   Any remote control can be used. 6 keys are supported.
                                    Press a button on the remote control in front of the clock.
                                    Then write the code displayed in the serial console to the file "Configuration.h".
                                    If you see more than one try the code which is changing from button to button.
                                    DEBUG has to be defined to show you the code.

#define NEOPIXEL_TYPE               Specifies the NeoPixel driver. 400kHz, 800kHz, GRB, RGB, GRBW and RGBW.

#define LED_LAYOUT_HORIZONTAL_1     Horizontal and corner LEDs at the end of the strip. (As seen from the front.)

111                    114                    112
   000 001 002 003 004 005 006 007 008 009 010
   021 020 019 018 017 016 015 014 013 012 011
   022 023 024 025 026 027 028 029 030 031 032
   043 042 041 040 039 038 037 036 035 034 033
   044 045 046 047 048 049 050 051 052 053 054
   065 064 063 062 061 060 059 058 057 056 055
   066 067 068 069 070 071 072 073 074 075 076
   087 086 085 084 083 082 081 080 079 078 077
   088 089 090 091 092 093 094 095 096 097 098
   109 108 107 106 105 104 103 102 101 100 099
110                                            113

#define LED_LAYOUT_VERTICAL_1       Vertical and corner LEDs within the strip. (As seen from the front.)

000                    114                    102
   001 021 022 041 042 061 062 081 082 101 103
   002 020 023 040 043 060 063 080 083 100 104
   003 019 024 039 044 059 064 079 084 099 105
   004 018 025 038 045 058 065 078 085 098 106
   005 017 026 037 046 057 066 077 086 097 107
   006 016 027 036 047 056 067 076 087 096 108
   007 015 028 035 048 055 068 075 088 095 109
   008 014 029 034 049 054 069 074 089 094 110
   009 013 030 033 050 053 070 073 090 093 111
   010 012 031 032 051 052 071 072 091 092 112
011                                           113

#define LED_LAYOUT_VERTICAL_2       Vertical and corner LEDs at the end of the strip. (As seen from the front.)

112                    111                    110
   009 010 029 030 049 050 069 070 089 090 109
   008 011 028 031 048 051 068 071 088 091 108
   007 012 027 032 047 052 067 072 087 092 107
   006 013 026 033 046 053 066 073 086 093 106
   005 014 025 034 045 054 065 074 085 094 105
   004 015 024 035 044 055 064 075 084 095 104
   003 016 023 036 043 056 063 076 083 096 103
   002 017 022 037 042 057 062 077 082 097 102
   001 018 021 038 041 058 061 078 081 098 101
   000 019 020 039 040 059 060 079 080 099 100
113                                           114

******************************************************************************
Configuration.h - Misc:
******************************************************************************

#define DEBUG                       Show debug infos in the serial console
#define DEBUG_WEB                   Show debug infos on the web page
#define DEBUG_MATRIX                Renders the output of the matrix for the German front in the serial console
#define DEBUG_FPS                   Show number of loops per second in the serial console

#define SYSLOGSERVER                Turn logging to a syslogserver on/off
#define SYSLOGSERVER_SERVER         Address of the syslogserver
#define SYSLOGSERVER_PORT           Port of the syslogserver

#define UPDATE_INFO_*               The update info periodically anonymously checks if there is a firmwareupdate
                                    available. No user data is send to the host. Comment if you do not want this info
#define UPDATE_INFOSERVER           Address of the updateinfo server
#define UPDATE_INFOFILE             Path and name of the updateinfo file

#define SERIAL_SPEED                Serial port speed for the console

******************************************************************************
Events.h
******************************************************************************

event_t events[]                    Display a textfeed on a particular day of the year.
                                    The format of an entry in the array is:
                                    { month, day, "Text to display.", year, color },
                                    The last entry has no comma at the end.
                                    Year will be used to calculate an age. "present year" - year = age.
                                    '0' will not show an age.
                                    There can only be one event a day.
                                    The possible colors are:
                                    WHITE, RED, RED_25, RED_50, ORANGE, YELLOW, YELLOW_25, YELLOW_50, GREENYELLOW,
                                    GREEN, GREEN_25, GREEN_50, MINTGREEN, CYAN, CYAN_25, CYAN_50, LIGHTBLUE, BLUE,
                                    BLUE_25, BLUE_50, VIOLET, MAGENTA, MAGENTA_25, MAGENTA_50, PINK.
                                    Do not change the first entry.

******************************************************************************
Web-API:
******************************************************************************

http://your_clocks_ip/commitSettings?
a1=0                                Alarm 1 on [1] or off [0]
a1t=hh:mm                           Alarm 1 hour [hh] and minute [mm]
a1w1=2                              Set Sunday
a1w2=4                              Set Monday
a1w3=8                              Set Tuesday
a1w4=16                             Set Wednesday
a1w5=32                             Set Thursday
a1w6=64                             Set Friday
a1w7=128                            Set Saturday
a2=0                                Alarm 2 on [1] or off [0]
a2t=hh:mm                           Alarm 2 hour [hh] and minute [mm]
a2w1=2                              Set Sunday
a2w2=4                              Set Monday
a2w3=8                              Set Tuesday
a2w4=16                             Set Wednesday
a2w5=32                             Set Thursday
a2w6=64                             Set Friday
a2w7=128                            Set Saturday
hb=0                                Hourly beep on [1] or off [0]
ti=0                                Timer in minutes
mc=0                                Modechange on [1] or off [0]
ab=1                                ABC on [1] or off [0]
br=50                               Brightness in percent
co=14                               Number of the LEDs color. See Colors.h
cc=0                                Number of colorchange. See Colors.h
tr=1                                Number of transition. See Modes.h
to=15                               Timeout in seconds
no=hh:mm                            Night off hour [hh] and minute [mm]
do=hh:mm                            Day on hour [hh] and minute [mm]
ii=1                                "It is" on [1] or off [0]
st=YYYY-MM-DDThh:mm                 Set time and date

http://your_clocks_ip/setEvent?
day=dd                              Set day of event
month=mm                            Set month of event
color=0                             Color of the eventtext, 0 to 24 (optional)
text=text                           Set text of event, max. 40 characters
                                    e.g.: http://your_clocks_ip/setEvent?day=27&month=10&color=5&text=This%20is%20an%20event.

http://your_clocks_ip/showText?
buzzer=1                            Number of times the buzzer will beep before showing the text (optional)
color=0                             Color of the textfeed, 0 to 24 (optional)
text=text                           Set text of feed, max. 80 characters
                                    e.g.: http://your_clocks_ip/showText?buzzer=2&color=1&text=Instant%20text%20on%20Qlockwork!

http://your_clocks_ip/control?
mode=0                              Set clock to mode=0 (time), mode=1 (am/pm), ...
                                    mode=17 (off, if all other modes are enabled) -- see modes.h and count.
                                    e.g.: http://your_clocks_ip/control?mode=6

******************************************************************************
Changelog:
******************************************************************************

20200709:
Turn off timer when it is running.
Some localization in settings and weather.
Fixed alarm running 4 times longer than configured.
Set Hostname (Thanks to toto79).
Fixes to Buttons for esp8266 2.7.x (Thanks to toto79).
Fixes to WiFiManager for esp8266 2.7.x (Thanks to toto79).

20200112:
WEB API for Clock On/Off.

20200107:
Bugfixes.

20200101:
Outdoor Weather from OpenWeatherMap.
New LED driver with LED array mapping.
Reworked Configuration.h
Switched to lean Arduino_Json.h

20191212:
Some housekeeping.

20190520:
#define WIFI_BEEPS
Fixed rare flickering of fade transition.

20190204:
Maior update of "Readme.txt"

20190106:
Removed Yahoo weather.

20190105:
Bugfix negative outdoorTemperature.
Bugfix outdoorCode not available.

20181213:
No hourly beep if clock is off.
Some small improvements.

20181101:
Settings set to defaults!
Hourly beep.
Set color of textfeed from URL.
Set color of events[0].text from URL.
Code cleanup and some minor bugfixes.

20181027:
Show textfeed from URL.

20181021:
Set events[0] from URL.
Name is back to Qlockwork (from Qlockwork2).

20180930:
Use esp8266 by ESP8266 Community Version 2.4.2
Updated included libraries.
Removed legacy support for LPD8806RGBW.
Hardwareflags in debuginfo on webpage.
If NTP-request fails, try again every minute - don't wait for an hour.
More housekeeping.

20180929:
Cleaned up libraries.

20180130:
Transition "Move up"
Rewrote some code.
Removed FastLED.
FRONTCOVER_BINARY

20180120:
Hardware mode-, on/off- and timebutton.
Moved PIN_IR_RECEIVER from D3 to D0.
Tooltips for webbuttons.
Added description of the Web-API in Readme.txt
Cleanup and bugfixes.

20171127:
Bugfixes.

20171125:
Set weekdays for alarms on web-page.
Set alarm to "on" if alarmtime is changed.
Set time, timeout, "Night off", "Day on" on web-page.
Removed settings from clock-menu which are present on web-page.
AP mode if no WLAN is connected.
"#define DEBUG" and "#define DEBUG_WEB" are back.
Switched from RestClient.h to ArduinoHttpClient.h
Moved setting of frontcover to configuration.h "#define FRONTCOVER_*"
Removed "#define LANGUAGE_*". Now set from "#define FRONTCOVER_*"
Removed experimantal DUAL-support.
Settings set to defaults.

20171111 (stable):
Cleanup and bugfixes.
New syslog format.

20171019:
Changing automode.
Bugfix for Firefox.

20171013:
Settings of alarms, colorchange, automode and show "It is" on Web-Page.
RTC is now on UTC.

20171006:
Moonphase.
Color humidity indicator.
Faster Web-GUI.
Better offline behavior.
Reset via URL.

20170929:
Adaptive Brightness Control (ABC).
New syslog logging.
Code cleanup and bugfixes.

20170312:
Inital release.
