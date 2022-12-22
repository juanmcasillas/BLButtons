// ----------------------------------------------------------------------------
// Rotary Encoder Driver with Acceleration
// Supports Click, DoubleClick, Long Click
//
// (c) 2010 karl@pitrich.com
// (c) 2014 karl@pitrich.com
//
// Timer-based rotary encoder logic by Peter Dannegger
// http://www.mikrocontroller.net/articles/Drehgeber
// ----------------------------------------------------------------------------

#include "ClickEncoderI2C.h"

// ----------------------------------------------------------------------------
// Button configuration (values for 1ms timer service calls)
//
#define ENC_I2C_BUTTONINTERVAL    10  // check button every x milliseconds, also debouce time
#define ENC_I2C_DOUBLECLICKTIME  600  // second click within 600ms
#define ENC_I2C_HOLDTIME        1200  // report held button after 1.2s

// ----------------------------------------------------------------------------
// Acceleration configuration (for 1000Hz calls to ::service())
//
// calculator https://bit-calculator.com/bit-shift-calculator

#define ENC_I2C_ACCEL_TOP      8192  // 128 // 8192: 32 >> 8 // 6144: 24 >> 8 // was 3072 // max. acceleration: *12 (val >> 8)
#define ENC_I2C_ACCEL_INC       350   // was 25
#define ENC_I2C_ACCEL_DEC         2

// ----------------------------------------------------------------------------

#if ENC_I2C_DECODER != ENC_I2C_NORMAL
#  ifdef ENC_I2C_HALFSTEP
     // decoding table for hardware with flaky notch (half resolution)
     const int8_t ClickEncoderI2C::table[16] __attribute__((__progmem__)) = {
       0, 0, -1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, -1, 0, 0
     };
#  else
     // decoding table for normal hardware
     const int8_t ClickEncoderI2C::table[16] __attribute__((__progmem__)) = {
       0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, -1, 1, 0
     };
#  endif
#endif

// ----------------------------------------------------------------------------

ClickEncoderI2C::ClickEncoderI2C(PCF8574 *i2c_exp, uint8_t A, uint8_t B, uint8_t BTN, uint8_t stepsPerNotch, bool active)
  : accelerationEnabled(true),
    delta(0), last(0), acceleration(0),
    steps(stepsPerNotch),
    pinA(A), pinB(B), pinBTN(BTN), pinsActive(active),
    sign(1)
{
  uint8_t configType = (pinsActive == LOW) ? INPUT_PULLUP : INPUT;
  _i2c_exp = i2c_exp;
  _direction = ClickEncoderI2C::None;
  
  _i2c_exp->pinMode(pinA, configType);
  _i2c_exp->pinMode(pinB, configType);
  _i2c_exp->pinMode(pinBTN, configType);

  if (_i2c_exp->digitalRead(pinA) == pinsActive) {
    last = 3;
  }

  if (_i2c_exp->digitalRead(pinB) == pinsActive) {
    last ^=1;
  }

  #ifndef WITHOUT_I2C_BUTTON
  doubleClickEnabled = true;
  button = Open;
  #endif
}

