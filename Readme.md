# QLOCKWORK
## An advanced firmware for a DIY "word-clock".

Qlockwork is an ESP8266 (NodeMCU or WeMos D1 mini) firmware (under GPL license) for a so called "word-clock".
You can find the latest version at: [Qlockwork on GitHub](https://github.com/ch570512/Qlockwork)

The clock adjusts the time and date once every hour via NTP with a time server on the Internet.
If an RTC is installed, the time of the ESP is also set from the RTC via the SyncProvider.

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

**Warning:**
Do not power up the clock from USB only.
This would blow up the ESP or even the USB port because of the high power demand of the LED stripe.
Always (!) use an external 5V powersupply with at least 4A.

**Disclaimer:**
Qlockwork uses lots of third party libraries.
I cannot guarantee the integrity of these libraries.
You use the Qlockwork firmware at your own risk.

## Top features:
- Almost no electronics needed. Only an ESP8266 and an LED-stripe.
- Optional support for LDR, Buzzer, temperature and humidity sensor, IR-remote and buttons.
- Support for NeoPixel (RGB and RGBW) LED-stripes.
- Support for various horizontal and vertical LED layouts. 3 layouts included.
- Webpage to control and configure the clock via WiFi.
- Adaptive brightness control when using an LDR.
- 3 transitions for timechange.
- Indoor temperature from RTC or temperature and humidity from DHT sensor.
- Outdoor temperature and humidity from MeteoWeather.
- Visualisation of moonphase.
- Show sunrise and sunset times with animation.
- Textfeed for events and infos, local and over the web.
- Support for 16 frontcovers (Original and DIY) in 6 languages.
- 25 Colors.
- 99 minute timer.
- 2 Alarms with weekday selection.
- NTP timesync with timezone support.
- Automatic adjustment of daylight saving time.
- USB and Over-the-air firmware updates.

## Modes:
- Time
- AM/PM
- Seconds
- Weekday
- Date
- Sunrise (needs MeteoWeather)
- Sunset (needs MeteoWeather)
- Moonphase
- Room temperature (needs RTC or DHT22)
- Room humidity (needs DHT22)
- Outdoor temperature (needs MeteoWeather)
- Outdoor humidity (needs MeteoWeather)
- Timer
- LED-address-test
- All LED "red"
- All LED "green"
- All LED "blue"
- All LED "white"

## Setup:
Have a look at `Configuration.h` and `Events.h`.

## Needed libraries (tested):
```
esp8266 by ESP8266 Community 3.1.2
Adafruit NeoPixel by Adafruit Version 1.15.2
Adafruit Unified Sensor by Adafruit <info@adafruit.com> Version 1.1.15
ArduinoJson by Benoit Blanchon <http://blog.benoitblanchon.fr/> Version 7.4.2
ArduinoHttpClient by Arduino Version 0.6.1
DHT sensor library by Adafruit Version 1.4.6
DS3232RTC by Jack Christensen <jack.christensen@outlook.com> Version 3.1.2
IRremoteESP8266 by Sebastien Warin, Mark Szabo, Ken Shirriff, David Conran Version 2.8.6
Time by Michael Margolis Version 1.6.1
Timezone by Jack Christensen <jack.christensen@outlook.com> Version 1.2.6
WiFiManager by tzapu Version 2.0.17
```

## Compiler-Options: (recommended/tested)
```
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
```

## Operation manual
- Press "on/off" to switch the LEDs on and off.
- Press "Settings" to configure the clock via web-site.
- Press "Mode" to jump to the next page.
- Press "Time" to always jump back to the time page.

### Modes:
```
Time:                               The default mode. It shows the actual time. :)
Display AM/PM:                      Indicates whether it is AM or PM.
Seconds:                            Shows the seconds.
Weekday:                            Shows the weekday in local language.
Date:                               Shows day and month.
Sunrise:                            Time of sunrise.
Sunset:                             Time of sunset.
Moonphase:                          Shows the moonphase.
Room temperature:                   Display of the measured temperature in the room (only with RTC or DHT22).
Room humidity:                      Display of the measured humidity in the room (only with DHT22).
Outdoor temperature:                Display the temperature for your location from MeteoWeather.
Outdoor humidity:                   Display the humidity for your location from MeteoWeather.
Timer:                              Display of the remaining time if a timer is set.
LED-Test:                           Moves a horizontal bar across the display.
Red:                                Set all LEDs to red.
Green:                              Set all LEDs to green.
Blue:                               Set all LEDs to blue.
White:                              Set all LEDs to white.
```

### Settings on Webpage:
```
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
```

## Web-API:
```
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
                                    e.g.: http://192.168.1.10/setEvent?day=27&month=10&color=5&text=This%20is%20an%20event.

http://your_clocks_ip/showText?
buzzer=1                            Number of times the buzzer will beep before showing the text (optional)
color=0                             Color of the textfeed, 0 to 24 (optional)
text=text                           Set text of feed, max. 80 characters
                                    e.g.: http://192.168.1.10/showText?buzzer=2&color=1&text=Instant%20text%20on%20Qlockwork!

http://your_clocks_ip/control?
mode=0                              Set clock to mode=0 (time), mode=1 (am/pm), ...
                                    mode=17 (off, if all other modes are enabled) -- see modes.h and count.
                                    e.g.: http://192.168.1.10/control?mode=6

http://your_clocks_ip/reset         Restart the clock.
```

## Changelog:

#### 20260115:
After all these years, it's time to tidy up a bit.
Web Server can now be disabled.
Retired the updateinfo-server.
Retired Arduino-OTA in preperation of moving to PlatformIO.
Added timezone Indian Standard Time (IST).
Moved documentation to the files where they are needed. "Configuration.h" and "Events.h"

#### 20240908:
Switch to MeteoWeather API for weather and sunrise/sunset information.
Beatification of debug output and webcontrol page for sunrise/sunset.
Fixed transitions.

#### 20220830:
Fixed the issue that adaptive brightness control (ABC) can not be disabled in settings.

#### 20220429:
Moved the web-site from "not_available.com" to "not_available.com"
The UPDATE_INFOSERVER also moved there. Please update your Configuration.h

#### 20220411:
Fixed a bug causing the project not to compile in a certain configuration.
<Qlockwork.ino:1504:33: error: 'save_color_sunrise_sunset' was not declared in this scope>

#### 20220312:
Clocks brightness from LDR is now transitioning smoothly between values.
Sunrise and sunset now uses global timeout to switch back to time.
Fixed a bug causing the RTC not to work.
Added option to select RBG or RGBW for LedDriver.
Using transition "Move up" in menus. "Fade" was not working too well.

#### 20220311:
Fixed a bug preventing compilation using esp8266 by ESP8266 Community (3.0.2).
IDE change from VisualMicro to free Visual Studio Code.

#### 20220310:
Mode sunrise and sunset (Thanks to GenosseFlosse).

#### 20210422:
Reduced the watchdog resets (Thanks to espuno).

#### 20210321:
Fixed openweather bug for more than one weathercondition (Thanks to Manfred).
Setting RTC vom Web should be LT - not UTC (Thanks to Manfred).
Clear all LEDs before exiting test pattern (Thanks to espuno).
Calculate white channel for NEO_WRGB (Thanks to Manfred).
New LED_LAYOUT_VERTICAL_3 (Like vertical 2 but alarm LED = 114).

#### 20210224:
WLAN RSSI on WEB page in Debug.
// -30 dBm Ausgezeichnet Dies ist die maximal erreichbare und für jedes Einsatzszenario geeignete Signalstärke.
// -50 dBm Ausgezeichnet Dieser ausgezeichnete Signalpegel ist für alle Netzwerkanwendungen geeignet.
// -65 dBm Sehr gut Empfohlen für die Unterstützung von Smartphones und Tablets.
// -67 dBm Sehr gut Diese Signalstärke reicht für Voice-over-IP und Video-Streaming aus.
// -70 dBm Akzeptabel Diese Stufe ist die minimale Signalstärke um eine zuverlässige Paket-Zustellung zu gewährleisten.
// -80 dBm Schlecht Ermöglicht grundlegende Konnektivität, die Paket-Zustellung ist jedoch unzuverlässig.
// -90 dBm Sehr schlecht Meistens Rauschen, das die meisten Funktionen behindert.
//-100 dBm Am schlechtesten Nur Rauschen.

#### 20210218:
Outdoor pressure on WEB page.
Replaced WEB page title HOSTNAME with WEBSITE_TITLE in configuration (Thanks to GenosseFlosse).

#### 20200709:
Turn off timer when it is running.
Some localization in settings and weather.
Fixed alarm running 4 times longer than configured.
Set Hostname (Thanks to toto79).
Fixes to Buttons for esp8266 2.7.x (Thanks to toto79).
Fixes to WiFiManager for esp8266 2.7.x (Thanks to toto79).

#### 20200112:
WEB API for Clock On/Off.

#### 20200107:
Bugfixes.

#### 20200101:
Outdoor weather from OpenWeatherMap.
New LED driver with LED array mapping.
Reworked Configuration.h
Switched to lean Arduino_Json.h

#### 20191212:
Some housekeeping.

#### 20190520:
#define WIFI_BEEPS
Fixed rare flickering in fade transition.

#### 20190204:
Maior update of "Readme.txt"

#### 20190106:
Removed Yahoo weather.

#### 20190105:
Bugfix negative outdoorTemperature.
Bugfix outdoorCode not available.

#### 20181213:
No hourly beep if clock is off.
Some small improvements.

#### 20181101:
Settings set to defaults!
Hourly beep.
Set color of textfeed from URL.
Set color of events[0].text from URL.
Code cleanup and some minor bugfixes.

#### 20181027:
Show textfeed from URL.

#### 20181021:
Set events[0] from URL.
Name is back to Qlockwork (from Qlockwork2).

#### 20180930:
Use esp8266 by ESP8266 Community Version 2.4.2
Updated included libraries.
Removed legacy support for LPD8806RGBW.
Hardwareflags in debuginfo on webpage.
If NTP-request fails, try again every minute - don't wait for an hour.
More housekeeping.

#### 20180929:
Cleaned up libraries.

#### 20180130:
Transition "Move up"
Rewrote some code.
Removed FastLED.
FRONTCOVER_BINARY

#### 20180120:
Hardware mode-, on/off- and timebutton.
Moved PIN_IR_RECEIVER from D3 to D0.
Tooltips for webbuttons.
Added description of the Web-API in Readme.txt
Cleanup and bugfixes.

#### 20171127:
Bugfixes.

#### 20171125:
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

#### 20171111:
Cleanup and bugfixes.
New syslog format.

#### 20171019:
Changing automode.
Bugfix for Firefox.

#### 20171013:
Settings of alarms, colorchange, automode and show "It is" on Web-Page.
RTC is now on UTC.

#### 20171006:
Moonphase.
Color humidity indicator.
Faster Web-GUI.
Better offline behavior.
Reset via URL.

#### 20170929:
Adaptive Brightness Control (ABC).
New syslog logging.
Code cleanup and bugfixes.

#### 20170312:
Inital release.