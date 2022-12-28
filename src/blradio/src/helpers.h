//////////////////////////////////////////////////////////////////////////////
///
/// @file helpers.h
/// @author Juan M. Casillas (juanm.casillas@gmail.com)
/// @brief Misc function helpers
/// @version 0.1
/// @date 2022/12/28
///
/// Misc function helpers
/// 
/// @copyright Copyright (c) 12/28/2022 Juan M. Casillas <juanm.casillas@gmail.com>
///
//////////////////////////////////////////////////////////////////////////////

#ifndef __HELPERS_H__
#define __HELPERS_H__

#include "devel.h"

extern unsigned char selector_map(unsigned long buttons);
extern void timer_isr();
extern uint8_t button_mapping(uint8_t signal_num);
extern void send_buttons();
extern void pressKey(uint8_t key);
extern void releaseKey(uint8_t key);
extern void keypadEvent(KeypadEvent key);

extern word SELECTOR;

#ifndef RELEASE
extern void print_signals();
#endif

#endif