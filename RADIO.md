Lights:

    taxi lights (left, right)
    nav lights 
    landing lights (left, right)
    anti collision lights
    formation lights
    
    - internal lights
    - dim 
    - center console
    - instruments
    - left console
    - right console
    - storm lights

F5
AN/ARC-164 UHF
    1 selector for channel preset
    5 selector for radio frequency tunning knobs
    1 mode (off, main, both, adf)
    1 freq mode (manual, preset, guard)
    1 selector antena (upper, auto, lower)

TACAN
    2 tunning knobs
    1 tacan mode: (off, t/r, a/a rec a/a t/r)
    1 nav_mode (df, tacan)


HSI
    1 knob for HDG
    1 knob for CRS


C101
ADF
    1 selector NAV/ADF (HSI)
    1 transfer button (to select adf1, adf2)
    3 kobs for ADF tunning
    1 selector for mode (off, ant, adf, test)
VOR (VHF-NAV)
    1 knob for tunning
    1 selector for mode (off, nav, stdby, dme, guard)
    1 selector antena (upper, auto, lower)
    1 dme hold or norm
VOR (ILS/AN/ARN-127)
    1 VHF 10 position selector (KHz) 105-117
    1 VHF 10 position selector (KHz) 0-9
    1 VHF 2 position selector (KHz) 00,05,

Huey
UHF AN/ARC-51BX (AA comms most used)
    1 function (off, t/r, t/r+g, adf)
    1 man/preset/gd-xmit
    1 select channel (1-20)
    3 dial knobs

VHF AN/ARC-134 (AA and Tower)
    1 function (off, power, test)
    2 dial knobs

FM AN/ARC-131 internal flight comms, ground homing
    1 function (off, t/r, retran,  home)
    4 dial knobs

AN/ARN-82 Radio Nav Panel
    1 selector (ADF, VOR)

AN/AR-83 ADF
    1 selector (off, adf, ant, loop)
    1 adf loop (left, right)
    1 bfo (on, off)
    1 adf band selector (3 pos)
    1 tunning knob
    1 adf gain

Pro MICRO inputs

ENCODERS:
    CLK         2x encoder D2/D3 se supone que se puede meter en un keypad.
    DT          
    SW          to the KEYPAD (button) + GND
    VCC (+)     common
    GND         common

5 encoders:
    2 (CLK,DT) x 5 = 10 signals + 5 SW = 15 (4x4) = 16 inputs. left 1.

Selectors & switches.
    3 position
    1 radio selector 8 pos (better 6)
    7 ?


Resume:

ENCODERS:
2 PINs PER encoder: 8 PINS for input encoding.

Encoders:                            =  4 + 8 PINS.
Encoder 1P12T using 12T              = 12
5 three way switch (using 2)         = 10            (10)
5 two way switch (using 1)           =  5            (7)
5 buttons                            =  5            (5 black, 5 red)
TOTAL                                = 36 input.

6x6 = 36 matrix.

Buttons:
  Encoders: 3 * 4 =       12
  3-way: 5 x 2 =          10
  2-way: 5 x 1 =           5
  5 buttons: 5 x 1 =       5
  TOTAL                   32 buttons.
  Modes (12): 12 x 32=    384 buttons (3x128 buttons) (gamepad device with 3 pages ? XD)


https://github.com/RobTillaart/PCF8574/issues/27

3 x 3 -> 9 buttons (5 buttons + 4 encoders)

1 expansor para los switches de 3 vias (8 inputs) 0x1
1 expansor para los switches de 2 vias + 1 de tres vias (8 inputs) 0x2
1 expansor para los encoders (4 * 2) 8 inputs.
2 + 6 = 8 PINS.


