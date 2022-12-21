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

uint8_t get_bit(unsigned long pos) {
    uint8_t value = ( 1 ? (BUTTONS >> pos) & 1UL : 0);
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
    DriverPort.begin(57600, SERIAL_8N1, PIN_RX, PIN_TX );
    delay(1000);
}


// Arduino sends 6 bytes (the size of the structure ... there) but here, it 
// doubles it (due its a 64 bit architecture, instead of 32). It "works" 
// because mixes the first package with the second. Fixed the size, it 
// works fine.
// define packet size ... in bytes.

#define PACKET_HEADER 0xDE
#define PACKET_FOOTER 0xAD
#define PACKET_SIZE 6 

struct serial_package_s {
    uint8_t header;
    unsigned long data;
    uint8_t footer;
} serial_package_default = { PACKET_HEADER, 0, PACKET_FOOTER};

typedef serial_package_s serial_package_t;
serial_package_t PACKET = serial_package_default;



void loop() {
    
    PREV_BUTTONS = BUTTONS;
    if ( DriverPort.available() >= PACKET_SIZE ) {
        uint8_t data[6]; // size of serial_package_s;

        for (int i=0; i< PACKET_SIZE; i++) {
            data[i] = DriverPort.read();
        }
        PACKET.header = data[0];
        PACKET.data   = data[1] | (data[2] << 8) | (data[3] << 16) | (data[4] << 24);
        PACKET.footer = data[5];

        // memset(line, 0, 128);
        // sprintf(line, "%x, %x, %x, %x, %x, %x",data[0],data[1],data[2],data[3],data[4],data[5]);
        // Serial.println(line);         

        // memset(line, 0, 128);
        // sprintf(line, "%x, %lu, %x",PACKET.header, PACKET.data, PACKET.footer);
        // Serial.println(line);      

        // Serial.println(sizeof(uint8_t));    
        // Serial.println(sizeof(PACKET));    
        // Serial.println(sizeof(serial_package_s));    
        // Serial.println(sizeof(uint8_t));
        
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
    delay(100);
}

