// //////////////////////////////////////////////////////////////////////////
//
// BL-Buttons. A Bluetooth-BLE buttons for simRacing.
// Support clutch (2 modes) so we have:
// 4 axis x 3 = 12 axis (8 axis supported for Xinput).
// 17 buttons = 51 buttons.
// 2 levers (two buttons) x 3 6 additional buttons.
// Pov hats are not in use.
//
// (c) 2020 Juan M. Casillas <juanm.casillas@gmail.com
// https://github.com/juanmcasillas/BLButtons
//
// (c) 2022 Add support for Keyboard mode. Also added support for Mouse
// Mode and, KeyPad mode.
//
// //////////////////////////////////////////////////////////////////////////

#include <AnalogSmooth.h>  // https://github.com/MichaelThessel/arduino-analog-smooth
#include <Bounce2.h>       // https://github.com/thomasfredericks/Bounce2
#include <Keypad.h>        // https://github.com/Chris--A/Keypad

#include <BleKeyboard.h>
#include <BleMouse.h>
#include <BleGamepad.h>

#define RELEASE 1

#define ROWS 4
#define COLS 4

uint8_t rowPins[ROWS] = {4, 13, 17, 5};
uint8_t colPins[COLS] = {14, 27, 26, 25};

#define NUM_BUTTONS 19

uint8_t keymap[ROWS][COLS] = {
    {1, 2, 3, 4},
    {5, 6, 7, 8},
    {9, 10, 11, 12},
    {13, 14, 15, 16}  //17,18,19
};

uint8_t button_key_map[] = {
    KEYPAD_DIVIDE,    KEY_F9,   KEY_F5,  KEY_F1,
    KEYPAD_MULTIPLY,  KEY_F10,  KEY_F6,  KEY_F2,
    KEYPAD_SUBTRACT,  KEY_F11,  KEY_F7,  KEY_F3,
    KEYPAD_ADD,       KEY_F12,  KEY_F8,  KEY_F4  
};

/* example using mouse mappings */
/*/
uint8_t button_key_map[] = {
    MOUSE_LEFT,     KEY_F9,   KEY_F5,  KEY_F1,
    MOUSE_RIGHT,    KEY_F10,  KEY_F6,  KEY_F2,
    MOUSE_FORWARD,  KEY_F11,  KEY_F7,  KEY_F3,
    MOUSE_BACK,     KEY_F12,  KEY_F8,  KEY_F4  
};
*/

Keypad customKeypad = Keypad(makeKeymap(keymap), rowPins, colPins, ROWS, COLS);


#define LED_PIN 2
#define POT_PIN_1 33
#define POT_PIN_2 32
#define POT_PIN_3 35
#define POT_PIN_4 34
#define BUTTON_LC1_PIN 18
#define BUTTON_LC2_PIN 19
#define BUTTON_1_PIN 21
#define BUTTON_CLUTCH_1_PIN 15
#define BUTTON_CLUTCH_2_PIN 2

#define DELAY_TIME 50

Bounce2::Button button_LC1 = Bounce2::Button();
Bounce2::Button button_LC2 = Bounce2::Button();
Bounce2::Button button_1 = Bounce2::Button();
Bounce2::Button clutch_1 = Bounce2::Button();
Bounce2::Button clutch_2 = Bounce2::Button();

int16_t pot_1 = 0;
int16_t pot_2 = 0;
int16_t pot_3 = 0;
int16_t pot_4 = 0;

int16_t pot_5 = 0;
int16_t pot_6 = 0;
int16_t pot_7 = 0;
int16_t pot_8 = 0;

AnalogSmooth as_1 = AnalogSmooth();
AnalogSmooth as_2 = AnalogSmooth();
AnalogSmooth as_3 = AnalogSmooth();
AnalogSmooth as_4 = AnalogSmooth();
AnalogSmooth as_5 = AnalogSmooth();
AnalogSmooth as_6 = AnalogSmooth();
AnalogSmooth as_7 = AnalogSmooth();
AnalogSmooth as_8 = AnalogSmooth();

int mode = 0;

void setup() {
#ifndef RELEASE
    Serial.begin(115200);
    Serial.println("BLButtons Started");
#endif

    analogReadResolution(12);
    analogSetAttenuation(ADC_11db);  // ADC_11db ADC_6db11

    button_LC1.attach(BUTTON_LC1_PIN, INPUT);
    button_LC1.setPressedState(HIGH);
    button_LC1.interval(5);

    button_LC2.attach(BUTTON_LC2_PIN, INPUT);
    button_LC2.setPressedState(HIGH);
    button_LC2.interval(5);

    button_1.attach(BUTTON_1_PIN, INPUT);
    button_1.setPressedState(HIGH);
    button_1.interval(5);

    clutch_1.attach(BUTTON_CLUTCH_1_PIN, INPUT);
    clutch_1.setPressedState(HIGH);
    clutch_1.interval(5);

    clutch_2.attach(BUTTON_CLUTCH_2_PIN, INPUT);
    clutch_2.setPressedState(HIGH);
    clutch_2.interval(5);

    //pinMode(LED_PIN, OUTPUT);            // Setup the LED

    customKeypad.addEventListener(keypadEvent);
    
    // multiple HID profile in a single BL connection!
    Keyboard.begin();
    Mouse.begin();
    Gamepad.begin();
    
}

