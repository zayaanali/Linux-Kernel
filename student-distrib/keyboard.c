/* keyboard.c - functions to interact with the keyboard */


#include "keyboard.h"
#include "lib.h"
#include "i8259.h"
#include "x86_desc.h"
#include "systemcall.h"
#include "terminal.h"


#define KEYBOARD_IRQ 1
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_CMD_PORT 0x64

#define LEFT_SHIFT_PRESSED 0x12
#define RIGHT_SHIFT_PRESSED 0x59
#define CAPS_LOCK_PRESSED 0x58
#define CAPS_LOCK_RELEASED 0xF0
#define TAB_SIZE 4

extern void keyboard_link(); 

/* Keyboard variables */
volatile int key_pressed;
volatile int shift_pressed;
volatile int caps_enabled;
volatile int ctrl_pressed;
volatile int enter_pressed;
volatile int alt_pressed;


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
    enter_pressed = 0;
    alt_pressed = 0;
    terminals[cur_terminal].buf_ptr = 0;

    /* Enable keyboard interrupt line */
    enable_irq(KEYBOARD_IRQ);

}

/* keyboard_handler
 *   Inputs: none
 *   Return Value: none
 *    Function: Get scankey from keyboard and print to screen */
extern void keyboard_handler() {
    char out;
    int i;
    uint32_t ignore;

    /* Remap the video memory to print to the visible terminal */
    remap_vidmem_visible();
    
    /* Get keyboard input */
    uint8_t scan_key = inb(KEYBOARD_DATA_PORT);

    /* Check if modifier is pressed. If so update modifier flag and return */
    if (check_modifiers(scan_key))
        { remap_vidmem_service(); send_eoi(KEYBOARD_IRQ);  return; }
 
    /* Check if invalid scan key (scancodes greater than 0x57 are not processed) */
    if (scan_key > 0x57) // invalid scan_key
        { remap_vidmem_service(); send_eoi(KEYBOARD_IRQ); return; }
            
    /* Check for tab (0x0F is tab scan code)*/
    if (scan_key == 0x0F) {
        for (i=0; i<TAB_SIZE; i++)
            { putc(' '); buf_push(' ');  }
        remap_vidmem_service(); send_eoi(KEYBOARD_IRQ); return;
    } 
    
    /* Backspace (0x0E is backspace scan code)*/
    if (scan_key == 0x0E) {
        if (terminals[cur_terminal].buf_ptr==0)
            { remap_vidmem_service(); send_eoi(KEYBOARD_IRQ); return; }
        
        buf_pop();
        putc('\b');
        remap_vidmem_service(); send_eoi(KEYBOARD_IRQ); return;
    }
    
    /* CTRL functions (0x26/0x2E are l/c scan codes) */
    if (ctrl_pressed) {
        if (scan_key == 0x26) // CTRL + L
            { clear(); remap_vidmem_service(); send_eoi(KEYBOARD_IRQ); return; } 
        else if (scan_key == 0x2E) // CTRL + C
            { remap_vidmem_service(); halt(ignore); send_eoi(KEYBOARD_IRQ); return; }
        else // do nothing
            { remap_vidmem_service(); send_eoi(KEYBOARD_IRQ); return;}
    }

    /* ALT Functions (print nothing) */
    if (alt_pressed) {
        if (scan_key == 0x3B)
            terminal_switch(0);
        else if (scan_key == 0x3c)
            terminal_switch(1);
        else if (scan_key == 0x3d)
            terminal_switch(2);
        
        remap_vidmem_service(); send_eoi(KEYBOARD_IRQ); return;
    }    

    /* Set key to be printed */    
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
    
    /* Push character to line buffer and print to screen */
    buf_push(out); putc(out);

    /* Remap the video memory to print to the currently servicing terminal*/
    remap_vidmem_service();

    /* End of Interrupt */
    send_eoi(KEYBOARD_IRQ);
}

/* check_modifiers
 *   Inputs: scan key
 *   Return Value: 1 if modifier key pressed, 0 otherwise. Enter does not count as modifier but has flag which is updated
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
        case 0x38: // alt pressed
            alt_pressed = 1; return 1;
        case 0xB8: // alt released
            alt_pressed = 0; return 1;
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
            (scan_key>=0x10 && scan_key<=0x19) ||       // scan keys - q->p
            (scan_key>=0x1E && scan_key <= 0x26) ||     // scan keys - a->l
            (scan_key>=0x2C && scan_key<=0x32)          // scan keys - z->m
    ) {    
        return 1;
    } else {
        return 0;
    }
}

/* clear_line_buffer
 *   Inputs: none
 *   Return Value: none
 *   Function: clears teh line buffer, sets all characters to null  */
extern void clear_line_buffer() {
    int i;
    /* Set all elements of line buffer to null*/
    for (i=0; i < MAX_BUFFER_SIZE; i++) {
        terminals[cur_terminal].keyboard_buffer[i] = '\0';
    }
}

/* read_line_buffer
 *   Inputs: terminal buffer, num_bytes (number of bytes to read)
 *   Return Value: return number of bytes read
 *   Function: when enter is pressed, copy line buffer into terminal buffer (the specified number of bytes)  */
extern int read_line_buffer(char terminal_buffer[], int num_bytes) {
    int i, num_bytes_read = 0;

    /* Wait for enter keypress */
    enter_pressed=0;
    while (enter_pressed==0);
    
    /* Copy keyboard buffer into passed pointer. Protect read into line buffer */
    for (i=0; i < num_bytes; i++) {
        /* Check if reached newline (end of string). If so then clear line buffer and return */
        if (terminals[cur_terminal].keyboard_buffer[i] == '\n') 
            { clear_line_buffer(); terminals[cur_terminal].buf_ptr=0; return num_bytes_read; }
        
        /* Copy from line buffer to terminal buffer */
        terminal_buffer[i] = terminals[cur_terminal].keyboard_buffer[i];
        
        /* Increment number of bytes read */
        num_bytes_read++;
    }

    /* Clear the line buffer */
    clear_line_buffer();
    terminals[cur_terminal].buf_ptr=0;
    
    /* Return number of bytes read */
    return num_bytes_read;
}

/* buf_push
 *   Inputs: val (character to push onto buffer)
 *   Return Value: none
 *   Function: pushes a single character onto line buffer and appends newline to end (if there is enough space) */
void buf_push(char val) {
    int cur_buf_ptr = terminals[cur_terminal].buf_ptr;

    if (cur_buf_ptr+1 < MAX_BUFFER_SIZE) {
        terminals[cur_terminal].keyboard_buffer[cur_buf_ptr] = val;
        terminals[cur_terminal].buf_ptr++;
        terminals[cur_terminal].keyboard_buffer[ terminals[cur_terminal].buf_ptr ] = '\n';
    }
}

/* buf_pop
 *   Inputs: none
 *   Return Value: none
 *   Function: removes single character from line buffer (if able to), keeps the newline at the end of buffer */
void buf_pop() {
    int cur_buf_ptr = terminals[cur_terminal].buf_ptr;
    if (cur_buf_ptr-1 > -1) {
        terminals[cur_terminal].keyboard_buffer[cur_buf_ptr]='\n'; 
        terminals[cur_terminal].buf_ptr--;
    }
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
*         the rows where the cursor starts and ends. 
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

