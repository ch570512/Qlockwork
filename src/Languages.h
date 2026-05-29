#pragma once

#if defined(FRONTCOVER_EN) || defined(FRONTCOVER_BINARY)
const char sWeekday[][2] = {
    { 'S', 'U' },
    { 'M', 'O' },
    { 'T', 'U' },
    { 'W', 'E' },
    { 'T', 'H' },
    { 'F', 'R' },
    { 'S', 'A' }
};
#define LANGSTR "en"
#define TXT_SETTINGS "Settings"
#define TXT_ALARM "Alarm"
#define TXT_ON "on"
#define TXT_OFF "off"
#define TXT_HOURBEEP "Hourly beep"
#define TXT_TIMER "Timer"
#define TXT_MINUTES "minutes"
#endif

#if defined(FRONTCOVER_DE_DE) || defined(FRONTCOVER_DE_SW) || defined(FRONTCOVER_DE_BA) || defined(FRONTCOVER_DE_SA) || defined(FRONTCOVER_D3) || defined(FRONTCOVER_DE_MKF_DE) || defined(FRONTCOVER_DE_MKF_SW) || defined(FRONTCOVER_DE_MKF_BA) || defined(FRONTCOVER_DE_MKF_SA) || defined(FRONTCOVER_CH) || defined(FRONTCOVER_CH_GS)
const char sWeekday[][2] = {
    { 'S', 'O' },
    { 'M', 'O' },
    { 'D', 'I' },
    { 'M', 'I' },
    { 'D', 'O' },
    { 'F', 'R' },
    { 'S', 'A' }
};
#define LANGSTR "de"
#define TXT_SETTINGS "Einstellungen"
#define TXT_ALARM "Wecker"
#define TXT_ON "ein"
#define TXT_OFF "aus"
#define TXT_HOURBEEP "Stundenalarm"
#define TXT_TIMER "Timer"
#define TXT_MINUTES "Minuten"
#endif

#if defined(FRONTCOVER_FR)
const char sWeekday[][2] = {
    { 'D', 'I' },
    { 'L', 'U' },
    { 'M', 'A' },
    { 'M', 'E' },
    { 'J', 'E' },
    { 'V', 'E' },
    { 'S', 'A' }
};
#define LANGSTR "fr"
#define TXT_SETTINGS "Param�tres"
#define TXT_ALARM "R�veil"
#define TXT_ON "on"
#define TXT_OFF "off"
#define TXT_HOURBEEP "Alarme heure"
#define TXT_TIMER "Minuteur"
#define TXT_MINUTES "Minutes"
#endif

#if defined(FRONTCOVER_IT)
const char sWeekday[][2] = {
    { 'D', 'O' },
    { 'L', 'U' },
    { 'M', 'A' },
    { 'M', 'E' },
    { 'G', 'I' },
    { 'V', 'E' },
    { 'S', 'A' }
};
#define LANGSTR "it"
#define TXT_SETTINGS "Settings"
#define TXT_ALARM "Alarm"
#define TXT_ON "on"
#define TXT_OFF "off"
#define TXT_HOURBEEP "Hourly beep"
#define TXT_TIMER "Timer"
#define TXT_MINUTES "minutes"
#endif

#if defined(FRONTCOVER_ES)
const char sWeekday[][2] = {
    { 'D', 'O' },
    { 'L', 'U' },
    { 'M', 'A' },
    { 'M', 'I' },
    { 'J', 'U' },
    { 'V', 'I' },
    { 'S', 'A' }
};
#define LANGSTR "es"
#define TXT_SETTINGS "Settings"
#define TXT_ALARM "Alarm"
#define TXT_ON "on"
#define TXT_OFF "off"
#define TXT_HOURBEEP "Hourly beep"
#define TXT_TIMER "Timer"
#define TXT_MINUTES "minutes"
#endif

#if defined(FRONTCOVER_NL)
const char sWeekday[][2] = {
    { 'Z', 'O' },
    { 'M', 'A' },
    { 'D', 'I' },
    { 'W', 'O' },
    { 'D', 'O' },
    { 'V', 'R' },
    { 'Z', 'A' }
};
#define LANGSTR "nl"
#define TXT_SETTINGS "Settings"
#define TXT_ALARM "Alarm"
#define TXT_ON "on"
#define TXT_OFF "off"
#define TXT_HOURBEEP "Hourly beep"
#define TXT_TIMER "Timer"
#define TXT_MINUTES "minutes"
#endif