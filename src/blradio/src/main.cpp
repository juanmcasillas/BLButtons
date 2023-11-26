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

// define this to support multiple keyscan press (see below)
// #define ENABLE_KEYBOARD_MAP 1
#define RELEASE 1                 /* define this to remove all debug data */
#define DEBUG_SERIAL  0x1
#define DEBUG_SIGNALS 0x2
#define DEBUG_BUTTON  0x4
#define DEBUG_STATE   0x8
#define DEBUG_KEYS   0x10
#define DEBUG_ROTENC 0x20
#define DEBUG_ALL    0xFF
// #define DEBUG DEBUG_ALL
// #define DEBUG DEBUG_SERIAL | DEBUG_BUTTON
// #define DEBUG DEBUG_SERIAL | DEBUG_SIGNALS               /* define this to debug extra info */
// #define TESTING 1                                        /* undefine this to use the real BL stack. Clean All and build */


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

unsigned char rotenc_map[MAX_ROTARIES][2] = {
    { S_ROT_1_LEFT, S_ROT_1_RIGHT },
    { S_ROT_2_LEFT, S_ROT_2_RIGHT },
    { S_ROT_3_LEFT, S_ROT_3_RIGHT },
    { S_ROT_4_LEFT, S_ROT_4_RIGHT },
    { S_ROT_5_LEFT, S_ROT_5_RIGHT },
};

// 
// define the signals for the mode selector
//

#define S_MODE_1      47
#define S_MODE_2      48
#define S_MODE_3      49
#define S_MODE_4      50
#define S_MODE_5      51
#define S_MODE_6      52
#define S_MODE_7      53
#define S_MODE_8      54
#define MAX_MODES      8

unsigned char mode_map[MAX_MODES] = {
    S_MODE_1,
    S_MODE_2,
    S_MODE_3,
    S_MODE_4,
    S_MODE_5,
    S_MODE_6,
    S_MODE_7,
    S_MODE_8
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
    S_KP_12 ,   // rotenc 4
    S_KP_13 ,   // rotenc 3
    S_KP_14 ,   // rotenc 2
    S_KP_15 ,   // rotenc 1
    S_KP_16 // not used
};

// 
// signal container
//
#define MAX_SIGNALS 56
unsigned char SIGNALS[MAX_SIGNALS]      = { 0 };
unsigned char PREV_SIGNALS[MAX_SIGNALS] = { 0 };

// 
// data (single serial byte)
//
uint8_t DATA;



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

15, // S_SW1_1_S1
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
25, // S_KP_12     // ROT1
28, // S_KP_13     // ROT2
31, // S_KP_14     // ROT3
34, // S_KP_15     // ROT4

0, // S_KP_16     // not used.

//
// rotenc buttons (left, right)
// 

33, // S_ROT_4_LEFT  
35, // S_ROT_4_RIGHT 
30, // S_ROT_3_LEFT  
32, // S_ROT_3_RIGHT 
27, // S_ROT_2_LEFT  
29, // S_ROT_2_RIGHT 
24, // S_ROT_1_LEFT  
26, // S_ROT_1_RIGHT 
21, // S_ROT_5_LEFT  
23, // S_ROT_5_RIGHT

// 
// the mode selector now sends buttons
// FIRST_MODE_BUTTON to avoid mapping over

121, // S_MODE_1
122, // S_MODE_2
123, // S_MODE_3
124, // S_MODE_4
125, // S_MODE_5
126, // S_MODE_6
127, // S_MODE_7
128  // S_MODE_8

};

//
// create a keyboard map. If the signal is mapped here (!= 0)
// then invoke the keypress using the keyboard interface.
// useful for multiple keymapping (you know, obs)
// define ENABLE_KEYBOARD_MAP for support this (see top)



// if keyboard chord is enabled, keys are send with
// to avoid overlapping. Chord can be only 2 keys
// KEY_RIGHT_ALT + KEY_RIGHT_CTRL + KEY
#define KEYBOARD_CHORD 1

