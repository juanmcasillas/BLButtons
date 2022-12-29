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
//
// PCF8574
// 
// https://www.luisllamas.es/mas-pines-digitales-con-arduino-y-el-expansor-es-i2c-pcf8574/
// use this https://github.com/xreef/PCF8574_library/releases
// https://github.com/khoih-prog/ESP32TimerInterrupt
//

#define RELEASE 1                  // define this to remove all debug data
//#define DEBUG 1                 // define this to debug extra info
//#define TESTING 1               // undefine this to use the real BL stack. Clean All and build


// see helpers.h for debug defines

#include <Arduino.h>
#include <PCF8574.h>            // https://github.com/xreef/PCF8574_library
#include <Ticker.h>
#include <HardwareSerial.h>
#include <Keypad.h>             // https://github.com/Chris--A/Keypad
    
#include <BleKeyboard.h>        // https://github.com/juanmcasillas/ESP32-BLE-Combo
#include <BleMouse.h>           // https://github.com/juanmcasillas/ESP32-BLE-Combo
#include <BleGamepad.h>         // https://github.com/juanmcasillas/ESP32-BLE-Combo

#include "rotencoder.h"
#include "bltools.h"



// 
// PIN DEFINITIONS
//

#define PIN_LED     2
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

//
// use the pin definitions for each service.
//

#define PIN_RX PIN_SERIAL_RX
#define PIN_TX PIN_SERIAL_TX

#define ROT5_PINA PIN_ROT5_A
#define ROT5_PINB PIN_ROT5_B // THE SAME as PIN_TX (see setup())

//
// define the signals that comes from the NANO via serial port
//
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

//
// maps the position on BUTTONS (bits) to signals
//
//
// maps the position on BUTTONS (bits) to signals
//
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
   16,    //S_SW2_2_S2 // bad wiring
   17,    //S_SW2_2_S1 // bad wiring
   18,    //S_SW2_1_S2 // bad wiring
   19     //S_SW2_1_S1 // bad wiring
};

//
// ESP 1way Switches
//
#define S_SW1_1_S1  15  // G15
#define S_SW1_2_S1  16  // G14
#define S_SW1_3_S1  17  // G12
#define S_SW1_4_S1  18  // G5
#define S_SW1_5_S1  19  // G4
#define S_SW1_6_S1  20  // G2

//
// define the keypad signals.
//
#define S_KP_1   21
#define S_KP_2   22
#define S_KP_3   23
#define S_KP_4   24
#define S_KP_5   25
#define S_KP_6   26
#define S_KP_7   27
#define S_KP_8   28
#define S_KP_9   29
#define S_KP_10  30
#define S_KP_11  31
#define S_KP_12  32
#define S_KP_13  33
#define S_KP_14  34
#define S_KP_15  35
#define S_KP_16  36

//
// define the rotary encoders buttons [left, right]
//
#define S_ROT_1_LEFT  37
#define S_ROT_1_RIGHT 38
#define S_ROT_2_LEFT  39
#define S_ROT_2_RIGHT 40
#define S_ROT_3_LEFT  41
#define S_ROT_3_RIGHT 42
#define S_ROT_4_LEFT  43
#define S_ROT_4_RIGHT 44
#define S_ROT_5_LEFT  45
#define S_ROT_5_RIGHT 46

//
// rotary definitions
//
#define MAX_ROTARIES 5
RotaryEncoder *encoders[MAX_ROTARIES];
#define ROT1    0
#define ROT2    1
#define ROT3    2
#define ROT4    3
#define ROT5    4
#define ROT_LEFT  0
#define ROT_RIGHT 1
unsigned char rotenc_map[MAX_ROTARIES][2] = {
    { S_ROT_1_LEFT, S_ROT_1_RIGHT },
    { S_ROT_2_LEFT, S_ROT_2_RIGHT },
    { S_ROT_3_LEFT, S_ROT_3_RIGHT },
    { S_ROT_4_LEFT, S_ROT_4_RIGHT },
    { S_ROT_5_LEFT, S_ROT_5_RIGHT },
};

//
// keypad matrix.
//
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

uint8_t keymap_signals[] = {
    0,      // not used, only for aligment
    S_KP_1  ,
    S_KP_2  ,
    S_KP_3  ,
    S_KP_4  ,
    S_KP_5  ,
    S_KP_6  ,
    S_KP_7  ,
    S_KP_8  ,
    S_KP_9  ,
    S_KP_10 ,
    S_KP_11 ,   // rotenc 5
    S_KP_12 ,   // rotenc 1
    S_KP_13 ,   // rotenc 2
    S_KP_14 ,   // rotenc 3
    S_KP_15 ,   // rotenc 4
    S_KP_16 // not used
};

// 
// signal container
//
#define MAX_SIGNALS 47
unsigned char SIGNALS[MAX_SIGNALS]      = { 0 };
unsigned char PREV_SIGNALS[MAX_SIGNALS] = { 0 };


