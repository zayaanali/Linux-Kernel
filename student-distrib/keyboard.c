#include "keyboard.h"
#include "lib.h"
#include "i8259.h"


#define KEYBOARD_IRQ 1
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_CMD_PORT 0x64

#define LEFT_SHIFT_PRESSED 0x12
#define RIGHT_SHIFT_PRESSED 0x59
#define CAPS_LOCK_PRESSED 0x58
#define CAPS_LOCK_RELEASED 0xF0

volatile int key_pressed;
volatile int shift_pressed;
volatile int caps_pressed;

// enable the keyboard interrupt line
extern void keyboard_init() {
    enable_irq(KEYBOARD_IRQ);
}

// If receive interrupt, then read from keyboard port
extern void keyboard_handler() {
    /* Start Protected Segment */
    cli();

    /* Read from keyboard */
    uint8_t scan_key = inb(KEYBOARD_DATA_PORT);




    /* Send EOI */
    send_eoi(KEYBOARD_IRQ);
    
    /* End Protected Segment */
    sti();
}


// check if modifiers held
void check_modifiers(uint8_t scan_key) {
    switch (scan_key) {
        case 0x12: // Left Shift
        case 0x59: // Right Shift
        case 0x3A: // Caps Lock
            return 1;
        default:
            return 0;
    }
}