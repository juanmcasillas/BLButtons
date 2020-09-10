#include "helper.h"

// build the string just like the scheduled.
String HelperClass::GetTimeStampNow() {
    time_t t = now(); 
    char *buf = (char *)malloc(1024);
    sprintf(buf, "%02d/%02d/%04d %02d:%02d:%02d", day(t), month(t), year(t), hour(t), minute(t), second(t));
    String ret = buf;
    free(buf);
    return(ret);
}

unsigned long HelperClass::ToTimestamp(String d) {
    // EPOCH 01/01/1970 00:00:00
    // 1 hour	3600 seconds
    // 1 day	86400 seconds
    // 1 week	604800 seconds
    // 1 month (30.44 days) 	2629743 seconds
    // 1 year (365.24 days) 	 31556926 seconds


    int day, month, year, hour, minute, second;

    sscanf(d.c_str(),"%02d/%02d/%04d %02d:%02d:%02d",
         &day, &month, &year, &hour, &minute, &second);
    
    unsigned long ret;

    ret = (year-1970 * (unsigned long)31556926) + (month * 2629743) + (day * 86400) +
          (hour * 3600) + (minute * 60) + second;
    
    return(ret);
}

void HelperClass::Setup_NTP() {
  NTP.setInterval(CONFIG.ntp_interval);
	NTP.begin(CONFIG.ntp_server.c_str(), CONFIG.ntp_timezone / 10, CONFIG.ntp_daylight);
	NTP.setInterval(CONFIG.ntp_period, CONFIG.ntp_period * 60);
	DEBUGLOG("NTP Last Sync: %s\n", NTP.getTimeDateString(NTP.getLastNTPSync()).c_str());
}

void HelperClass::Configure_NetWork() {

    CONFIG.wifi_sta = true; // try to start as STA
    DEBUGLOG("HW MAC_ADDR: %s\n", this->getMacAddress().c_str());

    // if not DHCP, configure static IP values (default)
    if (CONFIG.dhcp==false) {
        this->Setup_IP();
    }

    // if not AP mode, configure station mode
    if (CONFIG.ap_mode==false) {
        if (this->Setup_STA() == false) {
            // fallback to AP
            this->Setup_AP();    
        }
    }
    else {
        this->Setup_AP();
    }
    // Configure NTP
    this->Setup_NTP();
    
    // configure OTA
    this->Setup_OTA();
    
    // add MDNS service
    MDNS.addService("http","tcp",CONFIG.http_port);
}



String HelperClass::getMacAddress() {
	uint8_t baseMac[6];
	// Get MAC address for WiFi station
	esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
	char baseMacChr[18] = {0};
	sprintf(baseMacChr, "%02X:%02X:%02X:%02X:%02X:%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
	return String(baseMacChr);
}


void HelperClass::Setup_IP() {
    if (! WiFi.config(CONFIG.ip, CONFIG.gateway, CONFIG.network, CONFIG.dns)) {
        DEBUGLOG("Failed to configure IP settings\n");
    }

}

void HelperClass::Setup_AP() {
     WiFi.mode(WIFI_AP);
     WiFi.softAPConfig(CONFIG.ip, CONFIG.gateway, CONFIG.network);
     WiFi.softAP(CONFIG.ap_name.c_str(), CONFIG.ap_passwd.c_str());
     IPAddress ip = WiFi.softAPIP();
     DEBUGLOG("Configured as AP. http://%d.%d.%d.%d/ to manage\n", ip[0], ip[1], ip[2], ip[3]);
     CONFIG.wifi_sta = false; // we are AP
}

bool  HelperClass::Setup_STA() {
     WiFi.mode(WIFI_AP_STA);
     WiFi.disconnect();
  	 WiFi.setAutoReconnect(true);
     WiFi.begin(CONFIG.wifi_ssid.c_str(), CONFIG.wifi_passwd.c_str());
    
    int counter =0;
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        DEBUGLOG("Connecting as Wifi_STA, failed!\n");
        WiFi.disconnect(false);
        delay(CONFIG.retry_delay);
        WiFi.begin(CONFIG.wifi_ssid.c_str(), CONFIG.wifi_passwd.c_str());
        counter++;
        if (counter == CONFIG.retries) {
            DEBUGLOG("Max Retires happen (%d). Exiting\n", CONFIG.retries);
            return(false);
        }
    }
     DEBUGLOG("Configured as STA\n");
    return(true);
}

void HelperClass::Setup_OTA() {

  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);
  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");
  // No authentication by default
  // ArduinoOTA.setPassword("admin");
  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

    ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    DEBUGLOG("OTA: Start updating %s", type.c_str());
  });
  ArduinoOTA.onEnd([]() {
    DEBUGLOG("\nOTA: End");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    DEBUGLOG("OTA: Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    DEBUGLOG("OTA: Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      DEBUGLOG("OTA: Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      DEBUGLOG("OTA: Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      DEBUGLOG("OTA: Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      DEBUGLOG("OTA: Receive Failed");
    } else if (error == OTA_END_ERROR) {
      DEBUGLOG("OTA: End Failed");
    }
  });
  ArduinoOTA.begin();
}




HelperClass HELPER;