#include "config.h"
#include "logger.h"

void ConfigClass::begin(fs::FS *fs, String config_file) {
    _fs = fs;
    _config_file = config_file;

    this->LoadConfig();
}


bool ConfigClass::LoadConfig() {

	File configFile = _fs->open(this->_config_file.c_str(), "r");
	if (!configFile) {
		DEBUGLOG("ConfigClass: Failed to open config file %s\n", this->_config_file.c_str());
		return false;
	}

	size_t size = configFile.size();
	std::unique_ptr<char[]> buf(new char[size]);

	configFile.readBytes(buf.get(), size);
	configFile.close();
	DEBUGLOG("JSON file size: %d bytes\r\n", size);
	StaticJsonDocument<2048> json;

 
    auto error = deserializeJson(json, buf.get());
	if (error) {
        DEBUGLOG("ConfigClass: Failed to parse config file %s: %s\r\n", this->_config_file.c_str(), error.c_str());        
		return false;
	}
   
    

    if (json.containsKey("ap_mode")) {  this->ap_mode = json["ap_mode"].as<bool>(); }
    if (json.containsKey("dhcp")) {  this->dhcp = json["dhcp"].as<bool>(); }
    if (json.containsKey("wifi_ssid")) {  this->wifi_ssid = json["wifi_ssid"].as<const char *>(); }
    if (json.containsKey("wifi_passwd")) {  this->wifi_passwd = json["wifi_passwd"].as<const char *>(); }
    if (json.containsKey("ap_name")) {  this->ap_name = json["ap_name"].as<const char *>(); }
    if (json.containsKey("ap_passwd")) {  this->ap_passwd = json["ap_passwd"].as<const char *>(); }
    if (json.containsKey("http_user")) {  this->http_user = json["http_user"].as<const char *>(); }
    if (json.containsKey("http_passwd")) {  this->http_passwd = json["http_passwd"].as<const char *>(); }
    if (json.containsKey("retries")) {  this->retries = json["retries"].as<int>(); }
    if (json.containsKey("retry_delay")) {  this->retry_delay = json["retry_delay"].as<int>(); }
    if (json.containsKey("http_port")) {  this->http_port = json["http_port"].as<int>(); }
    if (json.containsKey("ip")) {  
        this->ip = IPAddress(json["ip"][0], json["ip"][1], json["ip"][2], json["ip"][3]);
    }
    if (json.containsKey("gateway")) {  
        this->gateway = IPAddress(json["gateway"][0], json["gateway"][1], json["gateway"][2], json["gateway"][3]);
    }
    if (json.containsKey("network")){  
        this->network = IPAddress(json["network"][0], json["network"][1], json["network"][2], json["network"][3]);
    }
    if (json.containsKey("dns")) {  
        this->dns = IPAddress(json["dns"][0], json["dns"][1], json["dns"][2], json["dns"][3]);
    }

    if (json.containsKey("ntp_server")) {  this->ntp_server = json["ntp_server"].as<const char *>(); }
    if (json.containsKey("ntp_period")) {  this->ntp_period = json["ntp_period"].as<int>(); }
    if (json.containsKey("ntp_interval")) {  this->ntp_interval = json["ntp_interval"].as<int>(); }
    if (json.containsKey("ntp_timezone")) {  this->ntp_timezone = json["ntp_timezone"].as<int>(); }
    if (json.containsKey("ntp_daylight")) {  this->ntp_daylight = json["ntp_daylight"].as<bool>(); }

    if (json.containsKey("scheduled_interval")) {  this->scheduled_interval = json["scheduled_interval"].as<float>(); }
    if (json.containsKey("log_file")) { this->log_file = json["log_file"].as<const char *>(); }

    // alarm configuration block

    if (json.containsKey("siren")) { 
        StaticJsonDocument<1024> siren = json["siren"];
        if (siren.containsKey("pin"))      { this->siren.pin = siren["pin"].as<int>(); }
        if (siren.containsKey("duration")) { this->siren.duration = siren["duration"].as<int>(); }
    }
    
    if (json.containsKey("zones")) { 
        StaticJsonDocument<1024> zones = json["zones"];
        JsonArray array = zones.as<JsonArray>();
        int i = 0;
        for(JsonVariant v : array) {
            StaticJsonDocument<512> zone = v;
            if (zone.containsKey("pin")) { this->zones[i].pin = zone["pin"].as<int>(); }
            if (zone.containsKey("name")) { this->zones[i].name = zone["name"].as<const char *>(); }
            if (zone.containsKey("enabled")) { this->zones[i].enabled = zone["enabled"].as<bool>(); }
            i++;
            if (i >= MAX_ZONES) {
                break;
            }
        }
    }

    if (json.containsKey("keys")) {        
        JsonArray array = json["keys"].as<JsonArray>();
        int i = 0;
        for(JsonVariant v : array) {
            StaticJsonDocument<512> key = v;
            if (key.containsKey("key")) {
                this->keys[i][0] = key["key"][0];
                this->keys[i][1] = key["key"][1];
                this->keys[i][2] = key["key"][2];
                this->keys[i][3] = key["key"][3];
            }
            i++;
            if (i>= MAX_KEYS) {
                break;
            }
        }
    }

    if (json.containsKey("passwd")) { this->passwd = json["passwd"].as<const char *>(); }
    if (json.containsKey("armed_delay")) { this->armed_delay = json["armed_delay"].as<unsigned long>(); }
     

	DEBUGLOG("Config initialized.\n");
    #ifndef RELEASE
        //String temp;
        //serializeJsonPretty(json, temp);
        //DEBUGLOG(temp.c_str());
        //DEBUGLOG("\n\n");
    #endif
	return true;
}



