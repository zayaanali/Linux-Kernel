#include "terminal.h"
#include "pcb.h"
#include "systemcall.h"
#include "scheduler.h"
#include "paging.h"
#include "page.h"

int32_t TERMINAL_VIDMEM_PTR[] = { TERM1_VIDMEM, TERM2_VIDMEM, TERM3_VIDMEM};
static char* video_mem = (char *)VIDEO;
int cur_terminal = 0; 

int32_t terminal_open(const uint8_t* filename) {

    // create stdin entry in file array
    if(pcb_ptr[active_pid]->fd_array[0].in_use==1){
        printf("stdin already exists \n");
        return -1;
    }

    pcb_ptr[active_pid]->fd_array[0].file_op_tbl_ptr = &term_funcs; 
    pcb_ptr[active_pid]->fd_array[0].in_use=1; 

    // create stdout entry in file array
    if(pcb_ptr[active_pid]->fd_array[1].in_use==1){
        printf("stdout already exists");
        return -1; 
    }

    pcb_ptr[active_pid]->fd_array[1].file_op_tbl_ptr =&term_funcs;
    pcb_ptr[active_pid]->fd_array[1].in_use=1; 

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
    //terminal_write(0, char_buf, num_bytes_read+1);

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


/* Switches execution from one terminal to the other */
int32_t terminal_switch(int new_term_idx) {
    
    uint32_t flags;
    cli_and_save(flags);

    //map virt vid mem to physical vid mem being viewed
    remap_vidmem_visible();


    /* Copy video mem from current terminal to memory (saving current video mem) */
    memcpy((void*)TERMINAL_VIDMEM_PTR[cur_terminal], (void*)video_mem, FOUR_KB);
        
    /* Save the cursor location */
    terminals[cur_terminal].cursor_x = get_screen_x();
    terminals[cur_terminal].cursor_y = get_screen_y();

    /* Copy new video terminal mem */
    memcpy((void*)video_mem, (void*)TERMINAL_VIDMEM_PTR[new_term_idx], FOUR_KB);
    update_screen_coords(terminals[new_term_idx].cursor_x, terminals[new_term_idx].cursor_y);
    set_cursor(terminals[new_term_idx].cursor_x, terminals[new_term_idx].cursor_y);

    /* Set new current terminal index */
    cur_terminal = new_term_idx;

    // restore virt vid mem to point to correct video page depending on what's viewed
    remap_vidmem_service();

    restore_flags(flags);

    return 0;
}

/* Remap to the currently serviced terminal */
void remap_vidmem_service() {
    // printf("remapping to terminal %d\n", active_tid);
    if(active_tid == cur_terminal){
        page_table[184].page_base_address = 184;
    }else{
        page_table[184].page_base_address = 184 + 1 + active_tid;
    }
    flush_tlb();
    
}

void remap_vidmem_visible() {
    page_table[184].page_base_address = 184;
    flush_tlb();
}

int remap_vidmem_vis_test() {
    page_table[184].page_base_address = 184;
    return active_tid;
}

void remap_vidmem_service_test(int tid) {
        if(tid == cur_terminal){
        page_table[184].page_base_address = 184;
    }else{
        page_table[184].page_base_address = 184 + 1 + tid;
    }
}

