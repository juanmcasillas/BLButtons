//////////////////////////////////////////////////////////////////////////////
///
/// @file i2cenc.ino
/// @author Juan M. Casillas (juanm.casillas@gmail.com)
/// @brief test the PCF8574 and encoder lib
/// @version 0.1
/// @date 2022/12/22
///
/// Code to test the PCF8574 expansor and some features.
/// P0 is the pin nearest the address jumpers.
/// 
/// @copyright Copyright (c) 12/22/2022 Juan M. Casillas <juanm.casillas@gmail.com>
///
//////////////////////////////////////////////////////////////////////////////
// PCF8574
// https://www.luisllamas.es/mas-pines-digitales-con-arduino-y-el-expansor-es-i2c-pcf8574/
// use this https://github.com/xreef/PCF8574_library/releases
// https://github.com/khoih-prog/ESP32TimerInterrupt
//
// 32 (0x20)
// default jumper connection:
// 32 (0x20) address
// x x x
// x x x
// | | | 
// x x x
//A0   A2
//
// 33 (0x21) address
// x x x
// |
// x x x
//   | | 
// x x x
//A0   A2
//
// 35 (0x23) address
// x x x
// | |
// x x x
//     |  
// x x x
//A0   A2
//

// our working example
//
// 35 (0x23) address
// x x x
// | |
// x x x
//     |  
// x x x
//A0   A2
//

// P0 -> push_button
// P1 -> push_button
// P2 -> push_button

// P3 -> SW
// P4 -> DT
// P5 -> CLK


// #define RELEASE 1
//#define DEBUG 1

#include <Arduino.h>
#include <PCF8574.h>
#include <Ticker.h>
#include <HardwareSerial.h>
#include "rotencoder.h"
#include "bltools.h"


Ticker periodicTicker;


#define EXPANDER_ADDESS 0x23
PCF8574 pcf8574(EXPANDER_ADDESS);



HardwareSerial DriverPort( 1 );
#define PIN_RX 27
#define PIN_TX 26


//
// unsigned char INPUT_PINS[] = {  0,  0,  2,  3,  4,  5,  6,  7, 
//                                 0,  0, 10, 11, 12,  0, A0, A1, 
//                                A2, A3, A4, A5,  0 , 0,  0,  0 ,
//                                 0,  0,  0,  0,  0 , 0,  0,  0  };
//
// SIGNALS [NANO]
// [2] = 1P12T_1
// [3] = 1P12T_2
// [4] = 1P12T_3
// [5] = 1P12T_4
// [6] = 1P12T_5
// [7] = 1P12T_6
// [10] = 1P12T_7
// [11] = 1P12T_8
// [12] = 1P12T_9
// [14] = 1P12T_10
// [15] = 1P12T_11
// [16] = SW2_1_S1
// [17] = SW2_1_S2
// [18] = SW2_2_S1
// [19] = Sw2_2_S2

#define S_1P12T_1    0
#define S_1P12T_2    1
#define S_1P12T_3    2
#define S_1P12T_4    3
#define S_1P12T_5    4
#define S_1P12T_6    5
#define S_1P12T_7    6
#define S_1P12T_8    7
#define S_1P12T_9    8
#define S_1P12T_10   9
#define S_1P12T_11  10

#define S_SW2_1_S1  11
#define S_SW2_1_S2  12
#define S_SW2_2_S1  13
#define S_SW2_2_S2  14

#define S_SW1_6_S1  15  // G2
#define S_SW1_5_S1  16  // G4
#define S_SW1_4_S1  17  // G5
#define S_SW1_3_S1  18  // G12
#define S_SW1_2_S1  19  // G14
#define S_SW1_1_S1  20  // G15

// rotary definitions
#define MAX_ROTARIES 5
RotaryEncoder *encoders[MAX_ROTARIES];
#define ROT5_PINA 35
#define ROT5_PINB 34
#define ROT1    0
#define ROT2    1
#define ROT3    2
#define ROT4    3
#define ROT5    4


unsigned char SIGNALS[21] = {0};



//
// refresh the encoders to get the value.
//
void timer_isr() {
    for (int i=0; i< MAX_ROTARIES; i++) {
        encoders[i]->service();
    }
}



