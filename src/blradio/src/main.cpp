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

// #define RELEASE 1
//#define DEBUG 1

#include <Arduino.h>
#include <PCF8574.h>
#include <Ticker.h>
#include <HardwareSerial.h>
#include <Keypad.h>        // https://github.com/Chris--A/Keypad

#include "rotencoder.h"
#include "bltools.h"

Ticker periodicTicker;

#define DELAY_TIME 50
#define EXPANDER_ADDESS 0x23
PCF8574 pcf8574(EXPANDER_ADDESS);

HardwareSerial DriverPort( 1 );

// pins for the extra rotary encoder
#define PIN_ROT5_A 16
#define PIN_ROT5_B 26
// pins for the serial port (only use the TX)
#define PIN_SERIAL_RX 27
#define PIN_SERIAL_TX 26
// pins for the 1-way switches
#define PIN_SW1_1_S1 15
#define PIN_SW1_2_S1 14
#define PIN_SW1_3_S1 12
#define PIN_SW1_4_S1 5
#define PIN_SW1_5_S1 4
#define PIN_SW1_6_S1 2

// Keypad row & col PINS
#define PIN_KP_ROW_0 13
#define PIN_KP_ROW_1 25
#define PIN_KP_ROW_2 33
#define PIN_KP_ROW_3 32

#define PIN_KP_COL_0 17
#define PIN_KP_COL_1 18
#define PIN_KP_COL_2 19
#define PIN_KP_COL_3 23

// use the pin definitions for each service.

#define PIN_RX PIN_SERIAL_RX
#define PIN_TX PIN_SERIAL_TX

// define the signals that comes from the NANO
// via serial port

#define S_1P12T_1    0
#define S_1P12T_2    1
#define S_1P12T_3    2
#define S_1P12T_4    3
#define S_1P12T_5    4
#define S_1P12T_6    5
#define S_1P12T_7    6
#define S_1P12T_8    7
#define S_1P12T_9    8
#define S_1P12T_10   9
#define S_1P12T_11  10

#define S_SW2_1_S1  11
#define S_SW2_1_S2  12
#define S_SW2_2_S1  13
#define S_SW2_2_S2  14

// maps the position on BUTTONS (bits) to signals
unsigned char NANO_BITS[] {
    2,    //S_1P12T_1 
    3,    //S_1P12T_2 
    4,    //S_1P12T_3 
    5,    //S_1P12T_4 
    6,    //S_1P12T_5 
    7,    //S_1P12T_6 
   10,    //S_1P12T_7 
   11,    //S_1P12T_8 
   12,    //S_1P12T_9 
   14,    //S_1P12T_10
   15,    //S_1P12T_11
   16,    //S_SW2_1_S1
   17,    //S_SW2_1_S2
   18,    //S_SW2_2_S1
   19     //S_SW2_2_S2
};


// ESP 1way Switches
#define S_SW1_6_S1  15  // G2
#define S_SW1_5_S1  16  // G4
#define S_SW1_4_S1  17  // G5
#define S_SW1_3_S1  18  // G12
#define S_SW1_2_S1  19  // G14
#define S_SW1_1_S1  20  // G15

// keypad matrix.


// rotary definitions
#define MAX_ROTARIES 5
RotaryEncoder *encoders[MAX_ROTARIES];
#define ROT5_PINA PIN_ROT5_A
#define ROT5_PINB PIN_ROT5_B // THE SAME as PIN_TX (see setup())
#define ROT1    0
#define ROT2    1
#define ROT3    2
#define ROT4    3
#define ROT5    4

// define the keypad matrix

#define KP_ROWS 4
#define KP_COLS 4



uint8_t kp_rowPins[KP_ROWS] = {PIN_KP_ROW_0, PIN_KP_ROW_1, PIN_KP_ROW_2, PIN_KP_ROW_3};
uint8_t kp_colPins[KP_COLS] = {PIN_KP_COL_0, PIN_KP_COL_1, PIN_KP_COL_2, PIN_KP_COL_3};


uint8_t keymap[KP_ROWS][KP_COLS] = {
    { 6,  5,   4,  3},
    {10,  9,   8,  7},
    {15,  14, 13, 12},
    { 2,  1,  11, 16}   // 16 last one not phisically connected.
};
Keypad customKeypad = Keypad(makeKeymap(keymap), kp_rowPins, kp_colPins, KP_ROWS, KP_COLS);


