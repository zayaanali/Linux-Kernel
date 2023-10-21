#include "terminal.h"


uint32_t terminal_open(const uint8_t* filename) {
    return 0;
}

uint32_t terminal_close(int32_t fd) {
    return 0;
}

// read from keyboard buffer into buf, return number of bytes read
uint32_t terminal_read(int32_t fd, void* buf, int32_t nbytes) {
    int num_bytes_read;
    
    /* Cast the void* pointer to char* (pointer to head of array) */
    char *char_buf = (char*)buf;
        
    /* Copy the keyboard buffer into the terminal buffer and get number of bytes read */
    num_bytes_read = read_line_buffer(char_buf, nbytes);

    /* Clear the keyboard buffer */
    clear_line_buffer();

    /* Write contents of the buffer to the screen */
    terminal_write(0, char_buf, num_bytes_read);

    /* return number of bytes read */
    return num_bytes_read;
}

uint32_t terminal_write(int fd, const void* buf, int32_t nbytes) {
    int i;
    int bytes_written;
    char *char_buf = (char*)buf;
    
    /* Print each character in the buffer */
    for (i=0; i < nbytes; i++) {
        if (char_buf[i] == '\n')  // check if reached end of buffer
            { putc('\n'); break; } 
        else 
            { putc(char_buf[i]); bytes_written++; }
        
    }
    
    /* Return number of bytes written */
    return bytes_written;
}

