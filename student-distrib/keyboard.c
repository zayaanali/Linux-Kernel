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
volatile int caps_enabled;
volatile int ctrl_pressed;
volatile int enter_pressed;



/*
    * TO DO
    * Add backspace function
    * fix tab functionality
    *  
*/

/* Keyboard map */
char key_map[] = {
    'z',   // Not a valid character for index 0
    'z',  // Escape
    '1','2','3','4','5','6','7','8','9','0','-','=',
    '\b',   // Backspace
    ' ',   // Tab
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
    buf_ptr = 0;

    /* Enable keyboard interrupt line */
    enable_irq(KEYBOARD_IRQ);

    /* Enable cursor */
    //enable_cursor(0,80);
}

/* keyboard_handler
 *   Inputs: none
 *   Return Value: none
 *    Function: Get scankey from keyboard and print to screen */
extern void keyboard_handler() {
    char out;
    int i;
    
    /* start critical section */
    

    /* Get keyboard input */
    uint8_t scan_key = inb(KEYBOARD_DATA_PORT);

    /* Check if modifier is pressed. If so update modifier flag and return */
    if (check_modifiers(scan_key))
        { send_eoi(KEYBOARD_IRQ);  return; }
 
    /* Check if invalid scan key */
    if (scan_key > 57) // invalid scan_key
        { send_eoi(KEYBOARD_IRQ); return; }
            
    /* Check for tab */
    if (scan_key == 0x0F) {
        for (i=0; i<4; i++)
            { putc(' '); buf_push(' '); }
        send_eoi(KEYBOARD_IRQ); return;
    } 
    
    /* Backspace */
    if (scan_key == 0x0E) {
        buf_pop();
        putc('\b');
        send_eoi(KEYBOARD_IRQ); return;
    }
    
    /* CTRL functions */
    if (ctrl_pressed) {
        if (scan_key == 0x26) { // CTRL + L
            clear(); clear_line_buffer(); set_cursor(0,0); 
            send_eoi(KEYBOARD_IRQ); return;    
        }
        
        if (scan_key == 0x2E) // CTRL + C
            { putc('^'); putc('c'); send_eoi(KEYBOARD_IRQ); return; }
    }    

    /* Set key to be printed hjhj*/    
    if (is_letter(scan_key)) { // is a letter (both caps and shift affect output)
        
        if (shift_pressed && caps_enabled) // shift + caps negate each other
            out = key_map[scan_key];
        else if (!shift_pressed && caps_enabled) // caps lock pressed, so print shifted letter
            out = shifted_key_map[scan_key];
        else if (shift_pressed && !caps_enabled) // shift pressed, so print shifted letter
            out = shifted_key_map[scan_key];
        else if (!shift_pressed && !caps_enabled) // no shift or caps, so print normal letter
            out = key_map[scan_key];
    
    } else { // not a letter (shift affects output, caps does not)
        
        if (shift_pressed)      
            out = shifted_key_map[scan_key];
        else if (caps_enabled)
            out = key_map[scan_key];
        else 
            out = key_map[scan_key];            
    
    }
// target remote 10.0.2.2:1234
    /* Push character to line buffer and print to screen */
    buf_push(out);
    putc(out);


    /* End critical section and send EOI */
    send_eoi(KEYBOARD_IRQ);
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
        case 0x1C: // enter pressed
            enter_pressed = 1; return 0;
        case 0x9C: // enter released
            enter_pressed = 0; return 0;
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

// clears the line buffer
extern void clear_line_buffer() {
    int i;
    /* Set all elements of line buffer to null*/
    for (i=0; i < MAX_BUFFER_SIZE; i++) {
        line_buffer[i] = '\0';
    }
}

// get keyboard buffer, copy into passed array
extern int read_line_buffer(char terminal_buffer[], int num_bytes) {
    int i, num_bytes_read = 0;
    
    /* Wait for enter keypress */
    //printf("enter_pressed=%d", enter_pressed);
    enter_pressed=0;
    while (enter_pressed==0);
    
    /* Copy keyboard buffer into passed pointer. Protect read into line buffer */

    for (i=0; i < num_bytes; i++) {
        /* Check if reach null termination */
        if (line_buffer[i] == '\0') 
            { terminal_buffer[i] = '\0'; return num_bytes_read; }
        
        /* Copy from line buffer to terminal buffer */
        terminal_buffer[i] = line_buffer[i];
        
        /* Increment number of bytes read */
        num_bytes_read++;
    }

    clear_line_buffer();
    buf_ptr=0;
    

    /* Return number of bytes read */
    return num_bytes_read;
}

void buf_push(char val) {
    if (buf_ptr+1 < MAX_BUFFER_SIZE)
        { line_buffer[buf_ptr]= val; buf_ptr++; line_buffer[buf_ptr] = '\n'; }
}

void buf_pop() {
    if (buf_ptr-1 > -1)
        { line_buffer[buf_ptr]='\n'; buf_ptr--; }
}

/*
*   Function: set_cursor
*   Desc: update the cursor's location 
*   Input: x, y positions
*   Output: None
*/
extern void set_cursor(int x, int y)
{
	uint16_t pos = y * TERM_WIDTH + x;
 
	outb(0x0F, 0x3D4);
	outb((uint8_t) (pos & 0xFF),0x3D5);
	outb(0x0E,0x3D4);
	outb((uint8_t) ((pos >> 8) & 0xFF), 0x3D5);
}


/*
*   Function: enable_cursor
*   Desc: Enabling the cursor also allows you to set the start and end scanlines, 
*         the rows where the cursor starts and ends. The highest scanline is 0 and 
*         the lowest scanline is the maximum scanline (usually 15).
*           (Will most likely be using this for different terminals in CP3)
*   Input: cursor_start, cursor_end
*   Output: None
*/
extern void enable_cursor(uint8_t cursor_start, uint8_t cursor_end)
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);
	outb(0x3D4, 0x0B);
	outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

/*
*   Function: disable_cursor
*   Desc: disables the cursor
*   Input: None
*   Output: None
*/
extern void disable_cursor()
{
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

