#include <Bounce2.h>      
#include <BleGamepad.h> 
//#define RELEASE 1

#define BUTTON_A_PIN 12
#define BUTTON_B_PIN 14
#define LED_PIN 2 


Bounce debouncer = Bounce();
BleGamepad bleGamepad("BL-Buttons", "JMCResearch.com", 100);

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

void loop() {
  if(bleGamepad.isConnected()) {

  buttonA.update();
  buttonB.update();

    debouncer.update();              // Update the Bounce instance
    int value = debouncer.read();

    // this is the right one
    if ( buttonA.pressed() ) {
        bleGamepad.press(BUTTON_1);
        digitalWrite(LED_PIN, HIGH );
        Serial.println("A-pressed");
    }
    if ( buttonA.released() ) {
      bleGamepad.release(BUTTON_1);
      digitalWrite(LED_PIN, LOW );
      Serial.println("A-released");
    }

    if ( buttonB.pressed() ) {
      bleGamepad.press(BUTTON_2);
      digitalWrite(LED_PIN, HIGH );
      Serial.println("B-pressed");
    }
   if ( buttonB.released() ) { 
      bleGamepad.release(BUTTON_2);
      digitalWrite(LED_PIN, LOW );
      Serial.println("B-released");
    }

    // press all buttons
    for (int i=0; i<128; i++) {
      bleGamepad.press(i+1);
       delay(200);
       bleGamepad.release(i+1);
    }

    // move all axes
    for (int i=-127; i<127; i++) {
       bleGamepad.setAxes(i, i, i, i, i, i, i,i,i,i, DPAD_DOWN_RIGHT);
       delay(200);
    }
       bleGamepad.setAxes(0, 0, 0, 0, 0, 0, 0,0,0,0, DPAD_CENTERED);

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
  delay(5);
}