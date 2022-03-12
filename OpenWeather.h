//*****************************************************************************
// OpenWeather.h - Get weather data and sunrise/sunset times from OpenWeather
//*****************************************************************************

#ifndef OPENWEATHER_H
#define OPENWEATHER_H

#include <Arduino_JSON.h>
#include <ESP8266WiFi.h>

class OpenWeather {
public:
    OpenWeather();
    ~OpenWeather();

    String description;
    double temperature;
    uint8_t humidity;
    uint16_t pressure;
    time_t sunrise;
    time_t sunset;

    uint8_t getOutdoorConditions(String location, String apiKey, String langStr);

private:

};

#endif