unsigned char KEYBOARD_MAP[] = {
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
KEYPAD_DIVIDE, // S_SW2_1_S1 mapped to signal 11 (position=11) mapped to KEYPAD_DIVIDE      // this is position 11 (signal == 11)
KEYPAD_MULTIPLY, // S_SW2_1_S2 mapped to signal 12 (position=12) mapped to KEYPAD_MULTIPLY  // this is position 12 (signal == 12)
KEYPAD_SUBTRACT, // S_SW2_2_S1 mapped to signal 13 (position=13) mapped to KEYPAD_SUBTRACT  // this is position 13 (signal == 13)
KEYPAD_ADD, // S_SW2_2_S2 mapped to signal 14 (position=14) mapped to KEYPAD_ADD            // this is position 14 (signal == 14)
0, // S_SW1_1_S1
0, // S_SW1_2_S1
0, // S_SW1_3_S1
0, // S_SW1_4_S1 
0, // S_SW1_6_S1  // bad wiring
0, // S_SW1_5_S1 
0,  // S_KP_1   
0,  // S_KP_2   
0,  // S_KP_3   
0,  // S_KP_4   
0,  // S_KP_5   
0,  // S_KP_6   
0,  // S_KP_7   
0,  // S_KP_8   
0,  // S_KP_9   
0, // S_KP_10
0, // S_KP_11     // ROT5
0, // S_KP_12     // ROT1
0, // S_KP_13     // ROT2
0, // S_KP_14     // ROT3
0, // S_KP_15     // ROT4
0, // S_KP_16     // not used.
0, // S_ROT_4_LEFT  
0, // S_ROT_4_RIGHT 
0, // S_ROT_3_LEFT  
0, // S_ROT_3_RIGHT 
0, // S_ROT_2_LEFT  
0, // S_ROT_2_RIGHT 
0, // S_ROT_1_LEFT  
0, // S_ROT_1_RIGHT 
0, // S_ROT_5_LEFT  
0, // S_ROT_5_RIGHT
0, // S_MODE_1
0, // S_MODE_2
0, // S_MODE_3
0, // S_MODE_4
0, // S_MODE_5
0, // S_MODE_6
0, // S_MODE_7
0  // S_MODE_8
};


#define FIRST_MODE_BUTTON 121


uint8_t SELECTOR;                // manages the state of the central selector (helpers.h)
uint8_t PREV_SELECTOR;

Ticker periodicTicker;
#define DELAY_TIME 50           // time to wait in the loop()
#define EXPANDER_ADDESS 0x23
PCF8574 pcf8574(EXPANDER_ADDESS);

HardwareSerial DriverPort( 1 );

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
 * @brief check if the signal has a keyboard mapping defined. Don't count the selector mode.
 * 
 * @param signal_num 
 * @return uint8_t 0 if not, else the value of the key being pressed.
*/

