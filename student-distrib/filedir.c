#include "filedir.h"
#include "filesystem.h"
#include "lib.h"
#include "pcb.h"
#include "systemcall.h"
#include "scheduler.h"

#define DIR_SIZE 64
#define FNAME_SIZE 32
//uint32_t file_pos; 

/* dir_open
 *   Inputs: fname  : name of directory file to open
 *   Return Value: 0 upon success
 *   Function: Initialize any structs/variables needed for directory handling 
 */
int32_t dir_open(const uint8_t* fname){

    if(strncmp((int8_t*)fname, ".", 32)!=0){
        return -1; 
    }
    // initialize file_pos to 0 to start first read from beginning of directory file
    //file_pos = 0; 

    return insert_into_file_array(&dir_funcs, -1);   // inode doesn't matter for directory, send invalid value
}


/* dir_close
 *   Inputs: fd  : file descriptor of directory to close
 *   Return Value: 0 upon success
 *   Function: Undo anything done with dir_open if necessary
 */
int32_t dir_close(int32_t fd){

    return remove_from_file_array(fd); 
}


/* dir_write
 *   Inputs: none
 *   Return Value: -1 for failure, this always fails
 *   Function: signals failure to write to read-only directory
 */
int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes){

    //cannot write to read-only directory
    return -1; 
}


/* dir_read
 *   Inputs: fname    : name of directory file to read data from
 *           buf      : buffer to store data read
 *           nbytes   : number of bytes to read 
 *   Return Value: number of bytes successfully read, -1 for failure
 *   Function: reads nbytes bytes from directory file "fname" into buffer buf
 */
int32_t dir_read(int32_t fd, void* buf, int32_t nbytes){

    dentry_t dentry[1]; 
    uint32_t bytes_read = 0; 

    cli();

    // Initialize variables for reading data
    uint32_t dir_fname_offset = pcb_ptr[active_pid]->fd_array[fd].file_pos % FNAME_SIZE;              // offset into file name of current directory entry
    uint32_t dir_index = pcb_ptr[active_pid]->fd_array[fd].file_pos / FNAME_SIZE;

    while((bytes_read < nbytes) && (dir_index < boot_block->num_dir_entries)){

        // Calculate how much data to read from this file name
        uint32_t to_read = nbytes-bytes_read;
        if (to_read > FNAME_SIZE - dir_fname_offset) {
            to_read = FNAME_SIZE - dir_fname_offset;
        }

        // Read data from file name
        read_dentry_by_index(dir_index, dentry);
        uint8_t* fname_ptr = dentry->file_name;
        memcpy(((uint8_t*)buf) + bytes_read, fname_ptr + dir_fname_offset, to_read);

        // Update variables
        bytes_read += to_read;
        dir_index++;
        dir_fname_offset = 0; // Reset offset for next blocks
    }

    // update file position
    pcb_ptr[active_pid]->fd_array[fd].file_pos +=bytes_read; 

    sti();

    ((uint8_t*)buf)[bytes_read] = '\0';

    return (strlen((const int8_t*)buf)); 
}
