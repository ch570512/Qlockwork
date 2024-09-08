//*****************************************************************************
// MeteoWeather.cpp - Get weather data from Open-Meteo
//*****************************************************************************

#include "MeteoWeather.h"

MeteoWeather::MeteoWeather() {
}

MeteoWeather::~MeteoWeather() {
}

uint8_t MeteoWeather::getOutdoorConditions(String lat, String lon, String timezone) {
    String response;
    WiFiClient client;
    JsonDocument doc;
    timezone.replace("/","%2F");
    if (client.connect("api.open-meteo.com", 80)) {
        String url = "/v1/forecast?latitude=" + String(lat) + "&longitude=" + String(lon) + "&current=temperature_2m,relative_humidity_2m,surface_pressure&daily=sunrise,sunset&timeformat=unixtime&timezone=" + timezone + "&forecast_days=1";
        client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: api.open-meteo.com" + "\r\n" + "Connection: close\r\n\r\n");
        unsigned long startMillis = millis();
        while (client.available() == 0) {
            if (millis() - startMillis > 5000) {
                client.stop();
                return 0;
            }
        }
         
        while (client.available())
            response = client.readString();
        #ifdef DEBUG
            Serial.println("Weather api response:");
            Serial.println(response);
        #endif

        response.trim();
        int first = response.indexOf("{"); // Extract json file content of message.
        String extract = response.substring(first);

        // Created with https://arduinojson.org/v7/assistant/

        DeserializationError error = deserializeJson(doc, extract);

        if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return 0;
        }

        float latitude = doc["latitude"]; // 52.52
        double longitude = doc["longitude"]; // 13.419998
        double generationtime_ms = doc["generationtime_ms"]; // 0.054001808166503906
        int utc_offset_seconds = doc["utc_offset_seconds"]; // 7200
        const char* timezone = doc["timezone"]; // "Europe/Berlin"
        const char* timezone_abbreviation = doc["timezone_abbreviation"]; // "CEST"
        int elevation = doc["elevation"]; // 38

        JsonObject current_units = doc["current_units"];
        const char* current_units_time = current_units["time"]; // "unixtime"
        const char* current_units_interval = current_units["interval"]; // "seconds"
        const char* current_units_temperature_2m = current_units["temperature_2m"]; // "°C"
        const char* current_units_relative_humidity_2m = current_units["relative_humidity_2m"]; // "%"
        const char* current_units_surface_pressure = current_units["surface_pressure"]; // "hPa"

        JsonObject current = doc["current"];
        long current_time = current["time"]; // 1725390000
        int current_interval = current["interval"]; // 900
        float current_temperature_2m = current["temperature_2m"]; // 27.1
        int current_relative_humidity_2m = current["relative_humidity_2m"]; // 53
        float current_surface_pressure = current["surface_pressure"]; // 1010.1

        JsonObject daily_units = doc["daily_units"];
        const char* daily_units_time = daily_units["time"]; // "unixtime"
        const char* daily_units_sunrise = daily_units["sunrise"]; // "unixtime"
        const char* daily_units_sunset = daily_units["sunset"]; // "unixtime"

        JsonObject daily = doc["daily"];
        long daily_time_0 = daily["time"][0]; // 1725314400
        long daily_sunrise_0 = daily["sunrise"][0]; // 1725337296
        long daily_sunset_0 = daily["sunset"][0]; // 1725385761

        temperature = (double)current_temperature_2m;
        humidity = (int)current_relative_humidity_2m;
        pressure = (int)current_surface_pressure;
        sunrise = (int)daily_sunrise_0; // api also provides sunrise / sunset times. Just use them as well.
        sunset = (int)daily_sunset_0;
        return 1;
    }
    return 0;
}
