//////////////////////////////////////////////////////////////////////////////
///
/// @file signaldefs.h
/// @author Juan M. Casillas (juanm.casillas@gmail.com)
/// @brief define the signals and the mappings
/// @version 0.1
/// @date 2022/12/28
///
/// Define the signals and the mappings
/// 
/// @copyright Copyright (c) 12/28/2022 Juan M. Casillas <juanm.casillas@gmail.com>
///
//////////////////////////////////////////////////////////////////////////////

#ifndef __SIGNALDEFS_H__
#define __SIGNALDEFS_H__

//
// define the signals that comes from the NANO via serial port
//
#define S_1P12T_1    0
#define S_1P12T_2    1
#define S_1P12T_3    2
#define S_1P12T_4    3
#define S_1P12T_5    4
#define S_1P12T_6    5
#define S_1P12T_7    6
#define S_1P12T_8    7
#define S_1P12T_9    8
#define S_1P12T_10   9
#define S_1P12T_11  10

#define S_SW2_1_S1  11
#define S_SW2_1_S2  12
#define S_SW2_2_S1  13
#define S_SW2_2_S2  14

//
// maps the position on BUTTONS (bits) to signals
//
extern unsigned char NANO_BITS[];

//
// ESP 1way Switches
//
#define S_SW1_1_S1  15  // G15
#define S_SW1_2_S1  16  // G14
#define S_SW1_3_S1  17  // G12
#define S_SW1_4_S1  18  // G5
#define S_SW1_5_S1  19  // G4
#define S_SW1_6_S1  20  // G2

//
// define the keypad signals.
//
#define S_KP_1   21
#define S_KP_2   22
#define S_KP_3   23
#define S_KP_4   24
#define S_KP_5   25
#define S_KP_6   26
#define S_KP_7   27
#define S_KP_8   28
#define S_KP_9   29
#define S_KP_10  30
#define S_KP_11  31
#define S_KP_12  32
#define S_KP_13  33
#define S_KP_14  34
#define S_KP_15  35
#define S_KP_16  36

//
// define the rotary encoders buttons [left, right]
//
#define S_ROT_1_LEFT  37
#define S_ROT_1_RIGHT 38
#define S_ROT_2_LEFT  39
#define S_ROT_2_RIGHT 40
#define S_ROT_3_LEFT  41
#define S_ROT_3_RIGHT 42
#define S_ROT_4_LEFT  43
#define S_ROT_4_RIGHT 44
#define S_ROT_5_LEFT  45
#define S_ROT_5_RIGHT 46

//
// rotary definitions
//
#define MAX_ROTARIES 5
extern RotaryEncoder *encoders[MAX_ROTARIES];
#define ROT1    0
#define ROT2    1
#define ROT3    2
#define ROT4    3
#define ROT5    4

#define ROT_LEFT  0
#define ROT_RIGHT 1
extern unsigned char rotenc_map[MAX_ROTARIES][2];


//
// keypad matrix.
//
#define KP_ROWS 4
#define KP_COLS 4

extern uint8_t kp_rowPins[KP_ROWS];
extern uint8_t kp_colPins[KP_COLS];

extern uint8_t keymap[KP_ROWS][KP_COLS];
extern Keypad customKeypad;

extern uint8_t keymap_signals[];

#define MAX_SIGNALS 47
extern unsigned char SIGNALS[MAX_SIGNALS];
extern unsigned char PREV_SIGNALS[MAX_SIGNALS];

#endif