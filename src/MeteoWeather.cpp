//*****************************************************************************
// MeteoWeather.cpp - Get weather data from Open-Meteo
//*****************************************************************************

#include "MeteoWeather.h"

MeteoWeather::MeteoWeather()
{
}

MeteoWeather::~MeteoWeather()
{
}

uint8_t MeteoWeather::getOutdoorConditions(String lat, String lon, String timezone)
{
    String response;
    WiFiClient client;
    JsonDocument doc;
    timezone.replace("/", "%2F");
    if (client.connect(F("api.open-meteo.com"), 80))
    {
        String url = "/v1/forecast?latitude=" + String(lat) + "&longitude=" + String(lon) + "&current=temperature_2m,relative_humidity_2m,surface_pressure&daily=sunrise,sunset&timeformat=unixtime&timezone=" + timezone + "&forecast_days=1";
        DEBUG_SERIAL_PRINTLN(F("Weather API GET:"));
        DEBUG_SERIAL_PRINTLN(url);
        client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: api.open-meteo.com" + "\r\n" + "Connection: close\r\n\r\n");
        unsigned long startMillis = millis();
        while (client.available() == 0)
        {
            if (millis() - startMillis > 5000)
            {
                client.stop();
                return 0;
            }
        }

        while (client.available())
            response = client.readString();

        DEBUG_SERIAL_PRINTLN(F("Weather API response:"));
        DEBUG_SERIAL_PRINTLN(response);

        response.trim();
        int first = response.indexOf("{"); // Extract json file content of message.
        String extract = response.substring(first);

        DeserializationError error = deserializeJson(doc, extract);

        if (error)
        {
            DEBUG_SERIAL_PRINTLN(F("deserializeJson() failed: "));
            DEBUG_SERIAL_PRINTLN(error.c_str());
            return 0;
        }

        JsonObject current = doc["current"];
        float current_temperature_2m = current["temperature_2m"];           // 27.1
        int current_relative_humidity_2m = current["relative_humidity_2m"]; // 53
        float current_surface_pressure = current["surface_pressure"];       // 1010.1

        JsonObject daily = doc["daily"];
        long daily_sunrise_0 = daily["sunrise"][0]; // 1725337296
        long daily_sunset_0 = daily["sunset"][0];   // 1725385761

        temperature = (double)current_temperature_2m;
        humidity = (int)current_relative_humidity_2m;
        pressure = (int)current_surface_pressure;
        sunrise = (int)daily_sunrise_0;
        sunset = (int)daily_sunset_0;

        DEBUG_SERIAL_PRINTLN("Outdoor temperature: " + String(temperature) + " °C");
        DEBUG_SERIAL_PRINTLN("Outdoor humidity: " + String(humidity) + " %rH");
#ifdef DEBUG
        struct tm *sunriseTime = localtime(&sunrise);
        Serial.printf("Sunrise: %02u:%02u\n", sunriseTime->tm_hour, sunriseTime->tm_min);
        struct tm *sunsetTime = localtime(&sunset);
        Serial.printf("Sunset: %02u:%02u\n", sunsetTime->tm_hour, sunsetTime->tm_min);
#endif
        return 1;
    }
    return 0;
}
