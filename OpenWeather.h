//******************************************************************************
// OpenWeather.h - Get weather data from OpenWeather
//******************************************************************************

#pragma once

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

	uint8_t getOutdoorConditions(String location, String apiKey);

private:

};
