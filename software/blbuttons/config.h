#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

#include "debug.h"



#define MAX_ZONES 5     // maximum monitored zones (check pins availability)
#define MAX_KEYS  5     // maximum RFID valid keys numbers
#define KEY_SIZE  4     // RFID key size (4 bytes)
#define MAX_TIME_DRIFT 3600  // max time drift in seconds when trying to setting the time as AP (no NTP, no RTC)
#define BEEP_DURATION 50     // ms duration of the beep (alarm on)
#define BEEP_WAIT 1000       // ms wait before beeps

class SirenClass {
    public:
        int pin = 0;
        int duration = 0;
        // internal state
        bool sounding = false; // alarm is sounding
        bool muted = false; // alarm is sounding, but don't sound (test)
        unsigned long time_left = 0;

        // beeping in ARMED_DELAY
        bool beep_wait_mode = false;            // sound or wait
        unsigned long beep_left = 0;           // to store millis
        unsigned long beep_duration = BEEP_DURATION; // duration of the beep (alarm on)
        unsigned long beep_wait = BEEP_WAIT;     // wait before beeps
};

class ZoneClass {
    public:
        int pin = 0;
        String name = "default_zone";
        bool enabled = false;
        bool fired = false; // the read event
};

enum class ALARM_STATUS: int {
    DISARMED    = 0, // alarm is disarmed. sensing, but not SOUND
    ARMED_DELAY = 1, // alarm is in waitting state to be armed (to allow people exit)
    ARMED       = 2  // alarm is armed. sensing, and SOUND
};

class ConfigClass {
    public:
        bool ap_mode = false; // false, station (default)
        bool dhcp = false; 
        String wifi_ssid = "XXXX";
        String wifi_passwd = "YYYY";
        String ap_name = "alarm_AP";
        String ap_passwd = "alarm_AP";
        String http_user = "admin";
        String http_passwd = "admin";
        int retries = 5;
        int retry_delay = 5000; // 5 seconds
        int http_port = 80;
        IPAddress ip = IPAddress(192,168,5,100);
        IPAddress gateway = IPAddress(192,168,5,1);
        IPAddress network = IPAddress(255,255,255,0);
        IPAddress dns = IPAddress(8,8,8,8);
        String ntp_server = "es.pool.ntp.org";
        int ntp_period = 15;
        int ntp_interval = 63;
        int ntp_timezone = 10; // madrid/spain
        bool ntp_daylight = true;
        float scheduled_interval = 1.0;
        String log_file = "/logfile.log";

        //
        // alarm configuration
        //

        SirenClass siren;
        ZoneClass zones[MAX_ZONES];
        byte keys[MAX_KEYS][KEY_SIZE] = { 
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
            { 0, 0, 0, 0 },
        };

        String passwd = "1234";
        unsigned long armed_delay = 1000; // 1 second
        unsigned long armed_delay_time_left = 0;

        bool LoadConfig();
        bool SaveConfig();
        void begin(fs::FS *fs, String config_file);

        // non persistent config
        ALARM_STATUS STATUS = ALARM_STATUS::DISARMED; 
        bool armed = false;
        String last_event = "-";
        bool wifi_sta = true; // false if ESP32 started as AP


    protected:
        fs::FS *_fs;
        String _config_file;
};

extern ConfigClass CONFIG;
#endif