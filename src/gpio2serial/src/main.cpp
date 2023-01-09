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

#define RELEASE 1
// #define DEBUG 1

#include <Arduino.h>
#include <AltSoftSerial.h>
#include "bltools.h"

#define DELAY_TIME     50               /* time to wait in the loop() */

// on nano, PIN_RX = 8, PIN_tx 9 always.
#define PIN_RX 8
#define PIN_TX 9

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

// NANO->A2-------------------------------------------------------------S2<-2W [2]
// NANO->A3-------------------------------------------------------------S1<-2W
// NANO->A4-------------------------------------------------------------S2<-2W [1]
// NANO->A5-------------------------------------------------------------S1<-2W

unsigned char INPUT_PINS[] = {  0,  0,  2,  3,  4,  5,  6,  7, 
                                0,  0, 10, 11, 12,  0, A0, A1, 
                               A2, A3, A4, A5,  0 , 0,  0,  0 ,
                                0,  0,  0,  0,  0 , 0,  0,  0  };


/**
 * @brief maps the selector signal to a number
 * 
 * @param buttons 
 * @return unsigned char 
*/
unsigned char selector_map(unsigned long buttons) {

    //
    // bits in the signals are no consecutive, due the 
    // nature of pins in the NANO, so I have to manage
    // the signal bitmap and build a "normalized"
    // integer. 0 means spureus contact, so just skip
    // it.

    word SELECTOR_value = buttons & 0xFFFFUL; // 16 first bytes
    SELECTOR_value = SELECTOR_value >> 2; // reduce the value
    // to get the number of bits used to generate the number.
    //word bits = (log (SELECTOR_value) / log (2));
    
    unsigned char ret;
    switch (SELECTOR_value) {
        case 1:    ret = 1;  break;
        case 2:    ret = 2;  break;
        case 4:    ret = 3;  break;
        case 8:    ret = 4;  break;
        case 16:   ret = 5;  break;
        case 32:   ret = 6;  break;
        case 256:  ret = 7;  break;
        case 512:  ret = 8;  break;
        case 1024: ret = 9;  break;
        case 4096: ret = 10; break;
        case 8192: ret = 11; break;
        default:   ret =  0; break;
    }
    return ret;
}

// reform the byte to send using a single byte:
// first 8 bits: coded the rotary position (1-16) only 11 used
// last 4 bits, encode the switch positions
// 7               0
// x x x x [ y y y y ]
// 
// x x x x: switch positions (bitmap)
// y y y y: position encoded (number)
//
void setup() {
     /// @brief  init all the devices as INPUT_PULLUP, so if 0, are active


     for (int i=0; i< NUM_INPUTS; i++) {
        if (INPUT_PINS[i] != 0) {
            #ifndef RELEASE
                memset(line, 0, 128);
                sprintf(line,"Init pin: %d [GPIO %d]",i, (int)INPUT_PINS[i]);
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
    delay(100); // wait for ESP32 so the ESP32 begin init before us
}


void loop() {

    for (int i=0; i< NUM_INPUTS; i++) {
        if (INPUT_PINS[i] != 0) {
            int input_value = digitalRead(INPUT_PINS[i]);
            input_value = ( input_value == 0 ? 1 : 0);  // PULLUP config, 0 is active (GND)
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





    // encode the data:
    // lower 4 bits are the selector position.
    uint8_t SELECTOR_value = selector_map(BUTTONS);
    if (SELECTOR_value == 0) {
        SELECTOR_value = selector_map(PREV_BUTTONS);
    }
    // process the 4 bits.
    // #define S_SW2_2_S2  16 (bit 4)
    // #define S_SW2_2_S1  17 (bit 5)
    // #define S_SW2_1_S2  18 (bit 6)
    // #define S_SW2_1_S1  19 (bit 7)
    
    uint8_t pin = 16;
    for (int pos=4; pos<8; pos++) {
        uint8_t mask = (uint8_t)1 << pos; 
        uint8_t value = get_bit(pin);
        SELECTOR_value =  (SELECTOR_value & ~mask) |  (value << pos);
        pin++;
    }
    if (PREV_BUTTONS != BUTTONS) {
        altSerial.write( (uint8_t *) &SELECTOR_value, 1 );
        PREV_BUTTONS = BUTTONS;
        #ifndef RELEASE
            print_state();
            memset(line, 0, LINE_SIZE);
            sprintf(line,"data: %x",SELECTOR_value);
            Serial.println(line);
        #endif
    }
    delay(DELAY_TIME);
}

