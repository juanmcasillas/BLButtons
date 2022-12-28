//////////////////////////////////////////////////////////////////////////////
///
/// @file pindefs.h
/// @author Juan M. Casillas (juanm.casillas@gmail.com)
/// @brief pin definitions
/// @version 0.1
/// @date 2022/12/28
///
/// PIN definition header
/// 
/// @copyright Copyright (c) 12/28/2022 Juan M. Casillas <juanm.casillas@gmail.com>
///
//////////////////////////////////////////////////////////////////////////////

#ifndef __PINDEFS_H__
#define __PINDEFS_H__

// 
// PIN DEFINITIONS
//

// pins for the extra rotary encoder
#define PIN_ROT5_A 16
#define PIN_ROT5_B 26

// pins for the serial port (only use the TX)
#define PIN_SERIAL_RX 27
#define PIN_SERIAL_TX 26

// pins for the 1-way switches
#define PIN_SW1_1_S1 15
#define PIN_SW1_2_S1 14
#define PIN_SW1_3_S1 12
#define PIN_SW1_4_S1 5
#define PIN_SW1_5_S1 4
#define PIN_SW1_6_S1 2

// Keypad row & col PINS
#define PIN_KP_ROW_0 13
#define PIN_KP_ROW_1 25
#define PIN_KP_ROW_2 33
#define PIN_KP_ROW_3 32

#define PIN_KP_COL_0 17
#define PIN_KP_COL_1 18
#define PIN_KP_COL_2 19
#define PIN_KP_COL_3 23

//
// use the pin definitions for each service.
//

#define PIN_RX PIN_SERIAL_RX
#define PIN_TX PIN_SERIAL_TX

#define ROT5_PINA PIN_ROT5_A
#define ROT5_PINB PIN_ROT5_B // THE SAME as PIN_TX (see setup())

#endif