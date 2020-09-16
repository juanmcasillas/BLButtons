// //////////////////////////////////////////////////////////////////////////
// 
// BL-Buttons. A Bluetooth-BLE buttons for simRacing
// Support clutch (2 modes) so we have:
// 4 axis x 3 = 12 axis (8 axis supported for Xinput)
// 17 buttons = 51 buttons
// 2 levers (two buttons) x 3 6 additional buttons
//
// (c) 2020 Juan M. Casillas <juanm.casillas@gmail.com 
// https://github.com/juanmcasillas/BLButtons
// 
// //////////////////////////////////////////////////////////////////////////
#include <Bounce2.h>      
#include <BleGamepad.h> 
#include <Keypad.h>           // https://github.com/Chris--A/Keypad
//#define RELEASE 1



#define ROWS 4
#define COLS 4
uint8_t rowPins[ROWS] = { 4, 16, 17,  5};
uint8_t colPins[COLS] = {14, 27, 26, 25};

#define NUM_BUTTONS 19
uint8_t keymap[ROWS][COLS] = {
  {1,2,3,4},
  {5,6,7,8},
  {9,10,11,12},
  {13,14,15,16} //17,18,19
};
Keypad customKeypad = Keypad( makeKeymap(keymap), rowPins, colPins, ROWS, COLS); 

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

BleGamepad bleGamepad("BL-Buttons", "JMCResearch.com", 100);

Button button_LC1 = Button();
Button button_LC2 = Button();
Button button_1 = Button();
Button clutch_1 = Button();
Button clutch_2 = Button();

int16_t pot_1 = 0;
int16_t pot_2 = 0;
int16_t pot_3 = 0;
int16_t pot_4 = 0;

int16_t pot_5 = 0;
int16_t pot_6 = 0;
int16_t pot_7 = 0;
int16_t pot_8 = 0;

int mode = 0;


void setup() {
#ifndef RELEASE
    Serial.begin(115200);
    Serial.println("BLButtons Started");
#endif

    analogReadResolution(12);
    analogSetAttenuation(ADC_11db); // ADC_11db ADC_6db11

    button_LC1.attach( BUTTON_LC1_PIN, INPUT  );
    button_LC1.setPressedState(HIGH);
    button_LC1.interval(5); 

    button_LC2.attach( BUTTON_LC2_PIN, INPUT  );
    button_LC2.setPressedState(HIGH);
    button_LC2.interval(5);   

    button_1.attach( BUTTON_1_PIN, INPUT  );
    button_1.setPressedState(HIGH);
    button_1.interval(5);  
    
    clutch_1.attach( BUTTON_CLUTCH_1_PIN, INPUT  );
    clutch_1.setPressedState(HIGH);
    clutch_1.interval(5); 

    clutch_2.attach( BUTTON_CLUTCH_2_PIN, INPUT  );
    clutch_2.setPressedState(HIGH);
    clutch_2.interval(5); 

    //pinMode(LED_PIN, OUTPUT);            // Setup the LED 
    
    customKeypad.addEventListener(keypadEvent);
    bleGamepad.begin();
}


void keypadEvent(KeypadEvent key){
  uint8_t keystate = customKeypad.getState();
  if (keystate==PRESSED)  { pressKey((NUM_BUTTONS*mode)+key); }
  if (keystate==RELEASED) { releaseKey((NUM_BUTTONS*mode)+key); }
}

void pressKey(uint8_t key) {
    //Serial.print("hold\t");
    //Serial.println(key);
    if(bleGamepad.isConnected()) {
      bleGamepad.press(key);
    }
}

void releaseKey(uint8_t key) {
    //Serial.print("release\t");
    //Serial.println(key);
    if(bleGamepad.isConnected()) {
      bleGamepad.release(key);
    }
}


void loop() {

  button_LC1.update();
  button_LC2.update();
  button_1.update();
  clutch_1.update();
  clutch_2.update();
  customKeypad.getKey();    // READ BUTTON MATRIX (EVENT CALLBACK SETUP)

  if(bleGamepad.isConnected()) {
    Serial.print(".");
    if (mode == 0) {
      pot_1 = map(analogRead(POT_PIN_1),0,4095,-127,127);// 0..4095
      pot_2 = map(analogRead(POT_PIN_2),0,4095,-127,127);// 0..4095
      pot_3 = map(analogRead(POT_PIN_3),0,4095,-127,127);// 0..4095
      pot_4 = map(analogRead(POT_PIN_4),0,4095,-127,127);// 0..4095
    }
    else {
      pot_5 = map(analogRead(POT_PIN_1),0,4095,-127,127);// 0..4095
      pot_6 = map(analogRead(POT_PIN_2),0,4095,-127,127);// 0..4095
      pot_7 = map(analogRead(POT_PIN_3),0,4095,-127,127);// 0..4095
      pot_8 = map(analogRead(POT_PIN_4),0,4095,-127,127);// 0..4095

    }

    if ( clutch_1.pressed() ) {
      mode = 1;
    }
   if ( clutch_1.released() ) { 
     mode = 0;
    }
    if ( clutch_2.pressed() ) {
      mode = 2;
    }
   if ( clutch_2.released() ) { 
     mode = 0;
    }

    // this is the right one
     if ( button_LC1.pressed() ) {
        bleGamepad.press((NUM_BUTTONS*mode)+BUTTON_17);
    }
   if ( button_LC1.released() ) { 
      bleGamepad.release((NUM_BUTTONS*mode)+BUTTON_17);
    }

    // this is the right one
     if ( button_LC2.pressed() ) {
        bleGamepad.press((NUM_BUTTONS*mode)+BUTTON_18);
    }
    if ( button_LC2.released() ) { 
      bleGamepad.release((NUM_BUTTONS*mode)+BUTTON_18);
    }

    if ( button_1.pressed() ) {
      bleGamepad.press((NUM_BUTTONS*mode)+BUTTON_19);
    }
   if ( button_1.released() ) { 
      bleGamepad.release((NUM_BUTTONS*mode)+BUTTON_19);
    }

    // update buttons & axis only here
    bleGamepad.setAxes(pot_1,pot_2,pot_3,pot_4,pot_5,pot_6,pot_7,pot_8, DPAD_CENTERED, DPAD_CENTERED,  DPAD_CENTERED, DPAD_CENTERED);
    
  }
  delay(50);
}