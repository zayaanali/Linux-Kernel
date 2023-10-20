#include "file.h"
#include "filesystem.h"

uint32_t file_pos[64];      // store current file position by inode#

/* file_open
 *   Inputs: fname  : name of file to open
 *   Return Value: 0 upon success
 *   Function: Initialize any structs/variables needed for file handling 
 */
int file_open(const uint8_t* fname){
    // initialize file position to start reading from to beginning of file
    dentry_t dentry[1];
    read_dentry_by_name(fname, dentry);

    int inode_id = dentry->inode_id;
    
    file_pos[inode_id] = 0; 

    return 0; 
}


/* file_close
 *   Inputs: fname  : name of file to close
 *   Return Value: 0 upon success
 *   Function: Undo anything done with file_open if necessary
 */
int file_close(int32_t fd){

    return 0; 
}

/* file_write
 *   Inputs: none
 *   Return Value: -1 for failure, this always fails
 *   Function: signals failure to write to read-only files
 */
int file_write(){

    //cannot write to read-only files
    return -1; 
}


/* file_read
 *   Inputs: fname    : name of file to read data from
 *           buf      : buffer to store data read
 *           nbytes   : number of bytes to read from the file
 *   Return Value: number of bytes read
 *   Function: reads nbytes bytes from file fname into buffer buf
 */
int file_read(const uint8_t* fname, uint8_t* buf, uint32_t nbytes){

    dentry_t dentry[1]; 
    uint32_t bytes_read; 

    if(0!=read_dentry_by_name(fname, dentry)){                                              // get directory entry for fname into dentry 
        return -1;
    }        
    bytes_read = read_data(dentry->inode_id, file_pos[dentry->inode_id], buf, nbytes);       // read data into buf, return number of bytes read

    // update file position
    file_pos[dentry->inode_id] +=bytes_read; 

    return bytes_read; 
}