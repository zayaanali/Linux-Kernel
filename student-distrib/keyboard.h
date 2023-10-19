/* keyboard.h - Defines used in interactions with the keyboard
 */


#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "types.h"

#define MAX_BUFFER_SIZE 128

/* Buffer (extra character for null termination) */
char line_buffer[MAX_BUFFER_SIZE];
int buf_ptr;


extern void keyboard_init();
extern void keyboard_handler();
int check_modifiers(uint8_t scan_key);
int is_letter(uint8_t scan_key);
extern int read_line_buffer(char terminal_buffer[], int num_bytes);
extern void clear_line_buffer();
void buf_push(char val);
void buf_pop();


#endif
