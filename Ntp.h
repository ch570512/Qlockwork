//******************************************************************************
// Ntp.h - Get UTC time from NTP
//******************************************************************************

#pragma once

#include <ESP8266WiFi.h>
#include <TimeLib.h>
#include <WiFiUDP.h>

class Ntp
{
public:
	Ntp();
	~Ntp();

	time_t getTime(char* server);

private:

};
