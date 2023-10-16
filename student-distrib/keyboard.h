/* keyboard.h - Defines used in interactions with the keyboard
 */


#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "types.h"

extern void keyboard_init();
extern void keyboard_handler();
void check_modifiers(uint8_t scan_key);

#endif
