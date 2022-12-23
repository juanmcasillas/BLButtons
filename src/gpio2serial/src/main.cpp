//////////////////////////////////////////////////////////////////////////////
///
/// @file main.cpp
/// @author Juan M. Casillas (juanm.casillas@gmail.com)
/// @brief Read data from GPIO, send them from the UART.
/// @version 0.1
/// @date 2022/12/22
///
/// Read data from the GPIO, send them using the UART (Serial)
/// Useful to extend the de available inputs using only 2 GPIO.
/// 
/// @copyright Copyright (c) 12/22/2022 Juan M. Casillas <juanm.casillas@gmail.com>
///
//////////////////////////////////////////////////////////////////////////////
// https://stackoverflow.com/questions/47981/how-do-i-set-clear-and-toggle-a-single-bit
// https://www.pjrc.com/teensy/td_libs_AltSoftSerial.html
// installed from Library Manager
// on nano, rx 8, tx 9 allways.
// this is the part from NANO (sender)

//#define RELEASE 1
// #define DEBUG 1

#include <Arduino.h>
#include <AltSoftSerial.h>
#include "bltools.h"

AltSoftSerial altSerial;
#define NUM_INPUTS 32

// POS in BUTTONS map (0-32) if 0, don't configure it
// unsigned char INPUT_PINS[] = {  0,  0,  2,  3,  4,  5,  6,  7, 
//                                 0,  0, 10, 11, 12, 13, A0, A1, 
//                                A2, A3, A4, A5,  0 , 0,  0,  0 ,
//                                 0,  0,  0,  0,  0 , 0,  0,  0  };
// The analog input pins can be used as digital pins, referred to as A0, A1, etc. 
// The exception is the Arduino Nano, Pro Mini, and Mini’s A6 and A7 pins, which 
// can only be used as analog inputs.

// so I ended with 15 inputs with 2 pins, not bad.

unsigned char INPUT_PINS[] = {  0,  0,  2,  3,  4,  5,  6,  7, 
                                0,  0, 10, 11, 12,  0, A0, A1, 
                               A2, A3, A4, A5,  0 , 0,  0,  0 ,
                                0,  0,  0,  0,  0 , 0,  0,  0  };


void setup() {
     /// @brief  init all the devices as INPUT_PULLUP, so if 0, are active

     for (int i=0; i< NUM_INPUTS; i++) {
        if (INPUT_PINS[i] != 0) {
            #ifndef RELEASE
                memset(line, 0, 128);
                sprintf(line,"Init pin: %d [GPIO %d]",INPUT_PINS[i]);
                Serial.println(line);
            #endif
            pinMode(INPUT_PINS[i], INPUT_PULLUP);        
        }
    }
    BUTTONS = 0;
    // force send first time.
    PREV_BUTTONS = ! BUTTONS;

#ifndef RELEASE
    Serial.begin(115200);
    Serial.println("NanoWriter Started - writting on D9");
#endif
    altSerial.begin(SERIAL_SPEED);
    delay(500);    
}


void loop() {
 
   
    
    for (int i=0; i< NUM_INPUTS; i++) {
        if (INPUT_PINS[i] != 0) {
            int input_value = digitalRead(INPUT_PINS[i]);
            input_value = ( input_value == 0 ? 1 : 0);  // PULLUP, 0 is active (GND)
            set_bit(i, input_value);

            #ifndef RELEASE
            #ifdef DEBUG
                if (input_value) {
                    memset(line, 0, 128);
                    sprintf(line,"button pressed: %d, [GPIO %d]", i, INPUT_PINS[i]);
                    Serial.println(line);
                    Serial.println(BUTTONS);
                }
            #endif
            #endif
        }
    }

    // if buttons changed, send them
    if (PREV_BUTTONS != BUTTONS) {
        #ifndef RELEASE
        print_state();
        #endif
        PACKET.data = BUTTONS;
        altSerial.write( (uint8_t *) &PACKET, sizeof( PACKET ) ); // this says 12.
        PREV_BUTTONS = BUTTONS;
    }
    delay(100);
}