// max buttons: 35 
// rotenc = 15 buttons. (12 counting only the last row)
#define LAST_FIXED_BUTTON 23 // the last button of the non movable buttons
unsigned char BUTTON_MAP[] = {
//
// NANO MODE.
// this is used as a mode selector, not a button
// so don't map it here. The number is the button
// number. 0 means no button, just for reference
// to map the table usign the signal ID.
//

0, // S_1P12T_1
0, // S_1P12T_2
0, // S_1P12T_3
0, // S_1P12T_4
0, // S_1P12T_5
0, // S_1P12T_6
0, // S_1P12T_7
0, // S_1P12T_8
0, // S_1P12T_9
0, // S_1P12T_10
0, // S_1P12T_11

// 
// NANO two way switches
//
11, // S_SW2_1_S1
12, // S_SW2_1_S2
13, // S_SW2_2_S1
14, // S_SW2_2_S2

// 
// ESP 32 one way switches
//

11, // S_SW1_1_S1
16, // S_SW1_2_S1
17, // S_SW1_3_S1
18, // S_SW1_4_S1 
20, // S_SW1_6_S1  // bad wiring
19, // S_SW1_5_S1 

//
// buttons
//
1,  // S_KP_1   
2,  // S_KP_2   
3,  // S_KP_3   
4,  // S_KP_4   
5,  // S_KP_5   
6,  // S_KP_6   
7,  // S_KP_7   
8,  // S_KP_8   
9,  // S_KP_9   
10, // S_KP_10

22, // S_KP_11     // ROT5
34, // S_KP_12     // ROT4
31, // S_KP_13     // ROT3
28, // S_KP_14     // ROT2
25, // S_KP_15     // ROT1

0, // S_KP_16     // not used.

//
// rotenc buttons (left, right)
// 

24, // S_ROT_1_LEFT  
26, // S_ROT_1_RIGHT 
27, // S_ROT_2_LEFT  
29, // S_ROT_2_RIGHT 
30, // S_ROT_3_LEFT  
32, // S_ROT_3_RIGHT 
33, // S_ROT_4_LEFT  
35, // S_ROT_4_RIGHT 
21, // S_ROT_5_LEFT  
23  // S_ROT_5_RIGHT 
};



word SELECTOR;                // manages the state of the central selector (helpers.h)
 
Ticker periodicTicker;
#define DELAY_TIME 50           // time to wait in the loop()
#define EXPANDER_ADDESS 0x23
PCF8574 pcf8574(EXPANDER_ADDESS);

HardwareSerial DriverPort( 1 );

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


/**
 * @brief process the rotary encoders periodically 1ms
 * periodicTicker.attach_ms(1, timer_isr);
 * 
*/
void timer_isr() {
    for (int i=0; i< MAX_ROTARIES; i++) {
        encoders[i]->service();
    }
}


/**
 * @brief do the buttom mapping using the selector.
 * 
 * first 23 buttons are constant (all the switches, pushbuttons and the ROT5 encoder (topmost))
 * lower rotary are changed based on selector.
 * with 23 + 12 (first 23 buttons + 12 buttons from ROTENCS) I have 8 available modes (with 8 buttons left)
 * so I will limit the rotary encoder using the hardware washer to allow only 8 modes, (losing 9, 10, 11) and
 * map the buttons. so
 * 
 * @param signal_num 
 * @return uint8_t 
*/
uint8_t button_mapping(uint8_t signal_num) {
    uint8_t button_map = BUTTON_MAP[signal_num];

    if (button_map > LAST_FIXED_BUTTON) {
        button_map = button_map + ((SELECTOR - 1) * 12);
    }
    return button_map;
}


