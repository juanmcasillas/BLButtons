#include <ArduinoOTA.h>
#ifdef ESP32
#include <FS.h>
#include <SPIFFS.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <esp_system.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESP8266mDNS.h>
#endif
#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>
#include <WiFiUdp.h>

#include "config.h"
#ifndef __WEBSERVER_H__
#define __WEBSERVER_H__

#define USER_CALLBACK_SIGNATURE std::function<void(AsyncWebServerRequest *request)> _usercallback

class WebServerClass {
    public:
        AsyncWebServer *WebServer;

        WebServerClass& setUSERCallback(USER_CALLBACK_SIGNATURE);
        void set_usercallbackfilter(String s);
        void begin(int port);
        void handle();
    protected:
        String _usercallbackfilter;
        USER_CALLBACK_SIGNATURE;
       
};

extern WebServerClass WebServer;

#endif