uint8_t keyboard_mapping(uint8_t signal_num) {
    uint8_t value = BUTTON_MAP[signal_num];
    // use only fixed buttons to map keyboards,
    // not use mode selector buttons.
    return ( value == 0 || value >  LAST_FIXED_BUTTON ? 0 : KEYBOARD_MAP[value] );
    
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

    if (button_map > LAST_FIXED_BUTTON && button_map < FIRST_MODE_BUTTON) {
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
            #ifdef ENABLE_KEYBOARD_MAP
                uint8_t key_map = keyboard_mapping(i);
            #endif
            if (button_map == 0) {
                continue;
            }

            if (button_value) {
                #if !defined(RELEASE) && (DEBUG & DEBUG_BUTTON)
                memset(line, 0, LINE_SIZE);
                sprintf(line, "Button press %d [selector: %x]", button_map, SELECTOR);
                Serial.println(line);
                if (key_map) {
                    memset(line, 0, LINE_SIZE);
                    sprintf(line, "Button press %d key_press %d", button_map, key_map);
                    Serial.println(line);
                }
                #endif
                
                #ifndef TESTING
                    Gamepad.press(button_map);
                    #ifdef ENABLE_KEYBOARD_MAP
                    if (key_map != 0) {
                        //
                        #ifdef KEYBOARD_CHORD
                            Keyboard.press(KEY_RIGHT_ALT);
                            Keyboard.press(KEY_RIGHT_CTRL);
                        #endif
                        Keyboard.press(key_map);
                        Keyboard.releaseAll();
                    }
                    #endif
                #endif

                

            }
            else {
                #if !defined(RELEASE) && (DEBUG & DEBUG_BUTTON)
                memset(line, 0, LINE_SIZE);
                sprintf(line, "Button release %d [selector: %x]", button_map, SELECTOR);
                Serial.println(line);
                if (key_map) {
                    memset(line, 0, LINE_SIZE);
                    sprintf(line, "Button release %d key_press %d", button_map, key_map);
                    Serial.println(line);
                }                
                #endif
                #ifndef TESTING
                    Gamepad.release(button_map);
                    // not needed, only send the keyboard press and release them quickly
                    // #ifdef ENABLE_KEYBOARD_MAP
                    // if (key_map != 0) {
                    //     #ifdef KEYBOARD_CHORD
                    //         Keyboard.release(KEY_RIGHT_ALT);
                    //         Keyboard.release(KEY_RIGHT_CTRL);
                    //     #endif
                    //     Keyboard.release(key_map);
                    // }
                    // #endif
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
    #if (!defined(RELEASE) && (DEBUG & DEBUG_KEYS))
        memset(line, 0, LINE_SIZE);
        sprintf(line,"key pressed: %d", key);
        Serial.println(line);
    #endif
    SIGNALS[keymap_signals[key]] = 1;
}

/**
 * @brief handler for manage a button release in the keypad
 * 
 * @param key 
*/
void releaseKey(uint8_t key) {
    #if (!defined(RELEASE) && (DEBUG & DEBUG_KEYS))
        memset(line, 0, LINE_SIZE);
        sprintf(line,"key released: %d", key);
        Serial.println(line);
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


#if !defined(RELEASE) && (DEBUG & DEBUG_SIGNALS)
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
    Serial.begin (115200);
    Serial.println("BL-Radio Started");
    #endif
    // serial port
    delay(1000);
    DriverPort.begin(SERIAL_SPEED, SERIAL_8N1, PIN_RX, PIN_TX );
    


    
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

    // the bluetooth things
    #ifndef TESTING
        Keyboard.begin();
        Mouse.begin();
        Gamepad.begin();
    #endif


    periodicTicker.attach_ms(1, timer_isr);

    // selector init mode
    SELECTOR = 1;
    PREV_SELECTOR = 0;

    // the keypad matrix
    customKeypad.addEventListener(keypadEvent);

    // the computer takes some time to work
    // so wait here before all the BLE is working
    //

}
















/**
 * @brief do the things. First, save the state, then read form the
 * serial port (selector, some switches) and then build the object
 * and send it using BLE. 
 * 
*/


#define DELAY_PERIOD 2000 
unsigned long timestamp = millis();
bool waitting_delay = true;

void loop() {

    waitting_delay = true;
    if (millis() - timestamp > DELAY_PERIOD) {
        waitting_delay = false;
        timestamp = millis();
    }

    //
    // read the keypad custom matrix for  buttons
    // 
    customKeypad.getKey();  

    // 
    // read the data from serial This must be encapsulated in a function.
    // 

    if ( DriverPort.available() ) {
        DATA = DriverPort.read();
        SIGNALS[S_SW2_2_S2] = ( (DATA >> 4) & 1UL ? 1 : 0);
        SIGNALS[S_SW2_2_S1] = ( (DATA >> 5) & 1UL ? 1 : 0);
        SIGNALS[S_SW2_1_S2] = ( (DATA >> 6) & 1UL ? 1 : 0);
        SIGNALS[S_SW2_1_S1] = ( (DATA >> 7) & 1UL ? 1 : 0);
        uint8_t SELECTOR_value = DATA & 15; // lower value;
        SELECTOR = SELECTOR_value;

        for (int i=0; i< MAX_MODES; i++) {
            unsigned char signal_id = mode_map[i];
            unsigned char signal_value = ( SELECTOR-1 == i ? 1 : 0 );
            SIGNALS[signal_id] = signal_value;
        }

        #if !defined(RELEASE) && (DEBUG & DEBUG_SERIAL)
            memset(line,0, LINE_SIZE);
            sprintf(line,"data: %x", DATA);
            Serial.println(line);
        #endif
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
                #if !defined(RELEASE) && (DEBUG & DEBUG_ROTENC)
                    memset(line, 0, 128);
                    sprintf(line,"encoder #%d CW (Right)", i+1);
                    Serial.println(line);
                #endif
                SIGNALS[rotenc_map[i][ROT_RIGHT]] = 1;
                break;
            case RotaryEncoder::LEFT:
                #if !defined(RELEASE) && (DEBUG & DEBUG_ROTENC)
                    memset(line, 0, 128);
                    sprintf(line,"encoder #%d CCW (Left)", i+1);
                    Serial.println(line);
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
    if ((memcmp(PREV_SIGNALS, SIGNALS, MAX_SIGNALS) != 0) || (SELECTOR != PREV_SELECTOR) || (!waitting_delay)) {
        #if !defined(RELEASE) && (DEBUG & DEBUG_SIGNALS)
        print_signals();
        #endif
        send_buttons();
        PREV_SELECTOR = SELECTOR;
        memcpy(PREV_SIGNALS, SIGNALS, MAX_SIGNALS);
    }


}


