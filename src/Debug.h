#pragma once

#include <Arduino.h>
#include "Configuration.h"

#ifdef DEBUG
#define DEBUG_SERIAL_PRINTLN(x) \
    Serial.print("[");          \
    Serial.print(__FILE__);     \
    Serial.print(":");          \
    Serial.print(__LINE__);     \
    Serial.print("] ");         \
    Serial.println(x)
#else
#define DEBUG_SERIAL_PRINTLN(x)
#endif

void debugScreenBuffer(uint16_t screenBuffer[]);
