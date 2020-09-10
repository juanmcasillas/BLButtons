#include "webserver.h"


WebServerClass WebServer;


void WebServerClass::handle() {
  ArduinoOTA.handle();
}

void WebServerClass::set_usercallbackfilter(String s) {
	this->_usercallbackfilter = s;
}

WebServerClass& WebServerClass::setUSERCallback(USER_CALLBACK_SIGNATURE) {
	this->_usercallback = _usercallback;
	return *this;
}

void WebServerClass::begin(int port) {

  this->WebServer = new AsyncWebServer(port);

  #ifdef ESP32
  this->WebServer->addHandler(new SPIFFSEditor(SPIFFS, CONFIG.http_user,CONFIG.http_passwd));
  #elif defined(ESP8266)
  this->WebServer->addHandler(new SPIFFSEditor(CONFIG.http_user,CONFIG.http_passwd));
  #endif
    
    this->WebServer->on("/heap", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/plain", String(ESP.getFreeHeap()));
    });

    // JMC new callback for GenericHandler
     this->WebServer->on(this->_usercallbackfilter.c_str(), [this](AsyncWebServerRequest *request) {
      
      // deal with authentication here
      //if (!this->WebServer->checkAuth(request))
      //  return request->requestAuthentication();
      if (this->_usercallback)
      {
        return this->_usercallback(request);
      }
      else
      {
        String values = "Callback 500 error";
        request->send(500, "text/plain", values);
        values = "";
      }
     });

    // serve files in the www directory
    this->WebServer->serveStatic("/", SPIFFS, "/www/").setDefaultFile("index.html");
    

    this->WebServer->onNotFound([](AsyncWebServerRequest *request){
        Serial.printf("NOT_FOUND: ");
        if(request->method() == HTTP_GET)
        Serial.printf("GET");
        else if(request->method() == HTTP_POST)
        Serial.printf("POST");
        else if(request->method() == HTTP_DELETE)
        Serial.printf("DELETE");
        else if(request->method() == HTTP_PUT)
        Serial.printf("PUT");
        else if(request->method() == HTTP_PATCH)
        Serial.printf("PATCH");
        else if(request->method() == HTTP_HEAD)
        Serial.printf("HEAD");
        else if(request->method() == HTTP_OPTIONS)
        Serial.printf("OPTIONS");
        else
        Serial.printf("UNKNOWN");
        Serial.printf(" http://%s%s\n", request->host().c_str(), request->url().c_str());

        if(request->contentLength()){
        Serial.printf("_CONTENT_TYPE: %s\n", request->contentType().c_str());
        Serial.printf("_CONTENT_LENGTH: %u\n", request->contentLength());
        }

        int headers = request->headers();
        int i;
        for(i=0;i<headers;i++){
        AsyncWebHeader* h = request->getHeader(i);
        Serial.printf("_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
        }

        int params = request->params();
        for(i=0;i<params;i++){
        AsyncWebParameter* p = request->getParam(i);
        if(p->isFile()){
            Serial.printf("_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
        } else if(p->isPost()){
            Serial.printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        } else {
            Serial.printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }
        }

        request->send(404);
    });
    this->WebServer->onFileUpload([](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){
        if(!index)
        Serial.printf("UploadStart: %s\n", filename.c_str());
        Serial.printf("%s", (const char*)data);
        if(final)
        Serial.printf("UploadEnd: %s (%u)\n", filename.c_str(), index+len);
    });
    this->WebServer->onRequestBody([](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total){
        if(!index)
        Serial.printf("BodyStart: %u\n", total);
        Serial.printf("%s", (const char*)data);
        if(index + len == total)
        Serial.printf("BodyEnd: %u\n", total);
    });
    
    // begin
    this->WebServer->begin();

}