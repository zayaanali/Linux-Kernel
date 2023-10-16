/* keyboard.c - functions to interact with the keyboard */


#include "keyboard.h"
#include "lib.h"
#include "i8259.h"
#include "x86_desc.h"

#define KEYBOARD_IRQ 1
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_CMD_PORT 0x64

#define LEFT_SHIFT_PRESSED 0x12
#define RIGHT_SHIFT_PRESSED 0x59
#define CAPS_LOCK_PRESSED 0x58
#define CAPS_LOCK_RELEASED 0xF0

extern void keyboard_link(); 

/* Keyboard variables */
volatile int key_pressed;
volatile int shift_pressed;
volatile int caps_pressed;
int pressed; 

char key_map[] = {
    'z',   // Not a valid character for index 0
    'z',  // Escape
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    '0',
    '-',
    '=',
    'z',   // Backspace
    'z',   // Tab
    'q',
    'w',
    'e',
    'r',
    't',
    'y',
    'u',
    'i',
    'o',
    'p',
    '[',
    ']',
    'z',  // Enter
    'z',  // Left Control
    'a',
    's',
    'd',
    'f',
    'g',
    'h',
    'j',
    'k',
    'l',
    ';',
    '\'',
    '`',
    'z',  // Left Shift
    '\\',
    'z',
    'x',
    'c',
    'v',
    'b',
    'n',
    'm',
    ',',
    '.',
    '/',
    'z',  // Right Shift
    '*',
    'z',  // Left Alt
    ' ',
    'z',  // Caps Lock
    'z', // f0 through f10
    'z',
    'z',
    'z',
    'z',
    'z',
    'z',
    'z',
    'z',
    'z',
    'z',   // Num Lock
    'z',   // Scroll Lock
    '7', // Home (7 on the numpad)
    '8', // Up Arrow (8 on the numpad)
    '9', // Page Up (9 on the numpad)
    '-', // Keypad -
    '4', // Left Arrow (4 on the numpad)
    '5', // Keypad 5
    '6', // Right Arrow (6 on the numpad)
    '+', // Keypad +
    '1', // End (1 on the numpad)
    '2', // Down Arrow (2 on the numpad)
    '3', // Page Down (3 on the numpad)
    '0', // Insert (0 on the numpad)
    '.', // Delete (Del on the numpad)
    'z',   // Not a valid character for indexes beyond 82
};

/* keyboard_init
 *   Inputs: none
 *   Return Value: none
 *    Function: Initialize keyboard */
extern void keyboard_init() {

    // put interrupt gate in idt
    // set reserved0|D|reserved1|reserved2|reserved3|reserved4[8] to 0|1|1|1|0|0000 0000 for 32-bit interrupt gate
    idt[33].present = 1;             // segment is present
    idt[33].dpl = 0;                 // DPL = 00 (highest priority)
    idt[33].reserved0 = 0;           
    idt[33].size = 1;                // size (D) = 1 (32 bit gate)
    idt[33].reserved1 = 1;
    idt[33].reserved2 = 1;
    idt[33].reserved3 = 0;
    idt[33].reserved4 = 0;
    idt[33].seg_selector = KERNEL_CS;

    // link idt entry to rtc liner function
    SET_IDT_ENTRY(idt[33], keyboard_link);

    /* Init keyboard variables */
    pressed = 0; 

    /* Enable keyboard interrupt line */
    enable_irq(KEYBOARD_IRQ);
}

// If receive interrupt, then read from keyboard port
extern void keyboard_handler() {
    pressed++; 

    /* Get keyboard input */
    uint8_t scan_key = inb(KEYBOARD_DATA_PORT);

    /* Print keyboard output */
    if(pressed%2==1){
        printf("%c ", key_map[scan_key]);
    }


    /* Send EOI */
    send_eoi(KEYBOARD_IRQ);


}


/* check_modifiers
 *   Inputs: none
 *   Return Value: none
 *    Function: check which modifiers are held and update keyboard vars */
void check_modifiers(uint8_t scan_key) {
    // switch (scan_key) {
    //     case 0x12: // Left Shift
    //     case 0x59: // Right Shift
    //     case 0x3A: // Caps Lock
    //         return 1;
    //     default:
    //         return 0;
    // }
}

