#pragma once

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include "Colors.h"
#include "Debug.h"
#include "Modes.h"
#include "Settings.h"

class WebServerClass
{
public:
    WebServerClass();
    void setup();
    void handle();

private:
    ESP8266WebServer server;

    void handleRoot();
    void handleNotFound();
    void handleButtonSettings();
    void handleCommitSettings();
    void handleReset();
    void handleSetEvent();
    void handleShowText();
    void handleControl();
    void callRoot();
    void handleButtonOnOff();
    void handleButtonMode();
    void handleButtonTime();

    // Helper to generate the root page content
    String generateRootPage();
    String generateSettingsPage();
};