#define MAX_SIGNALS 21
unsigned char SIGNALS[MAX_SIGNALS] = { 0 };
unsigned char PREV_SIGNALS[MAX_SIGNALS] = { 0 };


//
// refresh the encoders to get the value.
//
void timer_isr() {
    for (int i=0; i< MAX_ROTARIES; i++) {
        encoders[i]->service();
    }
}

#ifndef RELEASE
void print_signals() {
    Serial.println("\n----------------------------");
    for (int i=0; i<MAX_SIGNALS; i++) {
        memset(line, 0, LINE_SIZE);
        sprintf(line, "SIGNAL_%d: %d", i, SIGNALS[i]);
        Serial.println(line);
    }
}
#endif 


void pressKey(uint8_t key) {
    // if (bleDevice.isConnected()) {
    //     Gamepad.press(key);
    // }
    #ifndef RELEASE
        memset(line, 0, LINE_SIZE);
        sprintf(line,"key pressed: %d", key);
        Serial.println(line);
    #endif
}

void releaseKey(uint8_t key) {
    // if (bleDevice.isConnected()) {
    //     Gamepad.release(key);
    // }
    #ifndef RELEASE
        memset(line, 0, LINE_SIZE);
        sprintf(line,"key released: %d", key);
        Serial.println(line);
    #endif

}

void keypadEvent(KeypadEvent key) {
    uint8_t keystate = customKeypad.getState();
    if (keystate == PRESSED) {
        pressKey(key);
        
    }
    if (keystate == RELEASED) {
        releaseKey(key);
    }
}







void setup() {
    #ifndef RELEASE	  
    Serial.begin (921600);
    Serial.println("i2cTEST Started");
    #endif
    // serial port
    DriverPort.begin(SERIAL_SPEED, SERIAL_8N1, PIN_RX, PIN_TX );
    delay(500);

    // switches
    pinMode(PIN_SW1_1_S1,INPUT_PULLUP);
    pinMode(PIN_SW1_2_S1,INPUT_PULLUP);
    pinMode(PIN_SW1_3_S1,INPUT_PULLUP);
    pinMode(PIN_SW1_4_S1,INPUT_PULLUP);
    pinMode(PIN_SW1_5_S1,INPUT_PULLUP);
    pinMode(PIN_SW1_6_S1,INPUT_PULLUP);

    // encoders
    encoders[ROT1] = new RotaryEncoder(&pcf8574, P0, P1, -1);  // reversed, so change wires [due bad soldering]
    encoders[ROT2] = new RotaryEncoder(&pcf8574, P2, P3, -1);
    encoders[ROT3] = new RotaryEncoder(&pcf8574, P4, P5, -1);
    encoders[ROT4] = new RotaryEncoder(&pcf8574, P6, P7, -1); 
    encoders[ROT5] = new RotaryEncoder(ROT5_PINA, ROT5_PINB, -1); // reversed, so change wires [due bad soldering]

    //
    // Set low latency with this method or uncomment LOW_LATENCY define in the library
    // Needed for i2c encoders
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

    // the keypad matrix
    customKeypad.addEventListener(keypadEvent);
    
}



long prev_value = 0;

