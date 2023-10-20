#include "filedir.h"
#include "filesystem.h"

uint32_t file_pos; 

/* dir_open
 *   Inputs: fname  : name of directory file to open
 *   Return Value: 0 upon success
 *   Function: Initialize any structs/variables needed for directory handling 
 */
int dir_open(const uint8_t* fname){

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
    uint32_t bytes_read; 

    if(0!=read_dentry_by_name(fname, dentry)){                             // get directory entry for fname into dentry 
        return -1;
    }                                 
    bytes_read = read_data(dentry->inode_id, file_pos, buf, nbytes);       // read data into buf, return number of bytes read

    // update file position
    file_pos +=bytes_read; 

    return bytes_read; 
}