#include <Arduino.h>
#include <HardwareSerial.h>
#include "bltools.h"

// #define RELEASE 1
// #define DEBUG 1

HardwareSerial DriverPort( 1 );
#define PIN_RX 27
#define PIN_TX 26


void setup() {
#ifndef RELEASE
    Serial.begin(115200);
    Serial.println("ESPreader Started");
#endif
    DriverPort.begin(SERIAL_SPEED, SERIAL_8N1, PIN_RX, PIN_TX );
    delay(1000);
}

// Arduino sends 6 bytes (the size of the structure ... there) but here, it 
// doubles it (due its a 64 bit architecture, instead of 32). It "works" 
// because mixes the first package with the second. Fixed the size, it 
// works fine.
// define packet size ... in bytes.

void loop() {
    
    PREV_BUTTONS = BUTTONS;
    if ( DriverPort.available() >= PACKET_SIZE ) {
        uint8_t data[6]; // size of serial_package_s;

        for (int i=0; i< PACKET_SIZE; i++) {
            data[i] = DriverPort.read();
        }
        PACKET.header = data[0];
        PACKET.data   = data[1] | (data[2] << 8) | (data[3] << 16) | (data[4] << 24);
        PACKET.footer = data[5];

        // memset(line, 0, 128);
        // sprintf(line, "%x, %x, %x, %x, %x, %x",data[0],data[1],data[2],data[3],data[4],data[5]);
        // Serial.println(line);         

        // memset(line, 0, 128);
        // sprintf(line, "%x, %lu, %x",PACKET.header, PACKET.data, PACKET.footer);
        // Serial.println(line);      

        // Serial.println(sizeof(uint8_t));    
        // Serial.println(sizeof(PACKET));    
        // Serial.println(sizeof(serial_package_s));    
        // Serial.println(sizeof(uint8_t));
        
        if (PACKET.header == 0xDE && PACKET.footer == 0xAD) {
            BUTTONS = PACKET.data;
        }
        else {
            // some problem with the serial port.
        }


        
    }
    if (PREV_BUTTONS != BUTTONS) {
        #ifndef RELEASE
        print_state();
        #endif
    }
    delay(100);
}

