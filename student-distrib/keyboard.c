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

#include "x86_desc.h"

extern void keyboard_link(); 

volatile int key_pressed;
volatile int shift_pressed;
volatile int caps_pressed;

// enable the keyboard interrupt line
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

    enable_irq(KEYBOARD_IRQ);
}

// If receive interrupt, then read from keyboard port
extern void keyboard_handler() {
    /* Start Protected Segment */
//    cli(); //interrupt gate protects for us

    /* Read from keyboard */
    uint8_t scan_key = inb(KEYBOARD_DATA_PORT);

    printf("%d \n", scan_key);


    /* Send EOI */
    send_eoi(KEYBOARD_IRQ);

    
    
    /* End Protected Segment */
//    sti();
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