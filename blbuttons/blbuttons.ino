#include <BleGamepad.h> 
//#define RELEASE 1

//BleGamepad bleGamepad("BL-Buttons", "JMCResearch.com", 100);
BleGamepad bleGamepad;

void setup() {
#ifndef RELEASE
    Serial.begin(115200);
    Serial.println("BLButtons Started");
#endif
    bleGamepad.begin();
}

void loop() {
  if(bleGamepad.isConnected()) {
    Serial.println("Press buttons 1 and 14. Move all axes to max. Set DPAD to down right.");
    bleGamepad.press(BUTTON_14);
    bleGamepad.press(BUTTON_1);
    bleGamepad.setAxes(127, 127, 127, 127, 127, 127, DPAD_DOWN_RIGHT);
    delay(500);

    Serial.println("Release button 14. Move all axes to min. Set DPAD to centred.");
    bleGamepad.release(BUTTON_14);
    bleGamepad.setAxes(-127, -127, -127, -127, -127, -127, DPAD_CENTERED);
    delay(500);
  }
}