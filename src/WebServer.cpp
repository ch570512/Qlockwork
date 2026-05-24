#include "WebServer.h"

#include <ESP8266WebServer.h>
#include <cstdint>

// Webpage 404
void handleNotFound(ESP8266WebServer &webServer)
{
    webServer.send(404, "text/plain", "404 - File Not Found.");
}

// Webpage reset
void handleReset(ESP8266WebServer &webServer)
{
    webServer.send(200, "text/plain", "RESET. I'll be back!");
    ESP.restart();
}