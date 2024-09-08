//*****************************************************************************
// MeteoWeather.h - Get weather data and sunrise/sunset times from Open-Meteo
//*****************************************************************************

#ifndef METEOWEATHER_H
#define METEOWEATHER_H

// #include <Arduino_JSON.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

class MeteoWeather {
public:
    MeteoWeather();
    ~MeteoWeather();

    String description;
    double temperature;
    uint8_t humidity;
    uint16_t pressure;
    time_t sunrise;
    time_t sunset;

    uint8_t getOutdoorConditions(String lat, String lon, String timezone);

private:

};

#endif
