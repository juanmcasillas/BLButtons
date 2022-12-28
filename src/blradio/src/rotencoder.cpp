//////////////////////////////////////////////////////////////////////////////
///
/// @file rotencoder.cpp
/// @author Juan M. Casillas (juanm.casillas@gmail.com)
/// @brief implementation file
/// @version 0.1
/// @date 2022/12/22
///
///  
/// 
/// @copyright Copyright (c) 12/22/2022 Juan M. Casillas <juanm.casillas@gmail.com>
///
//////////////////////////////////////////////////////////////////////////////

#include "rotencoder.h"

/**
 * @brief Construct a new Rotary Encoder:: Rotary Encoder object. Local wired version
 * 
 * @param A 
 * @param B 
 * @param BTN 
 * @param active 
*/
RotaryEncoder::RotaryEncoder(uint8_t A, uint8_t B, int BTN, bool active) {
    i2c_exp = NULL;
    this->BaseConstructor(A, B, BTN, active);
    //this->pinMode(pinA,   configType);
    //this->pinMode(pinB,   configType);
    ESP32Encoder::useInternalWeakPullResistors=UP;
    this->_encoder.attachHalfQuad(pinA, pinB);
    if (pinBTN>=0) {
        this->pinMode(pinBTN, configType); 
        previous_button = this->digitalRead(pinBTN);
    }
      

}

/**
 * @brief Construct a new Rotary Encoder:: Rotary Encoder object. PCF8574 i2c expansor
 * 
 * @param _i2c_exp 
 * @param A 
 * @param B 
 * @param BTN 
 * @param active 
*/

RotaryEncoder::RotaryEncoder(PCF8574 *_i2c_exp, uint8_t A, uint8_t B, int BTN, bool active) {
    i2c_exp = _i2c_exp;
    this->BaseConstructor(A, B, BTN, active);
    i2c_exp->encoder(A,B);
    if (pinBTN>=0) {
        this->pinMode(pinBTN, configType); 
        previous_button = this->digitalRead(pinBTN);
    }

}

/**
 * @brief this function should be called periodically
 * 
 *  For example, using Ticker library:
 *  #include <Ticker.h>
 *  Ticker periodicTicker;
 * void timer_isr() {
 *   for (int i=0; i< MAX_ROTARIES; i++) {
 *       encoders[i]->service();
 *   }
 * }
 * periodicTicker.attach_ms(1, timer_isr);
*/
void RotaryEncoder::service() {

    if (i2c_exp != NULL) {
        changed = i2c_exp->readEncoderValue(pinA, pinB, &value);
    } else {
        value = _encoder.getCount();
    }

    if (pinBTN>=0) {
        uint8_t current_state_c = this->digitalRead(pinBTN); // BTN
        if (current_state_c!=previous_button){
        previous_button = current_state_c;
        button = (current_state_c == active_config ? RotaryEncoder::PRESSED  : RotaryEncoder::NONE);
        }
    }

    // cleanup the first time
    if (first_time) {
        changed = false;
        value = 0;
        previous_value = 0;
        first_time = 0;
    }


}

// protected functions

/**
 * @brief base constructor.
 * 
 * @param A 
 * @param B 
 * @param BTN 
 * @param active 
*/
void RotaryEncoder::BaseConstructor(uint8_t A, uint8_t B, int BTN, bool active) {
    pinA = A;
    pinB = B;
    pinBTN = BTN;
    active_config = active;

    configType = ((active_config == LOW) ? INPUT_PULLUP : INPUT);
    changed = false;
    button = RotaryEncoder::NONE;
    value = 0;
    previous_button  = 0;
    previous_value   = 0;

}

/**
 * @brief wrapper about digitalRead, to use the i2c or local one
 * 
 * @param pin 
 * @return uint8_t 
*/
uint8_t RotaryEncoder::digitalRead(uint8_t pin) {
    if (i2c_exp) {
        return i2c_exp->digitalRead(pin);
    }
    return ::digitalRead(pin);
}

/**
 * @brief wrapper about pinMode, to use the i2c or local one
 * 
 * @param pin 
 * @param mode 
*/
void RotaryEncoder::pinMode(uint8_t pin, uint8_t mode) {
    if (i2c_exp) {
        i2c_exp->pinMode(pin, mode);
        return;
    }
    ::pinMode(pin, mode);
}

