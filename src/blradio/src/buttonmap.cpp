//////////////////////////////////////////////////////////////////////////////
///
/// @file buttonmap.cpp
/// @author Juan M. Casillas (juanm.casillas@gmail.com)
/// @brief Button map implementation file
/// @version 0.1
/// @date 2022/12/28
///
/// Buttonmap implementation file
/// 
/// @copyright Copyright (c) 12/28/2022 Juan M. Casillas <juanm.casillas@gmail.com>
///
//////////////////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include <Keypad.h>   
#include "rotencoder.h"
#include "signaldefs.h"

unsigned char BUTTON_MAP[] = {
//
// NANO MODE.
// this is used as a mode selector, not a button
// so don't map it here. The number is the button
// number. 0 means no button, just for reference
// to map the table usign the signal ID.
//

0, // S_1P12T_1
0, // S_1P12T_2
0, // S_1P12T_3
0, // S_1P12T_4
0, // S_1P12T_5
0, // S_1P12T_6
0, // S_1P12T_7
0, // S_1P12T_8
0, // S_1P12T_9
0, // S_1P12T_10
0, // S_1P12T_11

// 
// NANO two way switches
//
12, // S_SW2_1_S1
13, // S_SW2_1_S2
14, // S_SW2_2_S1
15, // S_SW2_2_S2

// 
// ESP 32 one way switches
//

16, // S_SW1_1_S1
17, // S_SW1_2_S1
18, // S_SW1_3_S1
19, // S_SW1_4_S1 
20, // S_SW1_5_S1 
21, // S_SW1_6_S1 

//
// buttons
//
1,  // S_KP_1   
2,  // S_KP_2   
3,  // S_KP_3   
4,  // S_KP_4   
5,  // S_KP_5   
6,  // S_KP_6   
7,  // S_KP_7   
8,  // S_KP_8   
9,  // S_KP_9   
10, // S_KP_10

23, // S_KP_11     // ROT5
26, // S_KP_12     // ROT1
29, // S_KP_13     // ROT2
32, // S_KP_14     // ROT3
35, // S_KP_15     // ROT4

0, // S_KP_16     // not used.

//
// rotenc buttons (left, right)
// 

25, // S_ROT_1_LEFT  
27, // S_ROT_1_RIGHT 
28, // S_ROT_2_LEFT  
30, // S_ROT_2_RIGHT 
31, // S_ROT_3_LEFT  
33, // S_ROT_3_RIGHT 
34, // S_ROT_4_LEFT  
36, // S_ROT_4_RIGHT 
22, // S_ROT_5_LEFT  
24  // S_ROT_5_RIGHT 
};