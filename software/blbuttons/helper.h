#ifndef __HELPER_H__
#define __HELPER_H__

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
#include <TimeLib.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>
#include <WiFiUdp.h>
#include <NtpClientLib.h>


#include "config.h"
#include "logger.h"

class HelperClass {
    public:
        String GetTimeStampNow();
        unsigned long ToTimestamp(String d); 

        
        void Configure_NetWork();

    public:
        String getMacAddress();
        void Setup_IP();
        void Setup_AP();
        bool Setup_STA();
        void Setup_OTA();
        void Setup_NTP();
};

extern HelperClass HELPER;

#endif