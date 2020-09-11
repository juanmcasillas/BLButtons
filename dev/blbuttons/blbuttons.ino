#include <Bounce2.h>      
#include <BleGamepad.h> 
//#define RELEASE 1

#define BUTTON_A_PIN 12
#define BUTTON_B_PIN 14
#define LED_PIN 2 



BleGamepad bleGamepad("BL-Buttons1", "JMCResearch.com", 100);

Button buttonA = Button();
Button buttonB = Button();

void setup() {
#ifndef RELEASE
    Serial.begin(115200);
    Serial.println("BLButtons Started");
#endif

    buttonA.attach( BUTTON_A_PIN, INPUT_PULLUP  );
    buttonA.setPressedState(HIGH);
    buttonA.interval(5); 

    buttonB.attach( BUTTON_B_PIN, INPUT_PULLUP  );
    buttonB.setPressedState(HIGH);
    buttonB.interval(5);   
    
    pinMode(LED_PIN, OUTPUT);            // Setup the LED 
    
    
    bleGamepad.begin();
}

uint8_t button = 1;
signed char axis = -127;

void loop() {

  if(bleGamepad.isConnected()) {

    buttonA.update();
    buttonB.update();
    
    /*
    if (button > 128) {
      button = 1;
      bleGamepad.resetButtons();
    }

   if (axis > 127) {
      axis = -127;
      bleGamepad.setAxes(0,0,0,0,0,0,0,0,0,0, DPAD_CENTERED);
   }

    for (int i=1; i<=button; i++) {
      bleGamepad.press(i);
    }

    bleGamepad.setAxes(axis,axis,axis,axis,axis,axis,axis,axis,axis,axis, DPAD_DOWN_RIGHT);  
    button++;
    axis++;
    delay(100);
    */

    // this is the right one
    if ( buttonA.pressed() ) {
        bleGamepad.press(BUTTON_1);
        bleGamepad.press(BUTTON_64);
        bleGamepad.press(BUTTON_127);
        digitalWrite(LED_PIN, LOW );
        //Serial.println("A-pressed");
        bleGamepad.setAxes(0,0,0,0,0,0,0,0,0,0, DPAD_UP_RIGHT, DPAD_UP_LEFT,  DPAD_DOWN_RIGHT, DPAD_DOWN_LEFT);
    }
    if ( buttonA.released() ) {
      bleGamepad.release(BUTTON_1);
      bleGamepad.release(BUTTON_64);
      bleGamepad.release(BUTTON_127);
      digitalWrite(LED_PIN, HIGH );
     // Serial.println("A-released");
       bleGamepad.setAxes(0,0,0,0,0,0,0,0,0,0, DPAD_CENTERED, DPAD_CENTERED,  DPAD_CENTERED, DPAD_CENTERED);
    }

    if ( buttonB.pressed() ) {
      bleGamepad.press(BUTTON_9);
      bleGamepad.press(BUTTON_65);
      bleGamepad.press(BUTTON_128);
      digitalWrite(LED_PIN, LOW );
     // Serial.println("B-pressed");
      bleGamepad.setAxes(0,0,0,0,0,0,0,0,0,0, DPAD_DOWN_RIGHT, DPAD_DOWN_LEFT,  DPAD_UP_RIGHT, DPAD_UP_LEFT);
    }
   if ( buttonB.released() ) { 
      bleGamepad.release(BUTTON_9);
      bleGamepad.release(BUTTON_65);
      bleGamepad.release(BUTTON_128);
      digitalWrite(LED_PIN, HIGH );
     //Serial.println("B-released");
      bleGamepad.setAxes(0,0,0,0,0,0,0,0,0,0, DPAD_CENTERED, DPAD_CENTERED,  DPAD_CENTERED, DPAD_CENTERED);
    }



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
}