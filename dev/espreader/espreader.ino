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

unsigned long  set_bit(unsigned long pos, unsigned long val)  {
    unsigned long int mask = (unsigned long)1 << pos;

    BUTTONS = (BUTTONS & ~mask) |  (val << pos);

    #ifndef RELEASE
        memset(line, 0, 128);
        sprintf(line,"set_bit %d -> %d [%lu] [%lu]", pos, val, BUTTONS, mask);
        Serial.println(line);
    #endif
    
    return (BUTTONS);
}

byte get_bit(unsigned long pos) {
    byte value = ( 1 ? (BUTTONS >> pos) & 1UL : 0);
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


struct serial_package_s {
    byte header;
    unsigned long data;
    byte footer;
} serial_package_default = { 0xDE, 0, 0xAD};

typedef serial_package_s serial_package_t;
serial_package_t PACKET = serial_package_default;




void loop() {
    
    PREV_BUTTONS = BUTTONS;
    if ( DriverPort.available() >= sizeof(serial_package_t) ) {
        char data[sizeof(serial_package_t)]; // size of serial_package_s;

        for (int i=0; i< sizeof(serial_package_t); i++) {
            data[i] = DriverPort.read();
        }
        memcpy(&PACKET, data, sizeof(serial_package_t));
        if (PACKET.header == 0xDE && PACKET.footer == 0xAD) {
            BUTTONS = PACKET.data;
        }
        else {
        }
            memset(line, 0, 128);
            sprintf(line, "%x, %x, %x, %x, %x, %x",data[0],data[1],data[2],data[3],data[4],data[5]);
            Serial.println(line);

        //BUTTONS = data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
    }
    if (PREV_BUTTONS != BUTTONS) {
        #ifndef RELEASE
        print_state();
        #endif
    }
    delay(100);
}

