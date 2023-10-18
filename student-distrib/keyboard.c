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

#define BUFFER_SIZE 128

extern void keyboard_link(); 

/* Keyboard variables */
volatile int key_pressed;
volatile int shift_pressed;
volatile int caps_enabled;
volatile int ctrl_pressed;

/* Buffer */
char keyboard_buffer[BUFFER_SIZE];
volatile int buf_ptr;

/*
    * IGNORE
    * what to do on receive keyboard interrupt - simply write to buffer? do i also print to screen?
    * how is buffer printed to screen
    * 
    * keyboard inputs add to buffer
    * ctrl-c sends halt signal (?)
    * ctrl-l clears buffer and clears screen
    * enter prints newline, stores old buffer, clears buffer
    * 
    * read (store previous buffer), prints previous buffer?
    * write (command) directly modifies buffer (?)
    * 
    * normally just print directly to buffer, if read then write to buffer
    * write when you want to print contents of the buffer
    * 
*/

/* Keyboard map */
char key_map[] = {
    'z',   // Not a valid character for index 0
    'z',  // Escape
    '1','2','3','4','5','6','7','8','9','0','-','=',
    '\b \b',   // Backspace
    '    ',   // Tab
    'q', 'w','e','r','t','y','u','i','o','p','[',']',
    '\n',  // Enter
    ' ',  // Left Control
    'a','s','d','f','g','h','j','k','l',';','\'',
    '`',
    ' ',  // Left Shift
    '\\',
    'z','x','c','v','b','n','m',',','.','/',
    ' ',  // Right Shift
    '*',
    ' ',  // Left Alt
    ' ', // Space
    ' ',  // Caps Lock
};

/* Shifted keyboard map. Also used for caps */
char shifted_key_map[] = {
    'z',   // Not a valid character for index 0
    'z',   // escape
    '!', '@','#','$','%','^','&','*','(',')','_','+',
    'z',   // Backspace
    'z',   // Tab
    'Q','W','E','R','T','Y','U','I','O','P','{', '}',  
    'z',   // Enter
    'z',   // Left Control
    'A','S','D','F','G','H','J','K','L',':','"',
    '~',  // Shifted '`'
    'z',   // Left Shift
    '|',  // Shifted '\'
    'Z','X','C','V','B','N','M','<','>','?',
    'z',   // Right Shift
    '*',  // Keypad *
    'z',   // Left Alt
    ' ',
    'z',   // Caps Lock
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
    key_pressed = 0;
    shift_pressed = 0;
    caps_enabled = 0;
    ctrl_pressed = 0;

    /* Enable keyboard interrupt line */
    enable_irq(KEYBOARD_IRQ);
}

/* keyboard_handler
 *   Inputs: none
 *   Return Value: none
 *    Function: Get scankey from keyboard and print to screen */
extern void keyboard_handler() {
    /* start critical section */
    cli();

    /* Get keyboard input */
    uint8_t scan_key = inb(KEYBOARD_DATA_PORT);

    /* Check if modifier is pressed. If so update modifier flag and return */
    if (check_modifiers(scan_key))
        { send_eoi(KEYBOARD_IRQ); sti(); return; }
 
    
    /* Check if invalid scan key */
    if (scan_key > 57) // invalid scan_key
        { sti(); send_eoi(KEYBOARD_IRQ); return; }
    
    /* Is letter (check caps + shift) */
    if (is_letter(scan_key)) {
        
        if(ctrl_pressed && (scan_key == 0x26))  // ctrl-l and ctrl-L clear screen and puts cursor at top
            clear();    //clears screen/videomem
            clear_buffer(); //clears buffer (not implemented yet)
            //set_cursor();   //put cursor at the top (not implemented yet)
        //if(ctrl_pressed && 'c pressed' ) // ctrl-c halts
            //return;

        if (shift_pressed && caps_enabled) // shift + caps negate each other
            printf("%c", key_map[scan_key]);
        else if (!shift_pressed && caps_enabled) // caps lock pressed, so print shifted letter
            printf("%c", shifted_key_map[scan_key]);
        else if (shift_pressed && !caps_enabled) // shift pressed, so print shifted letter
            printf("%c", shifted_key_map[scan_key]);
        else if (!shift_pressed && !caps_enabled) // no shift or caps, so print normal letter
            printf("%c", key_map[scan_key]);
    
    } else { // not a letter (caps does not affect)
        
        if (shift_pressed)      
            printf("%c", shifted_key_map[scan_key]);
        if (caps_enabled)
            printf("%c", key_map[scan_key]);
        else 
            printf("%c", key_map[scan_key]);                            
    
    }
    
    
    
   
    //target remote 10.0.2.2:1234

    /* End critical section and send EOI */
    sti();
    send_eoi(KEYBOARD_IRQ);


}

// start writing to buffer
extern void syscall_read() {
    
}


// prints the buffer
extern void syscall_write() {

}

// clears the keyboard buffer
extern void clear_buffer() {
    uint8_t i;
    for (i=0; i < BUFFER_SIZE; i++){
        keyboard_buffer[i] = '\0';
    }
}


/* check_modifiers
 *   Inputs: scan key
 *   Return Value: 1 if modifier key pressed, 0 otherwise
 *   Function: check if modifier key is pressed, and if so update the corresponding flag */
int check_modifiers(uint8_t scan_key) {
    
    /* Set modifier flags based on scankeys*/
    switch (scan_key) {
        case 0x2A: // Left Shift
            shift_pressed = 1; return 1;
        case 0xAA: // left shift released
            shift_pressed = 0; return 1;
        case 0x36: // right shift pressed
            shift_pressed = 1; return 1;
        case 0xB6: // right shift released
            shift_pressed = 0; return 1;
        case 0x3A: // Caps Lock set
            caps_enabled = (caps_enabled==0) ? 1 : 0; return 1;
        case 0xBA: // caps lock released (do nothing)
            return 1;
        case 0x1D: // control pressed
            ctrl_pressed = 1; return 1;
        case 0x9D: // lcontrol released
            ctrl_pressed = 0; return 1;
        default:
            return 0;
    }
}



/* is_letter
 *   Inputs: scan key
 *   Return Value: 1 if scan key is letter, 0 otherwise
 *   Function: check if the input scan key is a letter  */
int is_letter(uint8_t scan_key) {
    if (    
            (scan_key>=0x10 && scan_key<=0x19) ||       // q-p
            (scan_key>=0x1E && scan_key <= 0x26) ||     // a-l
            (scan_key>=0x2C && scan_key<=0x32)          // z-m
    ) {    
        return 1;
    } else {
        return 0;
    }
}

