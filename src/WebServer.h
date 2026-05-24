#pragma once

#include <ESP8266WebServer.h>
#include <cstdint>

void handleNotFound(ESP8266WebServer &webServer);
void handleReset(ESP8266WebServer &webServer);