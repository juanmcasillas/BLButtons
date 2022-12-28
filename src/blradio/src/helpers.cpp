//////////////////////////////////////////////////////////////////////////////
///
/// @file helpers.cpp
/// @author Juan M. Casillas (juanm.casillas@gmail.com)
/// @brief Misc function helpers
/// @version 0.1
/// @date 2022/12/28
///
/// Misc function helpers
/// 
/// @copyright Copyright (c) 12/28/2022 Juan M. Casillas <juanm.casillas@gmail.com>
///
//////////////////////////////////////////////////////////////////////////////

#include <Arduino.h>
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

word SELECTOR = 0;

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
 * first 24 buttons are constant (all the switches, pushbuttons and the ROT5 encoder (topmost))
 * lower rotary are changed based on selector.
 * with 24 + 12 (first 24 buttons + 12 buttons from ROTENCS) I have 8 available modes (with 8 buttons left)
 * so I will limit the rotary encoder using the hardware washer to allow only 8 modes, (losing 9, 10, 11) and
 * map the buttons. so
 * 
 * @param signal_num 
 * @return uint8_t 
*/
uint8_t button_mapping(uint8_t signal_num) {
    uint8_t button_map = BUTTON_MAP[signal_num];

    if (button_map > 24) {
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