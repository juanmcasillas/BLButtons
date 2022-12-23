//////////////////////////////////////////////////////////////////////////////
///
/// @file i2cenc.ino
/// @author Juan M. Casillas (juanm.casillas@gmail.com)
/// @brief test the PCF8574 and encoder lib
/// @version 0.1
/// @date 2022/12/22
///
/// Code to test the PCF8574 expansor and some features.
/// P0 is the pin nearest the address jumpers.
/// 
/// @copyright Copyright (c) 12/22/2022 Juan M. Casillas <juanm.casillas@gmail.com>
///
//////////////////////////////////////////////////////////////////////////////
// PCF8574
// https://www.luisllamas.es/mas-pines-digitales-con-arduino-y-el-expansor-es-i2c-pcf8574/
// use this https://github.com/xreef/PCF8574_library/releases
// https://github.com/khoih-prog/ESP32TimerInterrupt
//
// 32 (0x20)
// default jumper connection:
// 32 (0x20) address
// x x x
// x x x
// | | | 
// x x x
//A0   A2
//
// 33 (0x21) address
// x x x
// |
// x x x
//   | | 
// x x x
//A0   A2
//
// 35 (0x23) address
// x x x
// | |
// x x x
//     |  
// x x x
//A0   A2
//

// our working example
//
// 35 (0x23) address
// x x x
// | |
// x x x
//     |  
// x x x
//A0   A2
//

// P0 -> push_button
// P1 -> push_button
// P2 -> push_button

// P3 -> SW
// P4 -> DT
// P5 -> CLK


// #define RELEASE 1
//#define DEBUG 1

#include <Arduino.h>
#include <PCF8574.h>
#include <Ticker.h>
#include <HardwareSerial.h>
#include "rotencoder.h"
#include "bltools.h"

#define EXPANDER_ADDESS 0x23
PCF8574 pcf8574(EXPANDER_ADDESS);
RotaryEncoder *encoder;
Ticker periodicTicker;

HardwareSerial DriverPort( 1 );
#define PIN_RX 27
#define PIN_TX 26


void timer_isr() {
    encoder->service();
}



void setup() {
    #ifndef RELEASE	  
    Serial.begin (115200);
    Serial.println("i2cTEST Started");
    #endif
    // serial port
    DriverPort.begin(SERIAL_SPEED, SERIAL_8N1, PIN_RX, PIN_TX );
    delay(500);

    // encoder pins
    pcf8574.pinMode(P0, INPUT_PULLUP);
    pcf8574.pinMode(P1, INPUT_PULLUP);
    pcf8574.pinMode(P2, INPUT_PULLUP);
    //first example
    //pcf8574.pinMode(P3, INPUT_PULLUP);    // set inside the code Button
    //pcf8574.pinMode(P4, INPUT_PULLUP);     
    //pcf8574.pinMode(P5, INPUT_PULLUP);

    // not needed, work the encoder, but safe.
    pcf8574.pinMode(P3, INPUT_PULLUP);    
    pcf8574.pinMode(P4, INPUT_PULLUP);     

    pcf8574.pinMode(P6, INPUT_PULLUP);
    pcf8574.pinMode(P7, INPUT_PULLUP);

    // encoder = new RotaryEncoder(&pcf8574, P5, P4, P3); // CLK(A), DT(B), SW
    encoder = new RotaryEncoder(&pcf8574, P3, P4, -1); // CLK(A), DT(B), SW

    // Interval in microsecs
    // Set low latency with this method or uncomment LOW_LATENCY define in the library
    // Needed for encoder

    pcf8574.setLatency(0);
    // Start library
    #ifndef RELEASE	  
    if (pcf8574.begin()){
        Serial.println("pcf8574 lib ok");
    }else{
        Serial.println("pcf8574 lib ko");
    }
    #endif
    
    periodicTicker.attach_ms(1, timer_isr);

}



long prev_value = 0;

void loop() {

    PREV_BUTTONS = BUTTONS;
    // 
    // read the data. This must be encapsulated in a function.
    // 
    if ( DriverPort.available() >= PACKET_SIZE ) {
        uint8_t data[6]; // size of serial_package_s;

        for (int i=0; i< PACKET_SIZE; i++) {
            data[i] = DriverPort.read();
        }
        PACKET.header = data[0];
        PACKET.data   = data[1] | (data[2] << 8) | (data[3] << 16) | (data[4] << 24);
        PACKET.footer = data[5];

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

    // the IC part here.
    uint8_t p0 = pcf8574.digitalRead(P0);
    uint8_t p1 = pcf8574.digitalRead(P1);
    uint8_t p2 = pcf8574.digitalRead(P2);
    uint8_t p6 = pcf8574.digitalRead(P6);
    uint8_t p7 = pcf8574.digitalRead(P7);

    p0 = ( p0 == 0 ? 1 : 0); // not wired
    p1 = ( p1 == 0 ? 1 : 0);
    p2 = ( p2 == 0 ? 1 : 0); 
    p6 = ( p6 == 0 ? 1 : 0); 
    p7 = ( p7 == 0 ? 1 : 0); 

    #ifndef RELEASE
    // not connected in hw
    if (p0) {
        memset(line, 0, 128);
        sprintf(line,"button pressed: %d, [GPIO %d]", 0, P0);
        Serial.println(line);
    }
    if (p1) {
        memset(line, 0, 128);
        sprintf(line,"button pressed: %d, [GPIO %d]", 1, P1);
        Serial.println(line);
    }
    if (p2) {
        memset(line, 0, 128);
        sprintf(line,"button pressed: %d, [GPIO %d]", 2, P2);
        Serial.println(line);
    }
    if (p6) {
        memset(line, 0, 128);
        sprintf(line,"button pressed: %d, [GPIO %d]", 6, P6);
        Serial.println(line);
    }
    if (p7) {
        memset(line, 0, 128);
        sprintf(line,"button pressed: %d, [GPIO %d]", 7, P7);
        Serial.println(line);
    }

    switch (encoder->getButton()) {
        case RotaryEncoder::PRESSED:
            Serial.println("Encoder Button pressed");
            break;
    }

    switch (encoder->getDirection()) {
        case RotaryEncoder::RIGHT:
            Serial.println("Encoder CW (Right)");
            break;
        case RotaryEncoder::LEFT:
            Serial.println("Encoder CCW (Left)");
            break;

    }
    long value = encoder->getValue();
    
    if (value != prev_value) {
        Serial.println(value);
        prev_value = value;
    }


    #endif
    delay(100);
}


