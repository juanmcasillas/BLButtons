// https://stackoverflow.com/questions/47981/how-do-i-set-clear-and-toggle-a-single-bit
// https://www.pjrc.com/teensy/td_libs_AltSoftSerial.html
// installed from Library Manager
// on nano, rx 8, tx 9 allways.
// this is the part from NANO (sender)
#include <AltSoftSerial.h>


#define RELEASE 1

#ifndef RELEASE
char line[128]; // for debug
#endif


// NANO lines are: 
/*
    20 lines, so I need 

    D9 Serial TX
    D12
    D11
    D10
    D9
    D8
    D7
    D6
    D5
    D4
    D3
    D2
    D13
    A0
    A1
    A2
    A3
    A4
    A5
    A6 <only analog>
    A7 <only analog>
*/
//32 bits, each position means the button.
//D0,D1-D13,A0-A8

AltSoftSerial altSerial;
unsigned long BUTTONS;
unsigned long PREV_BUTTONS;
#define NUM_INPUTS 32

// POS in BUTTONS map (0-32) if 0, don't configure it
// unsigned char INPUT_PINS[] = {  0,  0,  2,  3,  4,  5,  6,  7, 
//                                 0,  0, 10, 11, 12, 13, A0, A1, 
//                                A2, A3, A4, A5,  0 , 0,  0,  0 ,
//                                 0,  0,  0,  0,  0 , 0,  0,  0  };

unsigned char INPUT_PINS[] = {  0,  0,  2,  3,  4,  5,  6,  7, 
                                0,  0, 10, 11, 12,  0, A0, A1, 
                               A2, A3, A4, A5,  0 , 0,  0,  0 ,
                                0,  0,  0,  0,  0 , 0,  0,  0  };


unsigned long set_bit(unsigned char pos, unsigned char val)  {
    unsigned long mask = 1L << pos;
    BUTTONS = (BUTTONS & ~mask) | (val << pos);
    return (BUTTONS);
}

byte get_bit(unsigned char pos) {
    unsigned long mask = 1L << pos;
    byte value = ( 1 ? (BUTTONS >> pos) & 1L : 0);
    return(value);
}

#ifndef RELEASE
void print_state() {
    Serial.println("\n----------------------------");
    for (int i=1; i<=32; i++) {
        sprintf(line, "input_%d: %d", i, get_bit(i));
        Serial.println(line);
    }
}
#endif


void setup() {
     for (int i=0; i< NUM_INPUTS; i++) {
        if (INPUT_PINS[i] != 0) {
            #ifndef RELEASE
                memset(line, 0, 128);
                sprintf(line,"Init pin: %d",INPUT_PINS[i]);
                Serial.println(line);
            #endif
            pinMode(INPUT_PINS[i], INPUT_PULLUP);        
        }
    }
    BUTTONS = 0;

#ifndef RELEASE
    Serial.begin(115200);
    Serial.println("NanoWriter Started - writting on D9");
#endif
    altSerial.begin(115200);
    delay(1000);    
}



void loop() {
 

    PREV_BUTTONS = BUTTONS;
    
    for (int i=0; i< NUM_INPUTS; i++) {
        if (INPUT_PINS[i] != 0) {
            int input_value = digitalRead(INPUT_PINS[i]);
            set_bit(i, ! input_value);
            #ifndef RELEASE
                if (!input_value) {
                    memset(line, 0, 128);
                    sprintf(line,"button pressed: %d, %d", i, INPUT_PINS[i]);
                    Serial.println(line);
                }
            #endif
        }
    }

    // if buttons changed, send them
    if (PREV_BUTTONS != BUTTONS) {
        #ifndef RELEASE
        print_state();
        #endif
        altSerial.write( (uint8_t *) &BUTTONS, sizeof( BUTTONS ) );
    }
    delay(100);
}
