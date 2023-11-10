#include "terminal.h"
#include "pcb.h"
#include "systemcall.h"

int32_t terminal_open(const uint8_t* filename) {

    // create stdin entry in file array
    if(pcb_ptr[cur_pid]->fd_array[0].in_use==1){
        printf("stdin already exists \n");
        return -1;
    }

    pcb_ptr[cur_pid]->fd_array[0].file_op_tbl_ptr = &term_funcs; 
    pcb_ptr[cur_pid]->fd_array[0].in_use=1; 

    // create stdout entry in file array
    if(pcb_ptr[cur_pid]->fd_array[1].in_use==1){
        printf("stdout already exists");
        return -1; 
    }

    pcb_ptr[cur_pid]->fd_array[1].file_op_tbl_ptr =&term_funcs;
    pcb_ptr[cur_pid]->fd_array[1].in_use=1; 

    return 0;
}

int32_t terminal_close(int32_t fd) {
    return remove_from_file_array(fd); 
}

/* terminal_read
 *   Inputs: fd, buf (buffer to read into), nbytes (number of bytes to read)
 *   Return Value: Number of bytes written
 *   Function: Reads specified number of bytes from line buffer when return is pressed, and then prints to screen */
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes) {
    int num_bytes_read;
    
    /* Cast the void* pointer to char* (pointer to head of array) */
    char *char_buf = (char*)buf;
        
    /* Copy the keyboard buffer into the terminal buffer and get number of bytes read */
    num_bytes_read = read_line_buffer(char_buf, nbytes);

    /* Set last character as newline */
    char_buf[num_bytes_read] = '\n';
    
    /* Clear the keyboard buffer */
    clear_line_buffer();

    /* Write contents of the buffer to the screen */
    terminal_write(0, char_buf, num_bytes_read+1);

    /* return number of bytes read */
    return num_bytes_read;
}

/* terminal_write
 *   Inputs: fd, buf (buffer to write to screen), nbytes (number of bytes to write)
 *   Return Value: Number of bytes written
 *   Function: Write specified number of bytes from buffer to screen */
int32_t terminal_write(int fd, const void* buf, int32_t nbytes) {
    int i;
    int bytes_written=0;
    char *char_buf = (char*)buf;
    
    /* Print each character in the buffer */
    for (i=0; i < nbytes; i++) {
        if (char_buf[i]=='\0')
            continue;
        putc(char_buf[i]); 
        bytes_written++;
    }
    
    /* Return number of bytes written */
    return bytes_written;
}