|-------|-----------------------|----------------------|
| PIN   | TO                    | NOTES                |
|-------|-----------------------|---------------------:|
Button matrix: 9 values, 3x3 matrix
| 13    | COL_0                 |                      |
| 14    | COL_1                 | Boot Fail if HIGH    |
| 27    | COL_2                 |                      |
| 26    | ROW_0                 |                      |
| 25    | ROW_1                 |                      |
| 33    | ROW_2                 | CH_4 ADC1 together   |
|-------|-----------------------|----------------------|
MODE_SELECTOR = 12 values, 3x4 matrix
| 4     | ROW_0                 | Pulled up            |
| 16    | ROW_1                 | Connected to LED     |
| 17    | ROW_2                 |                      |
| 18    | COL_0                 |                      |
| 19    | COL_1                 |                      |
| 23    | COL_2                 |                      |
| 32    | COL_3                 | CH_5 ADC1            |
|-------|-----------------------|----------------------|
maybe use the 5th element.
| 15    | ENCODER_5 SW          |                      |
| 34    | ENCODER_5 DT          | CH_7 ADC1 only IN    | 
| 35    | ENCODER_5 CLK         | CH_6 ADC1 only IN    |
|-------|-----------------------|----------------------|
| 21    | SDA                   |                      |
| 22    | SCL                   |                      |
|-------|-----------------------|----------------------|
| 2     |                       | low or won't flash   | 
| 5     |                       | Boot Fail if HIGH    |
| 12    |                       |                      | 
|-------|-----------------------|----------------------|
|       | free pins             |                      |
|-------|-----------------------|----------------------|
|-------|-----------------------|----------------------|
| 0     |                       | not use              |
|-------|-----------------------|----------------------|


LAST CALCULATION.
Buttons must be momentary in order to work with the KEYPAD.

