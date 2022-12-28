//////////////////////////////////////////////////////////////////////////////
///
/// @file signaldefs.cpp
/// @author Juan M. Casillas (juanm.casillas@gmail.com)
/// @brief Implements the data structures
/// @version 0.1
/// @date 2022/12/28
///
/// Implements the data structures
/// 
/// @copyright Copyright (c) 12/28/2022 Juan M. Casillas <juanm.casillas@gmail.com>
///
//////////////////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include <Keypad.h>   

#include "devel.h"
#include "helpers.h"
#include "rotencoder.h"
#include "pindefs.h"
#include "signaldefs.h"



//
// maps the position on BUTTONS (bits) to signals
//
unsigned char NANO_BITS[] {
    2,    //S_1P12T_1 
    3,    //S_1P12T_2 
    4,    //S_1P12T_3 
    5,    //S_1P12T_4 
    6,    //S_1P12T_5 
    7,    //S_1P12T_6 
   10,    //S_1P12T_7 
   11,    //S_1P12T_8 
   12,    //S_1P12T_9 
   14,    //S_1P12T_10
   15,    //S_1P12T_11
   16,    //S_SW2_1_S1
   17,    //S_SW2_1_S2
   18,    //S_SW2_2_S1
   19     //S_SW2_2_S2
};

//
// rotary definitions
//
RotaryEncoder *encoders[MAX_ROTARIES];

unsigned char rotenc_map[MAX_ROTARIES][2] = {
    { S_ROT_1_LEFT, S_ROT_1_RIGHT },
    { S_ROT_2_LEFT, S_ROT_2_RIGHT },
    { S_ROT_3_LEFT, S_ROT_3_RIGHT },
    { S_ROT_4_LEFT, S_ROT_4_RIGHT },
    { S_ROT_5_LEFT, S_ROT_5_RIGHT },
};


//
// keypad matrix.
//
uint8_t kp_rowPins[KP_ROWS] = {PIN_KP_ROW_0, PIN_KP_ROW_1, PIN_KP_ROW_2, PIN_KP_ROW_3};
uint8_t kp_colPins[KP_COLS] = {PIN_KP_COL_0, PIN_KP_COL_1, PIN_KP_COL_2, PIN_KP_COL_3};

uint8_t keymap[KP_ROWS][KP_COLS] = {
    { 6,  5,   4,  3},
    {10,  9,   8,  7},
    {15,  14, 13, 12},
    { 2,  1,  11, 16}   // 16 last one not phisically connected.
};
Keypad customKeypad = Keypad(makeKeymap(keymap), kp_rowPins, kp_colPins, KP_ROWS, KP_COLS);

uint8_t keymap_signals[] = {
    0,      // not used, only for aligment
    S_KP_1  ,
    S_KP_2  ,
    S_KP_3  ,
    S_KP_4  ,
    S_KP_5  ,
    S_KP_6  ,
    S_KP_7  ,
    S_KP_8  ,
    S_KP_9  ,
    S_KP_10 ,
    S_KP_11 ,   // rotenc 5
    S_KP_12 ,   // rotenc 1
    S_KP_13 ,   // rotenc 2
    S_KP_14 ,   // rotenc 3
    S_KP_15 ,   // rotenc 4
    S_KP_16 // not used
};

//
// signal container
// 
unsigned char SIGNALS[MAX_SIGNALS]      = { 0 };
unsigned char PREV_SIGNALS[MAX_SIGNALS] = { 0 };