// ----------------------------------------------------------------------------
// call this every 1 millisecond via timer ISR
//
void ClickEncoderI2C::service(void)
{
  bool moved = false;
  unsigned long now = millis();
  _direction = ClickEncoderI2C::None;

  if (accelerationEnabled) { // decelerate every tick
    acceleration -= ENC_I2C_ACCEL_DEC;
    if (acceleration & 0x8000) { // handle overflow of MSB is set
      acceleration = 0;
    }
  }
  
  // new debug for the constructor bug
  // Serial.println("==");
  // Serial.println(pinA);
  // Serial.println(now);
  // Serial.println(accelerationEnabled);
  // Serial.println(acceleration);
  // Serial.println("--");


#if ENC_I2C_DECODER == ENC_I2C_FLAKY
  last = (last << 2) & 0x0F;

  if (_i2c_exp->digitalRead(pinA) == pinsActive) {
    last |= 2;
    _direction = ClickEncoderI2C::CW;
  }

  if (_i2c_exp->digitalRead(pinB) == pinsActive) {
    last |= 1;
    _direction = ClickEncoderI2C::CCW;
  }

  uint8_t tbl = pgm_read_byte(&table[last]);
  if (tbl) {
    delta += tbl;
    moved = true;
  }
#elif ENC_I2C_DECODER == ENC_I2C_NORMAL
  int8_t curr = 0;

  if (_i2c_exp->digitalRead(pinA) == pinsActive) {
    curr = 3;
    _direction = ClickEncoderI2C::CW;
  }

  if (_i2c_exp->digitalRead(pinB) == pinsActive) {
    curr ^= 1;
    _direction = ClickEncoderI2C::CCW;
  }

  int8_t diff = last - curr;

  if (diff & 1) {            // bit 0 = step
    last = curr;
    delta += (diff & 2) - 1; // bit 1 = direction (+/-)
    
    // jmc
    // if change of direction, remove acceleration
    if (accelerationEnabled && (diff & 2) != sign) {
      sign = (diff & 2);
      acceleration = 0;
    }
    moved = true;
  }
#else
# error "Error: define ENC_I2C_DECODER to ENC_I2C_NORMAL or ENC_I2C_FLAKY"
#endif

  if (accelerationEnabled && moved) {
    // increment accelerator if encoder has been moved
    if (acceleration <= (ENC_I2C_ACCEL_TOP - ENC_I2C_ACCEL_INC)) {
      acceleration += ENC_I2C_ACCEL_INC;
    }
  }

  // handle button
  //
#ifndef WITHOUT_I2C_BUTTON
  if (pinBTN > 0 // check button only, if a pin has been provided
      && (now - lastButtonCheck) >= ENC_I2C_BUTTONINTERVAL) // checking button is sufficient every 10-30ms
  {
    lastButtonCheck = now;

    if (_i2c_exp->digitalRead(pinBTN) == pinsActive) { // key is down
      keyDownTicks++;
      if (keyDownTicks > (ENC_I2C_HOLDTIME / ENC_I2C_BUTTONINTERVAL)) {
        button = Held;
      }
    }

    if (_i2c_exp->digitalRead(pinBTN) == !pinsActive) { // key is now up
      if (keyDownTicks /*> ENC_I2C_BUTTONINTERVAL*/) {
        if (button == Held) {
          button = Released;
          doubleClickTicks = 0;
        }
        else {
          #define ENC_I2C_SINGLECLICKONLY 1
          if (doubleClickTicks > ENC_I2C_SINGLECLICKONLY) {   // prevent trigger in single click mode
            if (doubleClickTicks < (ENC_I2C_DOUBLECLICKTIME / ENC_I2C_BUTTONINTERVAL)) {
              button = DoubleClicked;
              doubleClickTicks = 0;
            }
          }
          else {
            doubleClickTicks = (doubleClickEnabled) ? (ENC_I2C_DOUBLECLICKTIME / ENC_I2C_BUTTONINTERVAL) : ENC_I2C_SINGLECLICKONLY;
          }
        }
      }

      keyDownTicks = 0;
    }

    if (doubleClickTicks > 0) {
      doubleClickTicks--;
      if (--doubleClickTicks == 0) {
        button = Clicked;
      }
    }
  }
#endif // WITHOUT_I2C_BUTTON

}

// ----------------------------------------------------------------------------

ClickEncoderI2C::Direction ClickEncoderI2C::getDirection(void)
{
  return _direction;
}

int16_t ClickEncoderI2C::getValue(void)
{
  int16_t val;

  //cli();
  noInterrupts();
  val = delta;

  if (steps == 2) delta = val & 1;
  else if (steps == 4) delta = val & 3;
  else delta = 0; // default to 1 step per notch

  //sei();

  if (steps == 4) val >>= 2;
  if (steps == 2) val >>= 1;

  int16_t r = 0;
  int16_t accel = ((accelerationEnabled) ? (acceleration >> 8) : 0);

  if (val < 0) {
    r -= 1 + accel;
  }
  else if (val > 0) {
    r += 1 + accel;
  }

  interrupts();
  return r;
}

// ----------------------------------------------------------------------------

#ifndef WITHOUT_I2C_BUTTON
ClickEncoderI2C::Button ClickEncoderI2C::getButton(void)
{
  ClickEncoderI2C::Button ret = button;
  if (button != ClickEncoderI2C::Held) {
    button = ClickEncoderI2C::Open; // reset
  }
  return ret;
}
#endif
