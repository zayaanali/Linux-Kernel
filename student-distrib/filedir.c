#include "filedir.h"
#include "filesystem.h"
#include "lib.h"

#define DIR_SIZE 64
#define FNAME_SIZE 32
uint32_t file_pos; 

/* dir_open
 *   Inputs: fname  : name of directory file to open
 *   Return Value: 0 upon success
 *   Function: Initialize any structs/variables needed for directory handling 
 */
int dir_open(const uint8_t* fname){

    if(strncmp((int8_t*)fname, ".", 32)!=0){
        return -1; 
    }
    // initialize file_pos to 0 to start first read from beginning of directory file
    file_pos = 0; 

    return 0; 
}


/* dir_close
 *   Inputs: fd  : file descriptor of directory to close
 *   Return Value: 0 upon success
 *   Function: Undo anything done with dir_open if necessary
 */
int dir_close(int32_t fd){

    return 0; 
}


/* dir_write
 *   Inputs: none
 *   Return Value: -1 for failure, this always fails
 *   Function: signals failure to write to read-only directory
 */
int dir_write(int32_t fd, const void* buf, int32_t nbytes){

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
int dir_read(const uint8_t* fname, uint8_t* buf, uint32_t nbytes){

    dentry_t dentry[1]; 
    uint32_t bytes_read = 0; 

    // Initialize variables for reading data
    uint32_t dir_fname_offset = file_pos % FNAME_SIZE;              // offset into file name of current directory entry
    uint32_t dir_index = file_pos / FNAME_SIZE;

    while((bytes_read < nbytes) && (dir_index < boot_block->num_dir_entries)){

        // Calculate how much data to read from this file name
        uint32_t to_read = nbytes-bytes_read;
        if (to_read > FNAME_SIZE - dir_fname_offset) {
            to_read = FNAME_SIZE - dir_fname_offset;
        }

        // Read data from file name
        read_dentry_by_index(dir_index, dentry);
        uint8_t* fname_ptr = dentry->file_name;
        memcpy(buf + bytes_read, fname_ptr + dir_fname_offset, to_read);

        // Update variables
        bytes_read += to_read;
        dir_index++;
        dir_fname_offset = 0; // Reset offset for next blocks
    }

    // update file position
    file_pos +=bytes_read; 

    return bytes_read; 
}
