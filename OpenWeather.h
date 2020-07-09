//******************************************************************************
// OpenWeather.h - Get weather data from OpenWeather
//******************************************************************************

#ifndef OPENWEATHER_H
#define OPENWEATHER_H

#include <Arduino_JSON.h>
#include <ESP8266WiFi.h>

class OpenWeather
{
public:
    OpenWeather();
    ~OpenWeather();

    String description;
    double temperature;
    uint8_t humidity;
    uint16_t pressure;

    uint8_t getOutdoorConditions(String location, String apiKey, String langStr);

private:

};

#endif
