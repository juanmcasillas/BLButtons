//////////////////////////////////////////////////////////////////////////////
///
/// @file bltools.cpp
/// @author Juan M. Casillas (juanm.casillas@gmail.com)
/// @brief BL-Tools generic functions and helpers
/// @version 0.1
/// @date 2022/12/28
///
///  BL-Tools generic functions and helpers (implmentation)
/// 
/// @copyright Copyright (c) 12/28/2022 Juan M. Casillas <juanm.casillas@gmail.com>
///
//////////////////////////////////////////////////////////////////////////////
#include "bltools.h"

unsigned long BUTTONS = 0;                  // stores the signals in bitmap
unsigned long PREV_BUTTONS = 0;             // previous state

serial_package_s serial_package_default = { PACKET_HEADER, 0, PACKET_FOOTER};
serial_package_t PACKET = serial_package_default;

#ifndef RELEASE
char line[LINE_SIZE]; // for debug
/**
 * @brief print the content of the BUTTONS global variable
 * 
*/
void print_state() {
    Serial.println("\n----------------------------");
    for (int i=1; i<=32; i++) {
        memset(line, 0, LINE_SIZE);
        sprintf(line, "input_%d: %d", i, get_bit(i));
        Serial.println(line);
    }
}
#endif 

/**
 * @brief Set the bit data object
 * 
 * @param data the bitmap field
 * @param pos  the position, counting from right (lsb)
 * @param val  what to store (0,1)
 * @return unsigned long the bitmap field
*/
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

/**
 * @brief Get the bit data object
 * 
 * @param data the bitmap field
 * @param pos  the position, counting from right (lsb)
 * @return uint8_t the value (0,1)
*/
uint8_t get_bit_data(unsigned long data, unsigned long pos) {
    uint8_t value = ( (data >> pos) & 1UL ? 1 : 0);
    return(value);
}

/**
 * @brief Set the bit object same function as set_bit_data()
 * 
 * @param pos the position, counting from right (lsb)
 * @param val  
 * @return unsigned long 
*/
unsigned long set_bit(unsigned long pos, unsigned long val)  {
    unsigned long int mask = (unsigned long)1 << pos;

    BUTTONS = set_bit_data(BUTTONS, pos, val);
    return BUTTONS;
}

/**
 * @brief Get the bit object, same function as get_bit()
 * 
 * @param pos  the position, counting from right (lsb)
 * @return uint8_t  the value (0,1)
*/
uint8_t get_bit(unsigned long pos) {
    return( get_bit_data(BUTTONS, pos) );
}
