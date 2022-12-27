#include "bltools.h"


unsigned long BUTTONS = 0;
unsigned long PREV_BUTTONS = 0;

serial_package_s serial_package_default = { PACKET_HEADER, 0, PACKET_FOOTER};
serial_package_t PACKET = serial_package_default;

#ifndef RELEASE
char line[LINE_SIZE]; // for debug
void print_state() {
    Serial.println("\n----------------------------");
    for (int i=1; i<=32; i++) {
        memset(line, 0, LINE_SIZE);
        sprintf(line, "input_%d: %d", i, get_bit(i));
        Serial.println(line);
    }
}
#endif 

unsigned long set_bit_data(unsigned long data, unsigned long pos, unsigned long val)  {
    unsigned long int mask = (unsigned long)1 << pos;

    data = (data & ~mask) |  (val << pos);

    #ifndef RELEASE
    #ifdef DEBUG
        memset(line, 0, LINE_SIZE);
        sprintf(line,"set_bit %d -> %d [%lu] [%lu]", pos, val, data, mask);
        Serial.println(line);
    #endif
    #endif
    
    return (data);
}

uint8_t get_bit_data(unsigned long data, unsigned long pos) {
    uint8_t value = ( (data >> pos) & 1UL ? 1 : 0);
    return(value);
}

unsigned long set_bit(unsigned long pos, unsigned long val)  {
    unsigned long int mask = (unsigned long)1 << pos;

    BUTTONS = set_bit_data(BUTTONS, pos, val);
    return BUTTONS;
}

uint8_t get_bit(unsigned long pos) {
    return( get_bit_data(BUTTONS, pos) );
}
