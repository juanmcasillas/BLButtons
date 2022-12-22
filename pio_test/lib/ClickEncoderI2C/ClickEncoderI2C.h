// ----------------------------------------------------------------------------
// Rotary Encoder Driver with Acceleration
// Supports Click, DoubleClick, Long Click
// adapter to support i2c extensor to read
//
// (c) 2010 karl@pitrich.com
// (c) 2014 karl@pitrich.com
// (c) 2022 juanm.casillas@gmail.com
//
// Timer-based rotary encoder logic by Peter Dannegger
// http://www.mikrocontroller.net/articles/Drehgeber
// ----------------------------------------------------------------------------

#ifndef __have__ClickEncoderI2C_h__
#define __have__ClickEncoderI2C_h__

// ----------------------------------------------------------------------------


#include <Arduino.h>
#include <PCF8574.h>

// ----------------------------------------------------------------------------

#define ENC_I2C_NORMAL        (1 << 1)   // use Peter Danneger's decoder
#define ENC_I2C_FLAKY         (1 << 2)   // use Table-based decoder

// ----------------------------------------------------------------------------

#ifndef ENC_I2C_DECODER
#  define ENC_I2C_DECODER     ENC_I2C_NORMAL
#endif

#if ENC_I2C_DECODER == ENC_I2C_FLAKY
#  ifndef ENC_I2C_HALFSTEP
#    define ENC_I2C_HALFSTEP  1        // use table for half step per default
#  endif
#endif

// ----------------------------------------------------------------------------

class ClickEncoderI2C
{
public:
  typedef enum Button_e {
    Open = 0,
    Closed,

    Pressed,
    Held,
    Released,

    Clicked,
    DoubleClicked

  } Button;

public:
  ClickEncoderI2C(PCF8574 *i2c_exp, uint8_t A, uint8_t B, uint8_t BTN = -1, uint8_t stepsPerNotch = 1, bool active = LOW);

  void service(void);
  int16_t getValue(void);

#ifndef WITHOUT_I2C_BUTTON
public:
  Button getButton(void);
#endif

#ifndef WITHOUT_I2C_BUTTON
public:
  void setDoubleClickEnabled(const bool &d)
  {
    doubleClickEnabled = d;
  }

  const bool getDoubleClickEnabled()
  {
    return doubleClickEnabled;
  }
#endif

public:
  void setAccelerationEnabled(const bool &a)
  {
    accelerationEnabled = a;
    if (accelerationEnabled == false) {
      acceleration = 0;
    }
  }

  const bool getAccelerationEnabled()
  {
    return accelerationEnabled;
  }

private:
  const uint8_t pinA;
  const uint8_t pinB;
  const uint8_t pinBTN;
  const bool pinsActive;
  volatile int16_t delta; //v
  volatile int16_t sign; // jmc v
  volatile int16_t last; // v
  uint8_t steps;
  volatile uint16_t acceleration; // v
  bool accelerationEnabled;
#if ENC_I2C_DECODER != ENC_I2C_NORMAL
  static const int8_t table[16];
#endif
#ifndef WITHOUT_I2C_BUTTON
  volatile Button button; // v
  bool doubleClickEnabled;
  uint16_t keyDownTicks = 0;
  uint8_t doubleClickTicks = 0;
  unsigned long lastButtonCheck = 0;
#endif
  PCF8574 *_i2c_exp;
};

// ----------------------------------------------------------------------------

#endif // __have__ClickEncoderI2C_h__