/**
 * @brief send buttons to the BL device.
 * 
*/
void send_buttons() {

    #ifdef TESTING
    if (true) {
    #else
    if (bleDevice.isConnected()) {
    #endif

        for (uint8_t i=0; i<MAX_SIGNALS; i++) {
            uint8_t button_value = SIGNALS[i];
            uint8_t button_map = button_mapping(i);
            if (button_map == 0) {
                continue;
            }

            if (button_value) {
                #ifndef RELEASE
                memset(line, 0, LINE_SIZE);
                sprintf(line, "Press %d [selector: %x]", button_map, SELECTOR);
                Serial.println(line);
                #endif
                
                #ifndef TESTING
                    Gamepad.press(button_map);
                #endif
            }
            else {
                #ifndef RELEASE
                memset(line, 0, LINE_SIZE);
                sprintf(line, "Release %d [selector: %x]", button_map, SELECTOR);
                Serial.println(line);
                #endif
                #ifndef TESTING
                    Gamepad.release(button_map);
                #endif
            }
        }
        // I could use the POVS to map additional buttons.
        #ifndef TESTING
            Gamepad.setAxes(0, 0, 0, 0, 0, 0, 0, 0, DPAD_CENTERED, DPAD_CENTERED, DPAD_CENTERED, DPAD_CENTERED);
            //Gamepad.sendAllButtons();
        #endif
    }
}

/**
 * @brief handler for manage a button press in the keypad
 * 
 * @param key 
*/
void pressKey(uint8_t key) {
    #ifndef RELEASE
    #ifdef DEBUG
        memset(line, 0, LINE_SIZE);
        sprintf(line,"key pressed: %d", key);
        Serial.println(line);
    #endif
    #endif
    SIGNALS[keymap_signals[key]] = 1;
}

/**
 * @brief handler for manage a button release in the keypad
 * 
 * @param key 
*/
void releaseKey(uint8_t key) {
    #ifndef RELEASE
    #ifdef DEBUG
        memset(line, 0, LINE_SIZE);
        sprintf(line,"key released: %d", key);
        Serial.println(line);
    #endif
    #endif
    SIGNALS[keymap_signals[key]] = 0;
}


/**
 * @brief manage the keypad event
 * 
 * @param key 
*/
void keypadEvent(KeypadEvent key) {
    uint8_t keystate = customKeypad.getState();
    if (keystate == PRESSED) {
        pressKey(key);
        
    }
    if (keystate == RELEASED) {
        releaseKey(key);
    }
}


#ifndef RELEASE
/**
 * @brief print the signal contents to debug
 * 
*/
void print_signals() {
    Serial.println("\n----------------------------");
    for (int i=0; i<MAX_SIGNALS; i++) {
        memset(line, 0, LINE_SIZE);
        sprintf(line, "SIGNAL_%d: %d", i, SIGNALS[i]);
        Serial.println(line);
    }
}
#endif












/**
 * @brief setup the things. Check the delay to wait for the arduino
 * 
*/
void setup() {
    #ifndef RELEASE
    Serial.begin (921600);
    Serial.println("BL-Radio Started");
    #endif
    // serial port
    DriverPort.begin(SERIAL_SPEED, SERIAL_8N1, PIN_RX, PIN_TX );
    delay(500);

    // LED
    pinMode(PIN_LED, OUTPUT);
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
    
    if (pcf8574.begin()){
        #ifndef RELEASE	  
        Serial.println("pcf8574 lib ok");
        #endif
    }else{
        #ifndef RELEASE	  
        Serial.println("pcf8574 lib ko");
        #endif
    }

    periodicTicker.attach_ms(1, timer_isr);

    // selector init mode
    SELECTOR = 1;

    // the keypad matrix
    customKeypad.addEventListener(keypadEvent);

    // the bluetooth things
    #ifndef TESTING
        Keyboard.begin();
        Mouse.begin();
        Gamepad.begin();
    #endif
}
















/**
 * @brief do the things. First, save the state, then read form the
 * serial port (selector, some switches) and then build the object
 * and send it using BLE. 
 * 
*/

void loop() {

    PREV_BUTTONS = BUTTONS;
    memcpy(PREV_SIGNALS, SIGNALS, MAX_SIGNALS);

    //
    // read the keypad custom matrix for  buttons
    // 
    customKeypad.getKey();  

    // 
    // read the data from serial This must be encapsulated in a function.
    // 
    if ( DriverPort.available() >= PACKET_SIZE ) {
        uint8_t data[6]; // size of serial_package_s;
        digitalWrite(PIN_LED,HIGH);
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
        print_state();
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
        
        SIGNALS[S_SW2_1_S1] = get_bit(NANO_BITS[S_SW2_2_S2]);   // bad wiring. remap here.
        SIGNALS[S_SW2_1_S2] = get_bit(NANO_BITS[S_SW2_2_S1]);
        SIGNALS[S_SW2_2_S1] = get_bit(NANO_BITS[S_SW2_1_S2]);
        SIGNALS[S_SW2_2_S2] = get_bit(NANO_BITS[S_SW2_1_S1]);

        // process the selector.

        word SELECTOR_value = selector_map(BUTTONS);
        if (SELECTOR_value == 0) {
            // skip it because it's a bounce. Wait for the new
            goto end_loop;
        }
        SELECTOR = SELECTOR_value;
        Serial.println(SELECTOR);

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


    for (int i=0; i< MAX_ROTARIES; i++) {

        switch (encoders[i]->getDirection()) {
            case RotaryEncoder::RIGHT:
                #ifndef RELEASE
                #ifdef DEBUG
                    memset(line, 0, 128);
                    sprintf(line,"encoder #%d CW (Right)", i+1);
                    Serial.println(line);
                #endif
                #endif
                SIGNALS[rotenc_map[i][ROT_RIGHT]] = 1;
                break;
            case RotaryEncoder::LEFT:
                #ifndef RELEASE
                #ifdef DEBUG
                    memset(line, 0, 128);
                    sprintf(line,"encoder #%d CCW (Left)", i+1);
                    Serial.println(line);
                #endif
                #endif
                SIGNALS[rotenc_map[i][ROT_LEFT]] = 1;
                break;
            default:
                SIGNALS[rotenc_map[i][ROT_LEFT]] = 0;
                SIGNALS[rotenc_map[i][ROT_RIGHT]] = 0;
                break;
        }
    }

    // send the things here
    if (memcmp(PREV_SIGNALS, SIGNALS, MAX_SIGNALS) != 0) {
        #ifndef RELEASE
        #ifdef DEBUG
        print_signals();
        #endif
        #endif
        send_buttons();
    }

end_loop:
    // maybe it's going to be removed
    delay(DELAY_TIME);
    digitalWrite(PIN_LED,LOW); // turn off
}


