//////////////////////////////////////////////////////////////////////////////
///
/// @file bltools.h
/// @author Juan M. Casillas (juanm.casillas@gmail.com)
/// @brief 
/// @version 0.1
/// @date 2022/12/21
///
///  BL-Tools generic functions and helpers
/// 
/// @copyright Copyright (c) 12/21/2022 Juan M. Casillas <juanm.casillas@gmail.com>
///
//////////////////////////////////////////////////////////////////////////////

#ifndef __BL_TOOLS_H__
#define __BL_TOOLS_H__

#include <Arduino.h>

#define SERIAL_SPEED 57600

#ifndef RELEASE
#define LINE_SIZE 128
extern char line[LINE_SIZE]; // for debug
extern void print_state();
#endif

extern unsigned long BUTTONS;
extern unsigned long PREV_BUTTONS;

#define PACKET_HEADER 0xDE
#define PACKET_FOOTER 0xAD
#define PACKET_SIZE 6 

struct serial_package_s {
    uint8_t header;
    unsigned long data;
    uint8_t footer;
};

typedef serial_package_s serial_package_t;
extern serial_package_t PACKET;



extern unsigned long set_bit(unsigned long pos, unsigned long val);
extern uint8_t get_bit(unsigned long pos);


#endif
