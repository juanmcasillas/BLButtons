#include <Bounce2.h>      
#include <BleGamepad.h> 
#include <Keypad.h>           // https://github.com/Chris--A/Keypad
//#define RELEASE 1



#define ROWS 4
#define COLS 4
uint8_t rowPins[ROWS] = { 4, 16, 17,  5};
uint8_t colPins[COLS] = {14, 27, 26, 25};

uint8_t keymap[ROWS][COLS] = {
  {1,2,3,4},
  {5,6,7,8},
  {9,10,11,12},
  {13,14,15,16}
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

BleGamepad bleGamepad("BL-Buttons1", "JMCResearch.com", 100);

Button button_LC1 = Button();
Button button_LC2 = Button();
Button button_1 = Button();
Button clutch_1 = Button();
Button clutch_2 = Button();

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
  if (keystate==PRESSED)  { pressKey(key); }
  if (keystate==RELEASED) { releaseKey(key); }
}

void pressKey(uint8_t key) {
    Serial.print("hold\t");
    Serial.println(key);
    if(bleGamepad.isConnected()) {
      bleGamepad.press(key);
    }
}

void releaseKey(uint8_t key) {
    Serial.print("release\t");
    Serial.println(key);
    if(bleGamepad.isConnected()) {
      bleGamepad.release(key);
    }
}

uint8_t button = 1;
signed char axis = -127;
int16_t pot_1 = 0;
int16_t pot_2 = 0;
int16_t pot_3 = 0;
int16_t pot_4 = 0;

