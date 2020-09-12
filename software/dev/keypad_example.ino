#include <ESP32Encoder.h>     // https://github.com/madhephaestus/ESP32Encoder/
#include <Keypad.h>           // https://github.com/Chris--A/Keypad
#include <BleGamepad.h>       // https://github.com/MagnusThome/ESP32-BLE-Gamepad


BleGamepad bleGamepad("RaceKeys", "Arduino", 100);


////////////////////// BUTTON MATRIX //////////////////////
#define ROWS 4
#define COLS 4
uint8_t rowPins[ROWS] = {27, 25, 12, 32};
uint8_t colPins[COLS] = {17, 16, 00, 02};
uint8_t keymap[ROWS][COLS] = {
  {0,1,2,3},
  {4,5,6,7},
  {8,9,10,11},
  {12,13,14,15}
};
Keypad customKeypad = Keypad( makeKeymap(keymap), rowPins, colPins, ROWS, COLS); 


//////////// ROTARY ENCODERS (WITH PUSH SWITCHES) ////////////
#define MAXENC 5
uint8_t uppPin[MAXENC] = {36, 22, 39, 34, 19};
uint8_t dwnPin[MAXENC] = {26, 21, 35, 14, 23};
uint8_t prsPin[MAXENC] = {18, 15, 33, 13, 05};
uint8_t encoderUpp[MAXENC] = {16,19,22,25,28};
uint8_t encoderDwn[MAXENC] = {17,20,23,26,29};
uint8_t encoderPrs[MAXENC] = {18,21,24,27,30};
ESP32Encoder encoder[MAXENC];
unsigned long holdoff[MAXENC] = {0,0,0,0,0};
int32_t prevenccntr[MAXENC] = {0,0,0,0,0};
bool prevprs[MAXENC] = {0,0,0,0,0};
#define HOLDOFFTIME 150   // TO PREVENT MULTIPLE ROTATE "CLICKS" WITH CHEAP ENCODERS WHEN ONLY ONE CLICK IS INTENDED






////////////////////////////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(115200);

  for (uint8_t i=0; i<MAXENC; i++) {
    encoder[i].clearCount();
    encoder[i].attachSingleEdge(dwnPin[i], uppPin[i]);
    pinMode(prsPin[i], INPUT_PULLUP);
  }
  customKeypad.addEventListener(keypadEvent);
  //customKeypad.setHoldTime(1);
  bleGamepad.begin();
  Serial.println("Booted!");
}



////////////////////////////////////////////////////////////////////////////////////////

void loop() {

  unsigned long now = millis();


  // -- ROTARY ENCODERS : ROTATION -- //

  for (uint8_t i=0; i<MAXENC; i++) {
    int32_t cntr = encoder[i].getCount();
    if (cntr!=prevenccntr[i]) {
      if (!holdoff[i]) {
        if (cntr>prevenccntr[i]) { sendKey(encoderUpp[i]); }
        if (cntr<prevenccntr[i]) { sendKey(encoderDwn[i]); }
        holdoff[i] = now;
        if (holdoff[i]==0) holdoff[i] = 1;  // SAFEGUARD WRAP AROUND OF millis() (WHICH IS TO 0) SINCE holdoff[i]==0 HAS A SPECIAL MEANING ABOVE
      }
      else if (now-holdoff[i] > HOLDOFFTIME) {
        prevenccntr[i] = encoder[i].getCount();
        holdoff[i] = 0;
      }
    }
    
  // -- ROTARY ENCODERS : PUSH SWITCH -- //

    bool pressed = !digitalRead(prsPin[i]);
    if (pressed != prevprs[i]) {
      if (pressed) {  // PRESSED
        pressKey(encoderPrs[i]);
      }
      else {          // RELEASED
        releaseKey(encoderPrs[i]);
      }
      prevprs[i] = !prevprs[i];
    }
  }

  customKeypad.getKey();    // READ BUTTON MATRIX (EVENT CALLBACK SETUP)

  delay(10);
 
}




////////////////////////////////////////////////////////////////////////////////////////

void keypadEvent(KeypadEvent key){
  uint8_t keystate = customKeypad.getState();
  if (keystate==PRESSED)  { pressKey(key); }
  if (keystate==RELEASED) { releaseKey(key); }
}


////////////////////////////////////////////////////////////////////////////////////////

void sendKey(uint8_t key) {
    uint32_t gamepadbutton = pow(2,key);      // CONVERT TO THE BINARY MAPPING GAMEPAD KEYS USE
    Serial.print("pulse\t");
    Serial.println(key);
    if(bleGamepad.isConnected()) {
      bleGamepad.press(gamepadbutton);
      delay(150);
      bleGamepad.release(gamepadbutton);
    }
}

void pressKey(uint8_t key) {
    uint32_t gamepadbutton = pow(2,key);      // CONVERT TO THE BINARY MAPPING GAMEPAD KEYS USE
    Serial.print("hold\t");
    Serial.println(key);
    if(bleGamepad.isConnected()) {
      bleGamepad.press(gamepadbutton);
    }
}

void releaseKey(uint8_t key) {
    uint32_t gamepadbutton = pow(2,key);      // CONVERT TO THE BINARY MAPPING GAMEPAD KEYS USE
    Serial.print("release\t");
    Serial.println(key);
    if(bleGamepad.isConnected()) {
      bleGamepad.release(gamepadbutton);
    }
}



////////////////////////////////////////////////////////////////////////////////////////