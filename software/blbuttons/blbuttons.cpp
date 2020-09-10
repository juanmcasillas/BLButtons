///////////////////////////////////////////////////////////////////////////////
/// BLBUttons Implementation //////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#include "blbuttons.h"

// arduino init
bool BLButtonsClass::begin(fs::FS *fs, float scheduled_interval) {
    _fs = fs;
    _scheduled_interval = scheduled_interval;

    this->configure_inputs();
    LOGGER.INFO("Alarm starting");

    // configure the scheduler checker. Do the things scheduled_interval

    _ticker.attach(this->_scheduled_interval, &AlarmClass::CheckScheduler, static_cast<void *>(this));
    return (true);
}

// show some log info about configuration, starts, and so on (e.g. position, dates, schedules)
void BLButtonsClass::IsRunning() {
    LOGGER.INFO("Alarm is Running and Handling Requests!");
}

void BLButtonsClass::CheckScheduler(void *arg) {
    AlarmClass *self = reinterpret_cast<AlarmClass *>(arg);

    //
    // running each SCHEDULE_PERIOD
    // First, read all the inputs, and set the flags
    // Second, run the state machine and fire events
    //

    KEY_STATUS key_status = KEY_STATUS::NONE;
    String key = "";
    bool any_zone_fired = false;    // true if zone is enabled, and fired.

    if (mfrc522.PICC_IsNewCardPresent()) {
        if (mfrc522.PICC_ReadCardSerial()) {
            if (self->valid_key(mfrc522.uid.uidByte)) {
                key_status = KEY_STATUS::VALID;
            } else {
                key_status = KEY_STATUS::INVALID;
            }
            key = self->print_key(mfrc522.uid.uidByte);  
            mfrc522.PICC_HaltA();
        }
    }
    if (key_status == KEY_STATUS::INVALID) {
        LOGGER.WARNING("Trying to using an invalid key: %s", key.c_str());
    }

    for (int i = 0; i < MAX_ZONES; i++) {
        if (CONFIG.zones[i].pin == 0) {
            continue;
        }
        int value = digitalRead(CONFIG.zones[i].pin);

        /*
        DEBUGLOG("%s: read: %d (%s)\n", 
                CONFIG.zones[i].name.c_str(), 
                value, 
                (CONFIG.zones[i].enabled ? "enabled" : "disabled"));
        */

        CONFIG.zones[i].fired = (value == HIGH ? true : false);

        if (value == HIGH && CONFIG.zones[i].enabled ) {          
            any_zone_fired = true;  
            LOGGER.INFO("Zone: %s wired to pin %d FIRED",CONFIG.zones[i].name.c_str(), CONFIG.zones[i].pin);

            CONFIG.last_event = HELPER.GetTimeStampNow() + " " + CONFIG.zones[i].name;
        }
    }

    //
    // run the state machine
    // CONFIG.armed has the web selector for ARM or DISARM the alarm.
    // ARMED_DELAY is an internal (trasient) state to allow people going out
    //

    switch (CONFIG.STATUS) {

        case ALARM_STATUS::DISARMED:
            //DEBUGLOG("STATUS::DISARMED\n");

            if (key_status == KEY_STATUS::VALID || CONFIG.armed) {
                // go to ALARM_STATUS::ARMED_DELAY in the next round
                LOGGER.INFO("RFID read a valid ARMED key. Going to ARMED_DELAY state");
                CONFIG.STATUS = ALARM_STATUS::ARMED_DELAY;
                CONFIG.armed = true;
                CONFIG.armed_delay_time_left = millis();
                self->siren_beep_init();
                return;
            }
            break;

        case ALARM_STATUS::ARMED_DELAY:
            //DEBUGLOG("STATUS::ARMED_DELAY\n");

            if (key_status == KEY_STATUS::VALID || ! CONFIG.armed) {
                // go to ALARM_STATUS::DISARMED in next round
                LOGGER.INFO("RFID read a valid DISARMED key. Going to DISARMED state");
                CONFIG.STATUS = ALARM_STATUS::DISARMED;
                CONFIG.armed = false;
                CONFIG.armed_delay_time_left = 0;
                return;
            }
            // in this state, if time has passed, move to ARMED.
            if (millis() >  CONFIG.armed_delay_time_left + CONFIG.armed_delay) {
                LOGGER.INFO("ARMED_DELAY time expired. Going to ARMED state");
                CONFIG.STATUS = ALARM_STATUS::ARMED;
                CONFIG.armed_delay_time_left = 0;
                self->siren_beep_init();
                return;
            }
            // else, do some sound for confirmation during the DELAY time
            self->siren_beep();
            break;

        case ALARM_STATUS::ARMED:
            //DEBUGLOG("STATUS::ARMED\n");

            if (key_status == KEY_STATUS::VALID || !CONFIG.armed) {
                // go to ALARM_STATUS::DISARMED in next round
                LOGGER.INFO("RFID read a valid DISARMED key. Going to DISARMED state");
                CONFIG.STATUS = ALARM_STATUS::DISARMED;
                CONFIG.armed = false;
                self->siren_off();
                return;
            }

            // check here if some has been fired, and start the alarm, in that case.

            if (!CONFIG.siren.sounding && any_zone_fired) {
                LOGGER.ERROR("Intrusion detected [%s]", CONFIG.last_event.c_str());
                self->siren_on();
                return;
            }

            if (CONFIG.siren.sounding) {
                self->siren_sound();
                return;
            }

            break;

    };

}

