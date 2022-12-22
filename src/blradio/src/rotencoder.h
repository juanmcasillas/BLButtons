//////////////////////////////////////////////////////////////////////////////
///
/// @file rotencoder.h
/// @author Juan M. Casillas (juanm.casillas@gmail.com)
/// @brief rotary encoder wrapper. Only support left right and button
/// @version 0.1
/// @date 2022/12/22
///
/// rotary encoder wrapper. Only support left, right and button now also
/// value. Note that this version is very coupled to the PCF8574 code.
/// 
/// @copyright Copyright (c) 12/22/2022 Juan M. Casillas <juanm.casillas@gmail.com>
///
//////////////////////////////////////////////////////////////////////////////

#ifndef __ROTENCODER_H__
#define __ROTENCODER_H__

#include <Arduino.h>
#include <PCF8574.h>        // Using the expansor I2C

class  RotaryEncoder {
public:
typedef enum State_e {
    NONE = 0,
    LEFT,
    RIGHT,
    PRESSED
  } State;

    RotaryEncoder(uint8_t A, uint8_t B, uint8_t BTN, bool active = LOW); // local version
    RotaryEncoder(PCF8574 *_i2c_exp, uint8_t A, uint8_t B, uint8_t BTN, bool active = LOW); // expansor version
 
    State getDirection() { 
      State ret = State::NONE;
      
      if (value != previous_value) {
        ret = ( value > previous_value ? State::RIGHT : State::LEFT);
        previous_value = value;
      }
      return ret;
    }

    void  resetValues() {
      value = previous_value = 0;
    }



    State getButton()  { return button; }
    long  getValue()   { return value;  }
  
    void service();

protected:

    uint8_t digitalRead(uint8_t pin);
    void pinMode(uint8_t pin, uint8_t mode);
    void BaseConstructor(uint8_t A, uint8_t B, uint8_t BTN, bool active = LOW);


    uint8_t pinA;     // CLK
    uint8_t pinB;     // DT
    uint8_t pinBTN;   // SW
    bool    active_config; // INPUT_PULLOUT (active=LOW), INPUT (active=HIGH)
    PCF8574 *i2c_exp;
    uint8_t configType; // INPUT_PULLOUT (0 active) or INPUT (1 active)
    bool    changed;
    State   button;
    uint8_t previous_button;
    long    value;
    long    previous_value;
    uint8_t first_time = 1;
    unsigned char previous_state_a; // for "normal" rotary
 
};



#endif