bool ConfigClass::SaveConfig() {

	StaticJsonDocument<2048> json;
    
    json["ap_mode"] = this->ap_mode;
    json["dhcp"] = this->dhcp;
    json["wifi_ssid"] = this->wifi_ssid;
    json["wifi_passwd"] = this->wifi_passwd;
    json["ap_name"] = this->ap_name;
    json["ap_passwd"] = this->ap_passwd;
     
    json["http_user"] = this->http_user;
    json["http_passwd"] = this->http_passwd;
    json["retries"] = this->retries;
    json["retry_delay"] = this->retry_delay;
    json["http_port"] = this->http_port;
    
    JsonArray jsonip = json.createNestedArray("ip");
	jsonip.add(this->ip[0]);
	jsonip.add(this->ip[1]);
	jsonip.add(this->ip[2]);
	jsonip.add(this->ip[3]);

    JsonArray jsongateway = json.createNestedArray("gateway");
	jsongateway.add(this->gateway[0]);
	jsongateway.add(this->gateway[1]);
	jsongateway.add(this->gateway[2]);
	jsongateway.add(this->gateway[3]);

    JsonArray jsonnetwork = json.createNestedArray("network");
	jsonnetwork.add(this->network[0]);
	jsonnetwork.add(this->network[1]);
	jsonnetwork.add(this->network[2]);
	jsonnetwork.add(this->network[3]);

    JsonArray jsondns = json.createNestedArray("dns");
	jsondns.add(this->dns[0]);
	jsondns.add(this->dns[1]);
	jsondns.add(this->dns[2]);
	jsondns.add(this->dns[3]);

    json["ntp_server"] = this->ntp_server;
    json["ntp_period"] = this->ntp_period;
    json["ntp_interval"] = this->ntp_interval;
    json["ntp_timezone"] = this->ntp_timezone;
    json["ntp_daylight"] = this->ntp_daylight;
    json["scheduled_interval"] = this->scheduled_interval;
    json["log_file"] = this->log_file;

    // alarm block
    JsonObject jsonsiren = json.createNestedObject("siren");
    jsonsiren["pin"] = this->siren.pin;
    jsonsiren["duration"] = this->siren.duration;

    JsonArray jsonzones = json.createNestedArray("zones");
    for (int i=0; i < MAX_ZONES; i++) {

        // don't store empty zones
        if (this->zones[i].pin == 0) {
            continue;
        }

        StaticJsonDocument<512> jsonzone;
        jsonzone["pin"] = this->zones[i].pin;
        jsonzone["name"] = this->zones[i].name;
        jsonzone["enabled"] = this->zones[i].enabled;
        jsonzones.add(jsonzone);
    }

    JsonArray jsonkeys = json.createNestedArray("keys");
    for (int i=0; i < MAX_KEYS; i++) {

        // don't store empty keys
        if (this->keys[i][0] == 0 && 
            this->keys[i][1] == 0 && 
            this->keys[i][2] == 0 && 
            this->keys[i][3] == 0) {
            continue;
        }

        StaticJsonDocument<512> jsonkey;
        const size_t CAPACITY = JSON_ARRAY_SIZE(KEY_SIZE);
        StaticJsonDocument<CAPACITY> json_key_a;
        JsonArray array = json_key_a.to<JsonArray>();

        array.add(this->keys[i][0]);
        array.add(this->keys[i][1]);
        array.add(this->keys[i][2]);
        array.add(this->keys[i][3]);
        jsonkey["key"] = array;
        jsonkeys.add(jsonkey);
    }

    json["passwd"] = this->passwd;
    json["armed_delay"] = this->armed_delay;

	File configFile = _fs->open(this->_config_file.c_str(), "w");
    
	if (!configFile) {
		DEBUGLOG("Failed to open config file %s for writing\r\n", this->_config_file.c_str());
		configFile.close();
		return false;
	}

#ifndef RELEASE
	//String temp;
	//serializeJsonPretty(json, temp);
    //DEBUGLOG(temp.c_str());
#endif

	serializeJson(json,configFile);
	configFile.flush();
	configFile.close();
	return true;
}


ConfigClass CONFIG;