void BLButtonsClass::siren_on() {

    if (CONFIG.siren.sounding) {
        return;
    }

    CONFIG.siren.sounding = true;

    if (! CONFIG.siren.muted) {
        digitalWrite(CONFIG.siren.pin, HIGH);  // on
    }

    CONFIG.siren.time_left = millis();
    LOGGER.INFO("Siren ON");

}

void BLButtonsClass::siren_sound() {
    if (millis() >  CONFIG.siren.time_left + CONFIG.siren.duration) {
        this->siren_off();
    }
}

void BLButtonsClass::siren_off() {
   
    if (! CONFIG.siren.sounding) {
        return;
    }

    CONFIG.siren.sounding = false;

    if (! CONFIG.siren.muted) {
        digitalWrite(CONFIG.siren.pin, LOW);  // off
    }

    CONFIG.siren.time_left = 0;
    LOGGER.INFO("Siren OFF"); 
}

void BLButtonsClass::siren_beep_init() {
    CONFIG.siren.beep_wait_mode = false;
    CONFIG.siren.beep_left = 0;
    digitalWrite(CONFIG.siren.pin, LOW);  // off
}

void BLButtonsClass::siren_beep() {
   if (CONFIG.siren.beep_wait_mode == false) {
        if (CONFIG.siren.beep_left == 0) {
            CONFIG.siren.beep_left = millis();
            digitalWrite(CONFIG.siren.pin, HIGH);  // on
            return;
        }
        if (millis() >  CONFIG.siren.beep_left + CONFIG.siren.beep_duration) {
            digitalWrite(CONFIG.siren.pin, LOW);  // on
            CONFIG.siren.beep_left = 0;
            CONFIG.siren.beep_wait_mode = true;
            return;
        }
   } else {
        if (CONFIG.siren.beep_left == 0) {
            CONFIG.siren.beep_left = millis();
            return;
        }
        if (millis() >  CONFIG.siren.beep_left + CONFIG.siren.beep_wait) {
            CONFIG.siren.beep_left = 0;
            CONFIG.siren.beep_wait_mode = false;
            return;
        }
   }
}


void BLButtonsClass::configure_inputs() {
    // configure zones
    for (int i = 0; i < MAX_ZONES; i++) {
        if (CONFIG.zones[i].pin == 0) {
            continue;
        }
        pinMode(CONFIG.zones[i].pin, INPUT_PULLUP);
        LOGGER.INFO("Zone: %s is wired to pin %d",CONFIG.zones[i].name.c_str(), CONFIG.zones[i].pin);
    }

    // configure relay pin

    pinMode(CONFIG.siren.pin, OUTPUT);
    digitalWrite(CONFIG.siren.pin, LOW);  // alarm, off
}