void loop() {

    customKeypad.getKey();  // READ BUTTON MATRIX (EVENT CALLBACK SETUP)
    PREV_BUTTONS = BUTTONS;
    memcpy(PREV_SIGNALS, SIGNALS, MAX_SIGNALS);

    // 
    // read the data from serial This must be encapsulated in a function.
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
        #ifdef DEBUG
        //print_state();
        #endif
        #endif

        SIGNALS[S_1P12T_1 ] = get_bit(NANO_BITS[S_1P12T_1 ]);
        SIGNALS[S_1P12T_2 ] = get_bit(NANO_BITS[S_1P12T_2 ]);
        SIGNALS[S_1P12T_3 ] = get_bit(NANO_BITS[S_1P12T_3 ]);
        SIGNALS[S_1P12T_4 ] = get_bit(NANO_BITS[S_1P12T_4 ]);
        SIGNALS[S_1P12T_5 ] = get_bit(NANO_BITS[S_1P12T_5 ]);
        SIGNALS[S_1P12T_6 ] = get_bit(NANO_BITS[S_1P12T_6 ]);
        SIGNALS[S_1P12T_7 ] = get_bit(NANO_BITS[S_1P12T_7 ]);
        SIGNALS[S_1P12T_8 ] = get_bit(NANO_BITS[S_1P12T_8 ]);
        SIGNALS[S_1P12T_9 ] = get_bit(NANO_BITS[S_1P12T_9 ]);
        SIGNALS[S_1P12T_10] = get_bit(NANO_BITS[S_1P12T_10]);
        SIGNALS[S_1P12T_11] = get_bit(NANO_BITS[S_1P12T_11]);
        SIGNALS[S_SW2_1_S1] = get_bit(NANO_BITS[S_SW2_1_S1]);
        SIGNALS[S_SW2_1_S2] = get_bit(NANO_BITS[S_SW2_1_S2]);
        SIGNALS[S_SW2_2_S1] = get_bit(NANO_BITS[S_SW2_2_S1]);
        SIGNALS[S_SW2_2_S2] = get_bit(NANO_BITS[S_SW2_2_S2]);
    }

    // 
    // read the switches
    //
    SIGNALS[S_SW1_1_S1] = ( digitalRead(PIN_SW1_1_S1) == 0 ? 0 : 1 );
    SIGNALS[S_SW1_2_S1] = ( digitalRead(PIN_SW1_2_S1) == 0 ? 0 : 1 );
    SIGNALS[S_SW1_3_S1] = ( digitalRead(PIN_SW1_3_S1) == 0 ? 0 : 1 );
    SIGNALS[S_SW1_4_S1] = ( digitalRead(PIN_SW1_4_S1) == 0 ? 0 : 1 );
    SIGNALS[S_SW1_5_S1] = ( digitalRead(PIN_SW1_5_S1) == 0 ? 0 : 1 );
    SIGNALS[S_SW1_6_S1] = ( digitalRead(PIN_SW1_6_S1) == 0 ? 0 : 1 );



    if (memcmp(PREV_SIGNALS, SIGNALS, MAX_SIGNALS) != 0) {
        print_signals();
    }


    // the IC part here.
    //uint8_t p0 = pcf8574.digitalRead(ROT5_PINA);
    //uint8_t p1 = pcf8574.digitalRead(ROT5_PINB);
    //uint8_t p2 = pcf8574.digitalRead(P2);
    //uint8_t p6 = pcf8574.digitalRead(P6);
    //uint8_t p7 = pcf8574.digitalRead(P7);
    //p0 = ( p0 == 0 ? 1 : 0); // not wired
    //p1 = ( p1 == 0 ? 1 : 0);
    //p2 = ( p2 == 0 ? 1 : 0); 
    //p6 = ( p6 == 0 ? 1 : 0); 
    // p7 = ( p7 == 0 ? 1 : 0); 

    // not connected in hw
    // if (p0) {
    //     memset(line, 0, 128);
    //     sprintf(line,"button pressed: %d, [GPIO %d]", 0, P0);
    //     Serial.println(line);
    // }
    // if (p1) {
    //     memset(line, 0, 128);
    //     sprintf(line,"button pressed: %d, [GPIO %d]", 1, P1);
    //     Serial.println(line);
    // }
    // if (p2) {
    //     memset(line, 0, 128);
    //     sprintf(line,"button pressed: %d, [GPIO %d]", 2, P2);
    //     Serial.println(line);
    // }
    // if (p6) {
    //     memset(line, 0, 128);
    //     sprintf(line,"button pressed: %d, [GPIO %d]", 6, P6);
    //     Serial.println(line);
    // }
    // if (p7) {
    //     memset(line, 0, 128);
    //     sprintf(line,"button pressed: %d, [GPIO %d]", 7, P7);
    //     Serial.println(line);
    // }

    for (int i=0; i< MAX_ROTARIES; i++) {
        if (encoders[i]->getButton() == RotaryEncoder::PRESSED) {
            #ifndef RELEASE
                memset(line, 0, 128);
                sprintf(line,"encoder #%d button pressed", i+1);
                Serial.println(line);
            #endif
        }

        switch (encoders[i]->getDirection()) {
            case RotaryEncoder::RIGHT:
                #ifndef RELEASE
                    memset(line, 0, 128);
                    sprintf(line,"encoder #%d CW (Right)", i+1);
                    Serial.println(line);
                #endif
                break;
            case RotaryEncoder::LEFT:
                #ifndef RELEASE
                    memset(line, 0, 128);
                    sprintf(line,"encoder #%d CCW (Left)", i+1);
                    Serial.println(line);
                #endif
                break;
        }
    }
    // maybe it's going to be removed
    delay(DELAY_TIME);
}


