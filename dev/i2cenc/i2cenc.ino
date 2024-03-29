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

#include "ClickEncoderI2C.h"

#define EXPANDER_ADDESS 0x23

char line[128];
PCF8574 pcf8574(EXPANDER_ADDESS);
ClickEncoderI2C *encoder;
int16_t last, value;
Ticker periodicTicker;

void  timerIsr()
{
    encoder->service();

}

void setup() {
    #ifndef RELEASE	  
    Serial.begin (115200);
    Serial.println("i2cTEST Started");
    #endif
    delay(500);


    

    // encoder pins
    pcf8574.pinMode(P0, INPUT_PULLUP);
    pcf8574.pinMode(P1, INPUT_PULLUP);
    pcf8574.pinMode(P2, INPUT_PULLUP);
    pcf8574.pinMode(P3, INPUT_PULLUP);
    pcf8574.pinMode(P4, INPUT_PULLUP);
    pcf8574.pinMode(P5, INPUT_PULLUP);
    pcf8574.pinMode(P6, INPUT_PULLUP);
    pcf8574.pinMode(P7, INPUT_PULLUP);

    encoder = new ClickEncoderI2C(&pcf8574, P5, P4, P3); // CLK(A), DT(B), SW
    // Interval in microsecs
    periodicTicker.attach_ms(1, timerIsr); // 1000 microseconds -> 1 milisecond

    last = -1;

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



}



void loop() {

    value += encoder->getValue();

    uint8_t p0 = pcf8574.digitalRead(P0);
    uint8_t p1 = pcf8574.digitalRead(P1);
    uint8_t p2 = pcf8574.digitalRead(P2);


    p0 = ( 1 ? p0 == 0: 0);
    p1 = ( 1 ? p1 == 0: 0);
    p2 = ( 1 ? p2 == 0: 0);

    #ifndef RELEASE
    if (p0) {
        memset(line, 0, 128);
        sprintf(line,"button pressed: %d, [GPIO %d]", 0, p0);
        Serial.println(line);
    }
    if (p1) {
        memset(line, 0, 128);
        sprintf(line,"button pressed: %d, [GPIO %d]", 1, p1);
        Serial.println(line);
    }
    if (p2) {
        memset(line, 0, 128);
        sprintf(line,"button pressed: %d, [GPIO %d]", 2, p2);
        Serial.println(line);
    }

    if (value != last) {
        last = value;
        memset(line, 0, 128);
        sprintf(line,"encoder value: %d", value);
        Serial.println(value);
    }

    ClickEncoderI2C::Button b = encoder->getButton();
    if (b != ClickEncoderI2C::Open) {
        Serial.print("Button: ");
        #define VERBOSECASE(label) case label: Serial.println(#label); break;
        switch (b) {
        VERBOSECASE(ClickEncoderI2C::Pressed);
        VERBOSECASE(ClickEncoderI2C::Held)
        VERBOSECASE(ClickEncoderI2C::Released)
        VERBOSECASE(ClickEncoderI2C::Clicked)
        case ClickEncoderI2C::DoubleClicked:
            Serial.println("ClickEncoderI2C::DoubleClicked");
            encoder->setAccelerationEnabled(!encoder->getAccelerationEnabled());
            Serial.print("  Acceleration is ");
            Serial.println((encoder->getAccelerationEnabled()) ? "enabled" : "disabled");
            break;
        }
    }    

    #endif
    delay(100);
}