String BLButtonsClass::GetStatus(AsyncWebServerRequest *request) {
    StaticJsonDocument<1024> root;

    // handle timeStamp set if AP mode is set
    // set the stamp as UTC instead TZ, this
    // can be configured, but for now, its ok

    if (request->hasParam("stamp", true) && CONFIG.wifi_sta == false) {
        AsyncWebParameter *p = request->getParam("stamp", true);
        String s_stamp = p->value().c_str();
        long stamp = s_stamp.toInt();

        // if drift is greater than one hour, update it
        // (to avoid constant time updates)
        if (abs(now() - stamp) > MAX_TIME_DRIFT) {
            setTime(stamp);
            LOGGER.INFO("Time updated");
        }
    }

    root["status"] = "OK";
    root["msg"] = "status.json";
    root["time"] = HELPER.GetTimeStampNow();
    root["armed"] = CONFIG.armed;
    root["last_event"] = CONFIG.last_event;
    root["siren"] = CONFIG.siren.sounding;
    root["muted"] = CONFIG.siren.muted;

    JsonArray jsonzones = root.createNestedArray("zones");
    for (int i = 0; i < MAX_ZONES; i++) {
        // don't store empty zones
        if (CONFIG.zones[i].pin == 0) {
            continue;
        }

        StaticJsonDocument<512> jsonzone;
        jsonzone["id"] = i;
        jsonzone["name"] = CONFIG.zones[i].name;
        jsonzone["enabled"] = CONFIG.zones[i].enabled;
        jsonzone["fired"] = CONFIG.zones[i].fired;
        jsonzones.add(jsonzone);
    }
    String ret;
    serializeJson(root, ret);
    return (ret);
}


String BLButtonsClass::SaveConfig(AsyncWebServerRequest *request, bool *error) {

    // STEP 1
    // requires one parameter, args with the json string inside, using POST (true= is POST)

    String json_value = "";
    String ret;
    StaticJsonDocument<512> root;

    *error = true;
    root["status"] = "ERROR";
    root["msg"] = "invalid arguments";
    root["time"] = HELPER.GetTimeStampNow();

    // if args not found, exit

    if (!request->hasParam("args", true)) {
        serializeJson(root, ret);
        return (ret);
    }

    AsyncWebParameter *p = request->getParam("args", true);
    json_value = p->value().c_str();

    // step 2, deserialize the JSON info
    StaticJsonDocument<2048> json;


    auto jerror = deserializeJson(json, json_value);
    if (jerror) {
        // error parsing json args
        root["msg"] = jerror.c_str();
        DEBUGLOG("SaveConfig: Failed to parse args: %s\n", jerror.c_str());

        serializeJson(root, ret);
        return (ret);
    }

    // process the save configuration values.
    // first, check the AUTH is set & is correct


    if ( !json.containsKey("auth") || 
         (json.containsKey("auth") && CONFIG.passwd != json["auth"].as<const char *>()) ) {
        // no auth, or invalid auth        
        root["msg"] = "bad auth";
        DEBUGLOG("SaveConfig: bad auth\n");
        serializeJson(root,ret);
        return(ret);
      
    }

    // extract parameters
    if (json.containsKey("armed")) CONFIG.armed = json["armed"].as<bool>();
    if (json.containsKey("muted")) CONFIG.siren.muted = json["muted"].as<bool>();

    if (json.containsKey("zones")) { 
        StaticJsonDocument<1024> zones = json["zones"];
        JsonArray array = zones.as<JsonArray>();
        int i = 0;
        for(JsonVariant v : array) {
            StaticJsonDocument<512> zone = v;
            int id = -1;
            int enabled = false;

            if (zone.containsKey("id")) { id = zone["id"].as<int>(); }
            if (zone.containsKey("enabled")) { enabled = zone["enabled"].as<bool>(); }
            
            if (id >= 0 && id < MAX_ZONES) {
                CONFIG.zones[id].enabled = enabled;
            }

            i++;
            if (i >= MAX_ZONES) {
                break;
            }
        }
    }
    // do the config persistent
    
    CONFIG.SaveConfig();
    LOGGER.INFO("Configuration Updated");

    *error = false;
    // return the Status information, instead my JSON
    return( this->GetStatus(request) );
}

///
/// rfid key helpers
///

bool BLButtonsClass::key_equal(byte *arrayA, byte *arrayB) {
    for (int index = 0; index < KEY_SIZE; index++) {
        if (arrayA[index] != arrayB[index]) return false;
    }
    return true;
}

bool BLButtonsClass::valid_key(byte *arrayA) {
    for (int i = 0; i < MAX_KEYS; i++) {
        byte *ptr = CONFIG.keys[i];
        if (this->key_equal(arrayA, ptr)) {
            return (true);
        }
    }

    return (false);
}

String BLButtonsClass::print_key(byte *buffer) {
    char m[512];
    sprintf(m, "%.2x %.2x %.2x %.2x",buffer[0], buffer[1], buffer[2], buffer[3]);
    String ret;
    ret = m;
    return(ret);
}
