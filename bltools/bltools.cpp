#include "bltools.h"


unsigned long BUTTONS = 0;
unsigned long PREV_BUTTONS = 0;

serial_package_s serial_package_default = { PACKET_HEADER, 0, PACKET_FOOTER};
serial_package_t PACKET = serial_package_default;

#ifndef RELEASE
char line[128]; // for debug
void print_state() {
    Serial.println("\n----------------------------");
    for (int i=1; i<=32; i++) {
        memset(line, 0, 128);
        sprintf(line, "input_%d: %d", i, get_bit(i));
        Serial.println(line);
    }
}
#endif 

unsigned long set_bit(unsigned long pos, unsigned long val)  {
    unsigned long int mask = (unsigned long)1 << pos;

    BUTTONS = (BUTTONS & ~mask) |  (val << pos);

    #ifndef RELEASE
    #ifdef DEBUG
        memset(line, 0, 128);
        sprintf(line,"set_bit %d -> %d [%lu] [%lu]", pos, val, BUTTONS, mask);
        Serial.println(line);
    #endif
    #endif
    
    return (BUTTONS);
}

uint8_t get_bit(unsigned long pos) {
    uint8_t value = ( 1 ? (BUTTONS >> pos) & 1UL : 0);
    return(value);
}


