//*****************************************************************************
// OpenWeather.cpp - Get weather data from OpenWeather
//*****************************************************************************

#include "OpenWeather.h"

OpenWeather::OpenWeather() {
}

OpenWeather::~OpenWeather() {
}

uint8_t OpenWeather::getOutdoorConditions(String location, String apiKey, String langStr) {
    String response;
    String tempDescription;
    WiFiClient client;
    if (client.connect("api.openweathermap.org", 80)) {
        String url = "/data/2.5/weather?q=" + String(location) + "&units=metric&appid=" + String(apiKey) + "&lang=" + langStr;
        client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: api.openweathermap.org" + "\r\n" + "Connection: close\r\n\r\n");
        unsigned long startMillis = millis();
        while (client.available() == 0) {
            if (millis() - startMillis > 5000) {
                client.stop();
                return 0;
            }
        }
        while (client.available())
            response = client.readStringUntil('\r');
        response.trim();
        //response.replace('[', ' ');
        //response.replace(']', ' ');
        JSONVar weatherArray = JSON.parse(response);
        if (JSON.typeof(weatherArray) == "undefined") {
            #ifdef DEBUG
            Serial.println("Parsing weatherArray failed!");
            #endif
            return 0;
        }
        description = "";
        for (uint8_t i = 0; i < weatherArray["weather"].length(); i++) {
            tempDescription = (const char*)weatherArray["weather"][i]["description"];
            description += tempDescription + " ";
        }
        temperature = (double)weatherArray["main"]["temp"];
        humidity = (int)weatherArray["main"]["humidity"];
        pressure = (int)weatherArray["main"]["pressure"];
        sunrise = (int)weatherArray["sys"]["sunrise"]; // api also provides sunrise / sunset times. Just use them as well.
        sunset = (int)weatherArray["sys"]["sunset"];
        return 1;
    }
    return 0;
}
