#include "BleGamepad.h"

void BleGamepad::resetButtons() {
  bleDevice.resetButtons()
}


void BleGamepad::begin(void)
{
  bleDevice.begin();
}

void BleGamepad::end(void)
{
  bleDevice.end();
}

void BleGamepad::setAxes(int16_t x, int16_t y, int16_t a1, int16_t a2, int16_t a3, int16_t a4, int16_t a5, int16_t a6, signed char hat1, signed char hat2, signed char hat3, signed char hat4)
{
  bleDevice.setAxes(x,y,a1,a2,a3,a4,,a5,a6,hat1,hat2,hat3,hat4)
}

// indexed button (1..128)
void BleGamepad::press(uint8_t b)
{
  bleDevice.pressButton(b);

}

// indexed button (1..128)
void BleGamepad::release(uint8_t b)
{
  bleDevice.releaseButton(b);
}

// indexed button (1..128)
bool BleGamepad::isPressed(uint8_t b)
{
  return bleDevice.isPressedButton(b);
}