void setup() {
    #ifndef RELEASE	  
    Serial.begin (115200);
    Serial.println("i2cTEST Started");
    #endif
    // serial port
    DriverPort.begin(SERIAL_SPEED, SERIAL_8N1, PIN_RX, PIN_TX );
    delay(500);


    // PINS ARE SWAPPED IN ORDER TO WORK IN THE WIRE.
    // p3 connected to wire B
    // p4 connected to wire A
    // if you invert the wires, doesn't work. Check it.

    // define the 6 encoders. 
    // CLK(A), DT(B), SW
    encoders[ROT1] = new RotaryEncoder(&pcf8574, P1, P0, -1);
    encoders[ROT2] = new RotaryEncoder(&pcf8574, P2, P3, -1);
    encoders[ROT3] = new RotaryEncoder(&pcf8574, P4, P5, -1);
    encoders[ROT4] = new RotaryEncoder(&pcf8574, P6, P7, -1);
    encoders[ROT5] = new RotaryEncoder(ROT5_PINB, ROT5_PINA, -1);

    //pinMode(ROT5_PINA,INPUT_PULLUP);
    //pinMode(ROT5_PINB,INPUT_PULLUP);

    // Interval in microsecs
    // Set low latency with this method or uncomment LOW_LATENCY define in the library
    // Needed for encoder

    pcf8574.setLatency(0);
    // Start library
    #ifndef RELEASE	  
    if (pcf8574.begin()){
        Serial.println("pcf8574 lib ok");
    }else{
        Serial.println("pcf8574 lib ko");
    }
    #endif
    
    periodicTicker.attach_ms(1, timer_isr);

}



long prev_value = 0;

void loop() {

    PREV_BUTTONS = BUTTONS;

    // 
    // read the data from serial This must be encapsulated in a function.
    // 
    if ( DriverPort.available() >= PACKET_SIZE ) {
        uint8_t data[6]; // size of serial_package_s;

        for (int i=0; i< PACKET_SIZE; i++) {
            data[i] = DriverPort.read();
        }
        PACKET.header = data[0];
        PACKET.data   = data[1] | (data[2] << 8) | (data[3] << 16) | (data[4] << 24);
        PACKET.footer = data[5];

        if (PACKET.header == 0xDE && PACKET.footer == 0xAD) {
            BUTTONS = PACKET.data;
        }
        else {
            // some problem with the serial port.
        }
    }

    if (PREV_BUTTONS != BUTTONS) {
        #ifndef RELEASE
        print_state();
        #endif
    }

    // the IC part here.
    //uint8_t p0 = pcf8574.digitalRead(ROT5_PINA);
    //uint8_t p1 = pcf8574.digitalRead(ROT5_PINB);
    //uint8_t p2 = pcf8574.digitalRead(P2);
    //uint8_t p6 = pcf8574.digitalRead(P6);
    //uint8_t p7 = pcf8574.digitalRead(P7);
    //p0 = ( p0 == 0 ? 1 : 0); // not wired
    //p1 = ( p1 == 0 ? 1 : 0);
    //p2 = ( p2 == 0 ? 1 : 0); 
    //p6 = ( p6 == 0 ? 1 : 0); 
    // p7 = ( p7 == 0 ? 1 : 0); 

    // not connected in hw
    // if (p0) {
    //     memset(line, 0, 128);
    //     sprintf(line,"button pressed: %d, [GPIO %d]", 0, P0);
    //     Serial.println(line);
    // }
    // if (p1) {
    //     memset(line, 0, 128);
    //     sprintf(line,"button pressed: %d, [GPIO %d]", 1, P1);
    //     Serial.println(line);
    // }
    // if (p2) {
    //     memset(line, 0, 128);
    //     sprintf(line,"button pressed: %d, [GPIO %d]", 2, P2);
    //     Serial.println(line);
    // }
    // if (p6) {
    //     memset(line, 0, 128);
    //     sprintf(line,"button pressed: %d, [GPIO %d]", 6, P6);
    //     Serial.println(line);
    // }
    // if (p7) {
    //     memset(line, 0, 128);
    //     sprintf(line,"button pressed: %d, [GPIO %d]", 7, P7);
    //     Serial.println(line);
    // }

    for (int i=0; i< MAX_ROTARIES; i++) {
        if (encoders[i]->getButton() == RotaryEncoder::PRESSED) {
            #ifndef RELEASE
                memset(line, 0, 128);
                sprintf(line,"encoder #%d button pressed", i+1);
                Serial.println(line);
            #endif
        }

        switch (encoders[i]->getDirection()) {
            case RotaryEncoder::RIGHT:
                #ifndef RELEASE
                    memset(line, 0, 128);
                    sprintf(line,"encoder #%d CW (Right)", i+1);
                    Serial.println(line);
                #endif
                break;
            case RotaryEncoder::LEFT:
                #ifndef RELEASE
                    memset(line, 0, 128);
                    sprintf(line,"encoder #%d CCW (Left)", i+1);
                    Serial.println(line);
                #endif
                break;
        }
    }

    // long value = encoder->getValue();
    // if (value != prev_value) {
    //     Serial.println(value);
    //     prev_value = value;
    // }


    delay(100);
}


