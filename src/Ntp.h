#pragma once

#include <ESP8266WiFi.h>
#include <TimeLib.h>
#include <WiFiUdp.h>

class Ntp {
public:
    Ntp();
    ~Ntp();

    time_t getTime(char* server);

private:

};