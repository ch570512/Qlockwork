/******************************************************************************
Events.h

Display a textfeed on a particular day of the year.

The format of an entry in the array is:
{ month, day, "Text to display.", year, color },
The last entry in the array has no comma at the end.

Year will be used to calculate an age. "present year" - year = age.
'0' will disable the calculation. There can only be one event a day.
The possible colors are:
WHITE, RED, RED_25, RED_50, ORANGE, YELLOW, YELLOW_25, YELLOW_50, GREENYELLOW,
GREEN, GREEN_25, GREEN_50, MINTGREEN, CYAN, CYAN_25, CYAN_50, LIGHTBLUE, BLUE,
BLUE_25, BLUE_50, VIOLET, MAGENTA, MAGENTA_25, MAGENTA_50, PINK.
******************************************************************************/

#ifndef EVENTS_H
#define EVENTS_H

#include "Colors.h"

struct event_t {
    uint8_t month;
    uint8_t day;
    String text;
    uint16_t year;
    eColor color;
};

event_t events[] = {
    { 0, 0, "", 0, WHITE }, // Do not remove
    { 1,  1, "Happy New Year!", 0, YELLOW_25 },
    { 3, 14, "Albert Einsteins birthday!", 1879, MAGENTA },
    { 12, 24, "Merry Christmas!", 0, RED }
};

#endif