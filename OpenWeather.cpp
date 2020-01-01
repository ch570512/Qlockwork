//******************************************************************************
// OpenWeather.cpp - Get weather data from OpenWeather
//******************************************************************************

#include "OpenWeather.h"

OpenWeather::OpenWeather()
{
}

OpenWeather::~OpenWeather()
{
}

uint8_t OpenWeather::getOutdoorConditions(String location, String apiKey)
{
	String response;
	WiFiClient client;
	if (client.connect("api.openweathermap.org", 80))
	{
		String url = "/data/2.5/weather?q=" + String(location) + "&units=metric&appid=" + String(apiKey);
		client.print(String("GET ") + url + " HTTP/1.1\r\n" + "Host: api.openweathermap.org" + "\r\n" + "Connection: close\r\n\r\n");
		unsigned long timeout = millis();
		while (client.available() == 0)
		{
			if (millis() - timeout > 5000)
			{
				client.stop();
				return 0;
			}
		}
		while (client.available())
			response = client.readStringUntil('\r');
		response.trim();
		response.replace('[', ' ');
		response.replace(']', ' ');
		JSONVar weatherArray = JSON.parse(response);
		description = weatherArray["weather"]["description"];
		temperature = (double)weatherArray["main"]["temp"];
		humidity = (int)weatherArray["main"]["humidity"];
		pressure = (int)weatherArray["main"]["pressure"];
		//if (description == "")
		//{
		//	description = "?Error";
		//	temperature = 0;
		//	humidity = 0;
		//	pressure = 0;
		//}
		return 1;
	}
}
