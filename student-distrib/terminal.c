#include "terminal.h"
#include "pcb.h"
#include "systemcall.h"
#include "scheduler.h"
#include "paging.h"
#include "page.h"

int32_t TERMINAL_VIDMEM_PTR[] = { TERM1_VIDMEM, TERM2_VIDMEM, TERM3_VIDMEM};
static char* video_mem = (char *)VIDEO;
int cur_terminal = 0; 


/* terminal_open
 *   Inputs: filename (ignore)
 *   Return Value: 0 if sucessfully opened, -1 on failure
 *   Function: "Opens" terminal by adding stdin and stdout to file array */
int32_t terminal_open(const uint8_t* filename) {

   
    // create stdin entry in file array at index 0
    if(pcb_ptr[active_pid]->fd_array[0].in_use==1){
        printf("stdin already exists \n");
        return -1;
    }

    cli();
    pcb_ptr[active_pid]->fd_array[0].file_op_tbl_ptr = &term_funcs; 
    pcb_ptr[active_pid]->fd_array[0].in_use=1; 
    sti();

    // create stdout entry in file array at index 1
    if(pcb_ptr[active_pid]->fd_array[1].in_use==1){
        printf("stdout already exists");
        return -1; 
    }

    cli();
    pcb_ptr[active_pid]->fd_array[1].file_op_tbl_ptr =&term_funcs;
    pcb_ptr[active_pid]->fd_array[1].in_use=1; 
    sti();

    return 0;
}

/* terminal_close
 *   Inputs: fd
 *   Return Value: 0 if sucessfully closed, -1 on failure
 *   Function: Closes terminal by removing it from file array */
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

    if(strncmp(char_buf, "exit\n",5)==0 && term_cur_pid[cur_terminal]<3){
        printf("cannot exit base shell \n");
        return 0; 
    }
  
    /* Clear the keyboard buffer */
    clear_line_buffer();

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

/* remap_vidmem
 *   Inputs: id (0-2) of new terminal
 *   Return Value: none
 *   Function: Switches terminal being viewed. */
int32_t terminal_switch(int new_term_idx) {
    
    uint32_t flags;
    cli_and_save(flags);

    /* Check if new terminal is the same as the terminal that is currently being viewed (if so do nothing)*/
    if (new_term_idx == cur_terminal)
        { restore_flags(flags); return 0; }
    
    /* Set screenX/Y to point to the one for the new structure. Set new cursor */
    update_screen_ptr(&terminals[new_term_idx].cursor_x, &terminals[new_term_idx].cursor_y);
    set_cursor(terminals[new_term_idx].cursor_x, terminals[new_term_idx].cursor_y);

    /* Save old terminal video mem */
    remap_vidmem(cur_terminal);
    memcpy((void*)TERMINAL_VIDMEM_PTR[cur_terminal], (void*)video_mem, FOUR_KB);
    
    /* Set new terminal video mem */
    memcpy((void*)video_mem, (void*)TERMINAL_VIDMEM_PTR[new_term_idx], FOUR_KB);
    remap_vidmem(new_term_idx);

    cur_terminal = new_term_idx;

    restore_flags(flags);
    return 0;
}


/* remap_vidmem
 *   Inputs: id (0-2) of new terminal
 *   Return Value: none
 *   Function: allows for servicing different terminals by remapping video memory.
 *              If new terminal to service is being viewed, virt. vidmem address xb8000 points directly
 *              to physical addresss xb8000, else points to terminal's background page */
void remap_vidmem(int new_term) {
    
    /* Set screenX/Y ptr to point to new terminal cursorx/y */
    update_screen_ptr(&terminals[new_term].cursor_x, &terminals[new_term].cursor_y);
    
    /* is the new terminal the currently viewed terminal */
    if (new_term == cur_terminal) {
        /*if so, remap vidmem to currently viewing video memory at xb8000*/
        page_table[184].page_base_address = 184;

        //similarly remap vidmem used for fish
        video_page_table[0].page_base_address = (VIDEO) >> 12;

        // update cursor
        set_cursor(terminals[new_term].cursor_x, terminals[new_term].cursor_y);
    } else {
        //if not, remap vidmem to the vidmem of the new terminal (0 indexed)

        /*map virt. addr xb8000 to terminal's backup page, stored in order directly under 
         * 4kb page at xb8000, +1 for 0-indexed*/
        page_table[184].page_base_address = 184 + 1 + new_term;

        // similarly map video page used for fish
        video_page_table[0].page_base_address = (TERMINAL_VIDMEM_PTR[new_term]) >> 12;
    }

    
    flush_tlb();
}
