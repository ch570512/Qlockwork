//******************************************************************************
// LedDriver.h
//******************************************************************************

#ifndef LEDDRIVER_H
#define LEDDRIVER_H

#include <Adafruit_NeoPixel.h>
#include "Configuration.h"
#include "Colors.h"

class LedDriver
{
public:
    LedDriver();
    ~LedDriver();

    void clear();
    void show();
    void setPixel(uint8_t x, uint8_t y, uint8_t color, uint8_t brightness);
    void setPixel(uint8_t num, uint8_t color, uint8_t brightness);

private:
    Adafruit_NeoPixel *strip;

};

#endif
