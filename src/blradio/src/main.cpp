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

#define RELEASE 1

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
#include "pindefs.h"
#include "signaldefs.h"
#include "buttonmap.h"
#include "helpers.h"

//word SELECTOR;                // manages the state of the central selector (helpers.h)
  
Ticker periodicTicker;
#define DELAY_TIME 50           // time to wait in the loop()
#define EXPANDER_ADDESS 0x23
PCF8574 pcf8574(EXPANDER_ADDESS);

HardwareSerial DriverPort( 1 );

/**
 * @brief setup the things. Check the delay to wait for the arduino
 * 
*/
void setup() {

    Serial.begin (921600);
    Serial.println("BL-Radio Started");

    
    // serial port
    DriverPort.begin(SERIAL_SPEED, SERIAL_8N1, PIN_RX, PIN_TX );
    delay(500); // wait less than the arduino

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

    // selector init mode
    SELECTOR = 0;

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
}