void loop() {

  button_LC1.update();
  button_LC2.update();
  button_1.update();
  clutch_1.update();
  clutch_2.update();
  customKeypad.getKey();    // READ BUTTON MATRIX (EVENT CALLBACK SETUP)

  if(bleGamepad.isConnected()) {


    
    pot_1 = map(analogRead(POT_PIN_1),0,4095,-127,127);// 0..4095
    pot_2 = map(analogRead(POT_PIN_2),0,4095,-127,127);// 0..4095
    pot_3 = map(analogRead(POT_PIN_3),0,4095,-127,127);// 0..4095
    pot_4 = map(analogRead(POT_PIN_4),0,4095,-127,127);// 0..4095
 

    if ( clutch_1.pressed() ) {
      bleGamepad.press(BUTTON_20);
      //digitalWrite(LED_PIN, LOW );
      //Serial.println("A-pressed");
      //bleGamepad.setAxes(-32767,32767,0,0,0,0,0,0,0,0, DPAD_UP_RIGHT, DPAD_UP_LEFT,  DPAD_DOWN_RIGHT, DPAD_DOWN_LEFT);
    }
   if ( clutch_1.released() ) { 
      bleGamepad.release(BUTTON_20);
      //digitalWrite(LED_PIN, HIGH );
     // Serial.println("A-released");
       //bleGamepad.setAxes(0,0,0,0,0,0,0,0,0,0, DPAD_CENTERED, DPAD_CENTERED,  DPAD_CENTERED, DPAD_CENTERED);
    }

    if ( clutch_2.pressed() ) {
      bleGamepad.press(BUTTON_21);
      //digitalWrite(LED_PIN, LOW );
      //Serial.println("A-pressed");
      //bleGamepad.setAxes(-32767,32767,0,0,0,0,0,0,0,0, DPAD_UP_RIGHT, DPAD_UP_LEFT,  DPAD_DOWN_RIGHT, DPAD_DOWN_LEFT);
    }
   if ( clutch_2.released() ) { 
      bleGamepad.release(BUTTON_21);
      //digitalWrite(LED_PIN, HIGH );
     // Serial.println("A-released");
       //bleGamepad.setAxes(0,0,0,0,0,0,0,0,0,0, DPAD_CENTERED, DPAD_CENTERED,  DPAD_CENTERED, DPAD_CENTERED);
    }

    // this is the right one
     if ( button_LC1.pressed() ) {
        bleGamepad.press(BUTTON_17);
        //digitalWrite(LED_PIN, LOW );
        //Serial.println("A-pressed");
        //bleGamepad.setAxes(-32767,32767,0,0,0,0,0,0,0,0, DPAD_UP_RIGHT, DPAD_UP_LEFT,  DPAD_DOWN_RIGHT, DPAD_DOWN_LEFT);
    }
   if ( button_LC1.released() ) { 
      bleGamepad.release(BUTTON_17);
      //digitalWrite(LED_PIN, HIGH );
     // Serial.println("A-released");
       //bleGamepad.setAxes(0,0,0,0,0,0,0,0,0,0, DPAD_CENTERED, DPAD_CENTERED,  DPAD_CENTERED, DPAD_CENTERED);
    }

    // this is the right one
     if ( button_LC2.pressed() ) {
        bleGamepad.press(BUTTON_18);
        //digitalWrite(LED_PIN, LOW );
        //Serial.println("A-pressed");
        //bleGamepad.setAxes(-32767,32767,0,0,0,0,0,0,0,0, DPAD_UP_RIGHT, DPAD_UP_LEFT,  DPAD_DOWN_RIGHT, DPAD_DOWN_LEFT);
    }
    if ( button_LC2.released() ) { 
      bleGamepad.release(BUTTON_18);
      //digitalWrite(LED_PIN, HIGH );
     // Serial.println("A-released");
       //bleGamepad.setAxes(0,0,0,0,0,0,0,0,0,0, DPAD_CENTERED, DPAD_CENTERED,  DPAD_CENTERED, DPAD_CENTERED);
    }

    if ( button_1.pressed() ) {
      bleGamepad.press(BUTTON_19);
      //digitalWrite(LED_PIN, LOW );
     // Serial.println("B-pressed");
      //bleGamepad.setAxes(32767,32767,0,0,0,0,0,0,0,0, DPAD_DOWN_RIGHT, DPAD_DOWN_LEFT,  DPAD_UP_RIGHT, DPAD_UP_LEFT);
    }
   if ( button_1.released() ) { 
      bleGamepad.release(BUTTON_19);
      //digitalWrite(LED_PIN, HIGH );
     //Serial.println("B-released");
      //bleGamepad.setAxes(0,0,0,0,0,0,0,0,0,0, DPAD_CENTERED, DPAD_CENTERED,  DPAD_CENTERED, DPAD_CENTERED);
    }

    bleGamepad.setAxes(pot_1,pot_2,pot_3,pot_4,0,0,0,0,0,0, DPAD_CENTERED, DPAD_CENTERED,  DPAD_CENTERED, DPAD_CENTERED);

/*
    // press all buttons
    for (int i=0; i<128; i++) {
      bleGamepad.press(i+1);
       //delay(1000);
       //bleGamepad.release(i+1);
    }
    */

    // move all axes
    //for (int i=-127; i<127; i++) {
    //   bleGamepad.setAxes(i, i, i, i, i, i, i,i,i,i, DPAD_DOWN_RIGHT);
    //   //delay(200);
    //}


  

    //bleGamepad.setAxes(127, 127, 127, 127, 127, 127, DPAD_DOWN_RIGHT);
    //Serial.println("Press buttons 1 and 14. Move all axes to max. Set DPAD to down right.");
    //bleGamepad.press(BUTTON_14);
    //bleGamepad.press(BUTTON_1);
    //bleGamepad.setAxes(127, 127, 127, 127, 127, 127, DPAD_DOWN_RIGHT);
    //delay(500);
    //Serial.println("Release button 14. Move all axes to min. Set DPAD to centred.");
    //bleGamepad.release(BUTTON_14);
    //bleGamepad.setAxes(-127, -127, -127, -127, -127, -127, DPAD_CENTERED);
    //delay(500);
     
  }
  
  delay(50);
}