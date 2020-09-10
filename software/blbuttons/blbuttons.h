///////////////////////////////////////////////////////////////////////////////
/// BLButtons Implementation //////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#ifndef __BLBUTTONS_H__
#define __BLBUTTONS_H__

#include <Arduino.h>
#include <FS.h>
#include <SPIFFS.h>
#include <Ticker.h>
#include <ArduinoJson.h>



#include "config.h"
#include "logger.h"
#include "helper.h"
#include "webserver.h"

class BLButtonsClass {
    public:

    protected:
        FS *_fs;            // ICPFS implementation
        Ticker _ticker;     // Periodic Time Dispatcher (see CheckScheduler)
        float _scheduled_interval;

    public:
        bool begin(fs::FS *fs, float scheduled_interval);
        void IsRunning(); // shows the start information


        static void CheckScheduler(void *arg); // Programmed each 1 second. Passed myself as arg.


        // exposed WWW methods (REST)
        String GetStatus(AsyncWebServerRequest *request);
        String SaveConfig(AsyncWebServerRequest *request, bool *error);
        // internal methods used to do things
    protected:
        void configure_inputs();
        bool key_equal(byte *arrayA, byte *arrayB);
        bool valid_key(byte *arrayA);
        String print_key(byte *buffer);

        /*
        void siren_on();
        void siren_sound();
        void siren_off();
        void siren_beep();
        void siren_beep_init();
        */
};

#endif
