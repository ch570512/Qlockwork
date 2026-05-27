#pragma once

#include <Arduino.h>
#include <time.h>
#include <sys/time.h>
#include "Debug.h"

struct tm getTime();
uint8_t getHour(time_t zeit);
uint8_t getMinute(time_t zeit);
int getMoonphase(int y, int m, int d);
String padStringZeros(String input);
void handleTimeSetting(String input);