void keypadEvent(KeypadEvent key) {
    uint8_t keystate = customKeypad.getState();
    if (keystate == PRESSED) {
        
        if (mode == 2) {
            uint8_t mapped = button_key_map[key-1];
            if (mapped != MOUSE_LEFT && mapped != MOUSE_RIGHT && mapped != MOUSE_FORWARD && mapped != MOUSE_BACK) {
                Keyboard.write(mapped);
            }
            else {
                Mouse.click(mapped);
            }
        }
        else {
            pressKey((NUM_BUTTONS * mode) + key);
        }
    }
    if (keystate == RELEASED) {
        releaseKey((NUM_BUTTONS * mode) + key);
    }
}

void pressKey(uint8_t key) {
    if (bleDevice.isConnected()) {
        Gamepad.press(key);
    }
}

void releaseKey(uint8_t key) {
    if (bleDevice.isConnected()) {
        Gamepad.release(key);
    }
}

void loop() {
    button_LC1.update();
    button_LC2.update();
    button_1.update();
    clutch_1.update();
    clutch_2.update();
    customKeypad.getKey();  // READ BUTTON MATRIX (EVENT CALLBACK SETUP)

    if (bleDevice.isConnected()) {
        if (mode == 0) {
            pot_1 = map(as_1.smooth(analogRead(POT_PIN_1)), 0, 4095, -127, 127);  // 0..4095
            pot_2 = map(as_2.smooth(analogRead(POT_PIN_2)), 0, 4095, -127, 127);  // 0..4095
            pot_3 = map(as_3.smooth(analogRead(POT_PIN_3)), 0, 4095, -127, 127);  // 0..4095
            pot_4 = map(as_4.smooth(analogRead(POT_PIN_4)), 0, 4095, -127, 127);  // 0..4095
        } else {
            pot_5 = map(as_5.smooth(analogRead(POT_PIN_1)), 0, 4095, 10, -10);  // 0..4095    (rx) -> mouse_x
            pot_6 = map(as_6.smooth(analogRead(POT_PIN_2)), 0, 4095, 10, -10);  // 0..4095    (ry) -> mouse_y
            pot_7 = map(as_7.smooth(analogRead(POT_PIN_3)), 0, 4095, -5, 5);    // 0..4095    (z)  -> hweel
            pot_8 = map(as_8.smooth(analogRead(POT_PIN_4)), 0, 4095, -5, 5);    // 0..4095    (rz) -> wheel [leftmost]
        }

        if (clutch_1.pressed()) {
            mode = 1;
        }
        if (clutch_1.released()) {
            mode = 0;
        }
        if (clutch_2.pressed()) {
            mode = 2;
            // down - Map the keyboard controls and mouse input in the X-Y Axis.
        }
        if (clutch_2.released()) {
            mode = 0;
        }
        

        // this is the left one
        if (button_LC1.pressed()) {
            Gamepad.press((NUM_BUTTONS * mode) + BUTTON_17);
        }
        if (button_LC1.released()) {
            Gamepad.release((NUM_BUTTONS * mode) + BUTTON_17);
        }

        // this is the right one
        if (button_LC2.pressed()) {
            Gamepad.press((NUM_BUTTONS * mode) + BUTTON_18);
        }
        if (button_LC2.released()) {
            Gamepad.release((NUM_BUTTONS * mode) + BUTTON_18);
        }

        if (button_1.pressed()) {
            Gamepad.press((NUM_BUTTONS * mode) + BUTTON_19);
        }
        if (button_1.released()) {
            Gamepad.release((NUM_BUTTONS * mode) + BUTTON_19);
        }

        // update buttons & axis only here
        // mouse is nightmare only the wheel

        if (mode == 2) {
           Gamepad.setAxes(pot_1, pot_2, pot_3, pot_4, 0, 0, 0, 0, DPAD_CENTERED, DPAD_CENTERED, DPAD_CENTERED, DPAD_CENTERED);
           Mouse.move(pot_5,pot_6,pot_8,pot_7);
           //Mouse.wheel(pot_8,pot_7);
        }
        else {
            Gamepad.setAxes(pot_1, pot_2, pot_3, pot_4, pot_5, pot_6, pot_7, pot_8, DPAD_CENTERED, DPAD_CENTERED, DPAD_CENTERED, DPAD_CENTERED);
        }

    }
    delay(DELAY_TIME);
}
