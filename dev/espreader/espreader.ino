#include <HardwareSerial.h>


// #define RELEASE 1

#ifndef RELEASE
char line[128]; // for debug
#endif


HardwareSerial DriverPort( 1 );
#define PIN_RX 27
#define PIN_TX 26

unsigned long BUTTONS;
unsigned long PREV_BUTTONS;

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
        memset(line, 0, 128);
        sprintf(line, "input_%d: %d", i, get_bit(i));
        Serial.println(line);
    }
}
#endif 

void setup() {
#ifndef RELEASE
    Serial.begin(115200);
    Serial.println("ESPreader Started");
#endif
    DriverPort.begin(115200, SERIAL_8N1, PIN_RX, PIN_TX );
    delay(1000);
}






void loop() {
    PREV_BUTTONS = BUTTONS;
    if ( DriverPort.available() >= sizeof(BUTTONS) ) {
        char data[4];
        for (int i=0; i< sizeof(BUTTONS); i++) {
            data[i] = DriverPort.read();
        }
        BUTTONS = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
    }
    if (PREV_BUTTONS != BUTTONS) {
        #ifndef RELEASE
        print_state();
        #endif
    }
    delay(100);
}

