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
#define ENCODER_DO_NOT_USE_INTERRUPTS
#include <ESP32Encoder.h>   // https://github.com/madhephaestus/ESP32Encoder/

/**
 * @brief a class to encapsulate the Rotary Encoder functionality,
 * using a PCF8574 I2C expansor, or a "normal" one, a single ky-040
 * rotary encoder without breakout board.
 * 
*/
class  RotaryEncoder {
public:
typedef enum State_e {
    NONE = 0,
    LEFT,
    RIGHT,
    PRESSED
  } State;

    RotaryEncoder(uint8_t A, uint8_t B, int BTN, bool active = LOW); // local version
    RotaryEncoder(PCF8574 *_i2c_exp, uint8_t A, uint8_t B, int BTN, bool active = LOW); // expansor version
 
    /**
     * @brief Get the Direction object
     * 
     * @return State  returns (State::RIGHT, State::LEFT, State::NONE)
    */
    State getDirection() { 
      State ret = State::NONE;
      
      if (value != previous_value) {
        ret = ( value > previous_value ? State::RIGHT : State::LEFT);
        previous_value = value;
      }
      return ret;
    }

    /**
     * @brief reset the counter values.
     * 
    */
    void  resetValues() {
      value = previous_value = 0;
    }


    State getButton()  { return button; }
    long  getValue()   { return value;  }
  
    void service();

protected:

    uint8_t digitalRead(uint8_t pin);
    void pinMode(uint8_t pin, uint8_t mode);
    void BaseConstructor(uint8_t A, uint8_t B, int BTN=-1, bool active = LOW);


    uint8_t pinA;     // CLK
    uint8_t pinB;     // DT
    int     pinBTN;   // SW (may be -1, not used)
    bool    active_config; // INPUT_PULLOUT (active=LOW), INPUT (active=HIGH)
    PCF8574 *i2c_exp;
    uint8_t configType; // INPUT_PULLOUT (0 active) or INPUT (1 active)
    bool    changed;
    State   button;
    uint8_t previous_button;
    uint8_t first_time = 1;
    long    value = 0;
    long    previous_value = 0;
    ESP32Encoder _encoder;

 
};



#endif