So:
  Mode selector: 12 input keypadtrix 3*4 = 12 signals. (7 GPIO)
  5 3-way: (10 signals, using 8 in one expansor I2C using 2 in the other expansor.
  6 2-way: (6 signals, using 6 in the second expansor)
  5 encoders: 
        - 1x4 signals in a keypad arrangement of 3x3 GPIO (6 GPIO) 
        - 2x4 signals in the third expansor (8 signals)

        - 1x1 signal in the GPIO 2 (SW)
        - 2x1 signals in GPIO5, and 12.

TOTALS:
  1x  Mode:           12 signals, 4x3, 7 GPIO.
  5x  3-way:          10 signals, 2 GPIO (SCL,SDA)
  6x  2-way:           6 signals
  5x  encoders: SW:    4 signals in the keypad button, 3x3, 6 GPIO
             CLK/DT    8 signals in the third expansor
             SW:       1 signal on GPIO2
             CLK/DT    2 signals on GPIO5, GPIO12
  5x buttons:          5 signals in the keypad button.

## after implementing the Serial bridge between Arduino Nano & ESP32

So:
  Mode selector: 12 Signals from 15 available using GPIO27,GPIO26 [3 available, frees 4 GPIO[4,16,17,18,19,23,32], used on Serial]
  5 3-way: (10 signals, using 8 in one expansor I2C using 2 in the other expansor.
  6 2-way: (6 signals, using 6 in the second expansor)
  6 encoders: 
        - 1x6 signals in a keypad arrangement of 4x4 
        - 2x4 signals in the third expansor (8 signals) CLK/DT
        - 4x1 signals in GPIO5, GPIO12, 

  1x  Mode:           12 signals, 2 GPIO [Serial]
  6x  3-way:          10 signals, 2 GPIO (SCL,SDA)
                       2 signals, 2 GPIO [Serial]
  6x  2-way:           6 signals
  6x  encoders: SW:       6 signals in the keypad button, 4x4, 8 GPIO
                CLK/DT    8 signals in the third expansor (SCL,SDA)
                CLK/DT    2 signals on GPIO5, GPIO12
                CLK/DT    2 signals on GPI23, GPIO32
  6x buttons:          6 signals in the keypad button.

                       ?? 1 signal 2 GPIO available [Serial]
                       ?? 4 signal available on keypad.
 

  4 free gpios mean instead 3x3 (9) 4x4 (16) -> 7 more buttons !!! [13,14]

|-------|-----------------------|----------------------|
| PIN   | TO                    | NOTES                |
|-------|-----------------------|---------------------:|
Button matrix: 16 values, 4x4 matrix
| 13    | COL_0                 |                      |
| 14    | COL_1                 | Boot Fail if HIGH    |
| 16    | COL_2                 | Connected to LED     |
| 17    | COL_3                 |                      |
| 18    | ROW_0                 |                      |
| 19    | ROW_1                 |                      |
| 25    | ROW_2                 |                      |
| 33    | ROW_3                 | CH_4 ADC1 together   |
|-------|-----------------------|----------------------|
MODE_SELECTOR = 12 values, 12 signals, 2 GPIO [Serial]
| 27    | RX                    |                      |
| 26    | TX                    |                      |
|-------|-----------------------|----------------------|
Expansors I2C
|-------|-----------------------|----------------------|
| 21    | SDA                   |                      |
| 22    | SCL                   |                      |
|-------|-----------------------|----------------------|
Extra encoders 
| 5     | CLK                   | Boot Fail if HIGH    |
| 12    | DT                    |                      | 
| 23    | CLK                   |                      |
| 32    | DT                    | CH_5 ADC1            |
|-------|-----------------------|----------------------|
|       | free pins             |                      |
|-------|-----------------------|----------------------|
|-------|-----------------------|----------------------|
| 0     |                       | not use              |
|-------|-----------------------|----------------------|
maybe use the 5th element.
| 2     |                       | low or won't flash   | 
| 4     |                       | Pulled up            |
| 15    |                       |                      |
| 34    |                       | CH_7 ADC1 only IN    | 
| 35    |                       | CH_6 ADC1 only IN    |

https://github.com/AM-STUDIO/32-FUNCTION-BUTTON-BOX/blob/master/ARDUINO_BUTTON_BOXV2.ino
https://forum.arduino.cc/t/sketch-para-cinco-encoders-con-promicro/994954
https://www.youtube.com/watch?v=Z7Sc4MJ8RPM

```
//BUTTON BOX 
//USE w ProMicro

#include <Keypad.h>
#include <Joystick.h>

#define ENABLE_PULLUPS
#define NUMROTARIES 4
#define NUMBUTTONS 24
#define NUMROWS 5
#define NUMCOLS 5


byte buttons[NUMROWS][NUMCOLS] = {
  {0,1,2,3,4},
  {5,6,7,8,9},
  {10,11,12,13,14},
  {15,16,17,18,19},
  {20,21,22,23},
};

struct rotariesdef {
  byte pin1;
  byte pin2;
  int ccwchar;
  int cwchar;
  volatile unsigned char state;
};

rotariesdef rotaries[NUMROTARIES] {
  {0,1,24,25,0},
  {2,3,26,27,0},
  {4,5,28,29,0},
  {6,7,30,31,0},
};

#define DIR_CCW 0x10
#define DIR_CW 0x20
#define R_START 0x0

#ifdef HALF_STEP
#define R_CCW_BEGIN 0x1
#define R_CW_BEGIN 0x2
#define R_START_M 0x3
#define R_CW_BEGIN_M 0x4
#define R_CCW_BEGIN_M 0x5
const unsigned char ttable[6][4] = {
  // R_START (00)
  {R_START_M,            R_CW_BEGIN,     R_CCW_BEGIN,  R_START},
  // R_CCW_BEGIN
  {R_START_M | DIR_CCW, R_START,        R_CCW_BEGIN,  R_START},
  // R_CW_BEGIN
  {R_START_M | DIR_CW,  R_CW_BEGIN,     R_START,      R_START},
  // R_START_M (11)
  {R_START_M,            R_CCW_BEGIN_M,  R_CW_BEGIN_M, R_START},
  // R_CW_BEGIN_M
  {R_START_M,            R_START_M,      R_CW_BEGIN_M, R_START | DIR_CW},
  // R_CCW_BEGIN_M
  {R_START_M,            R_CCW_BEGIN_M,  R_START_M,    R_START | DIR_CCW},
};
#else
#define R_CW_FINAL 0x1
#define R_CW_BEGIN 0x2
#define R_CW_NEXT 0x3
#define R_CCW_BEGIN 0x4
#define R_CCW_FINAL 0x5
#define R_CCW_NEXT 0x6

const unsigned char ttable[7][4] = {
  // R_START
  {R_START,    R_CW_BEGIN,  R_CCW_BEGIN, R_START},
  // R_CW_FINAL
  {R_CW_NEXT,  R_START,     R_CW_FINAL,  R_START | DIR_CW},
  // R_CW_BEGIN
  {R_CW_NEXT,  R_CW_BEGIN,  R_START,     R_START},
  // R_CW_NEXT
  {R_CW_NEXT,  R_CW_BEGIN,  R_CW_FINAL,  R_START},
  // R_CCW_BEGIN
  {R_CCW_NEXT, R_START,     R_CCW_BEGIN, R_START},
  // R_CCW_FINAL
  {R_CCW_NEXT, R_CCW_FINAL, R_START,     R_START | DIR_CCW},
  // R_CCW_NEXT
  {R_CCW_NEXT, R_CCW_FINAL, R_CCW_BEGIN, R_START},
};
#endif

byte rowPins[NUMROWS] = {21,20,19,18,15}; 
byte colPins[NUMCOLS] = {14,16,10,9,8}; 

Keypad buttbx = Keypad( makeKeymap(buttons), rowPins, colPins, NUMROWS, NUMCOLS); 

Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, 
  JOYSTICK_TYPE_JOYSTICK, 32, 0,
  false, false, false, false, false, false,
  false, false, false, false, false);

void setup() {
  Joystick.begin();
  rotary_init();}

void loop() { 

  CheckAllEncoders();

  CheckAllButtons();

}

void CheckAllButtons(void) {
      if (buttbx.getKeys())
    {
       for (int i=0; i<LIST_MAX; i++)   
        {
           if ( buttbx.key[i].stateChanged )   
            {
            switch (buttbx.key[i].kstate) {  
                    case PRESSED:
                    case HOLD:
                              Joystick.setButton(buttbx.key[i].kchar, 1);
                              break;
                    case RELEASED:
                    case IDLE:
                              Joystick.setButton(buttbx.key[i].kchar, 0);
                              break;
            }
           }   
         }
     }
}


void rotary_init() {
  for (int i=0;i<NUMROTARIES;i++) {
    pinMode(rotaries[i].pin1, INPUT);
    pinMode(rotaries[i].pin2, INPUT);
    #ifdef ENABLE_PULLUPS
      digitalWrite(rotaries[i].pin1, HIGH);
      digitalWrite(rotaries[i].pin2, HIGH);
    #endif
  }
}


unsigned char rotary_process(int _i) {
   unsigned char pinstate = (digitalRead(rotaries[_i].pin2) << 1) | digitalRead(rotaries[_i].pin1);
  rotaries[_i].state = ttable[rotaries[_i].state & 0xf][pinstate];
  return (rotaries[_i].state & 0x30);
}

void CheckAllEncoders(void) {
  for (int i=0;i<NUMROTARIES;i++) {
    unsigned char result = rotary_process(i);
    if (result == DIR_CCW) {
      Joystick.setButton(rotaries[i].ccwchar, 1); delay(50); Joystick.setButton(rotaries[i].ccwchar, 0);
    };
    if (result == DIR_CW) {
      Joystick.setButton(rotaries[i].cwchar, 1); delay(50); Joystick.setButton(rotaries[i].cwchar, 0);
    };
  }
}
```


## Using a NANO as 1p12T driver using the Serial Port and a voltage divider.

First off all, we need to do a voltage adaptation from 5V to 3.3V in ESP:

```
                                                                                    x-G26<-ESP32 [Serial TX]
                                                +------------------------------------>G27<-ESP32 [Serial RX]
                                                |                      +------------->GND<-ESP32
                                                |                      |              
[SERIAL TX] NANO->D9------------5V--->[R10K]----x--3.3V--[R20K]------>GND
[SERIAL RX] NANO->D8-x                                                 |
                                                                       |
[D12 INPUT] NANO->D12---------------->[ PUSH BUTTON ]------------------+
[D11 INPUT] NANO->D11---------------->[ PUSH BUTTON ]------------------+
[D10 INPUT] NANO->D10---------------->[ PUSH BUTTON ]------------------+
                                                                       |
            NANO->GND--------------------------------------------------|
```


to use 2.0.5 core instead 1.0.6
Only can use 1.0.6 due the rest is compiling from 2.x.x (mac os 10.14 >)
https://docs.espressif.com/projects/arduino-esp32/en/latest/installing.html
https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

current
https://dl.espressif.com/dl/package_esp32_index.json
http://arduino.esp8266.com/stable/package_esp8266com_index.json