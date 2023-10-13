/*
    Store entries for the 20 exceptions needed in IDT. 
*/

#include "x86_desc.h"
#include "excepts.h"
#include "lib.h"

void setup_exceptions(){
    
    int i;                                        // loop variable 
    uint16_t lower_bits;                          // 16 lower bits for long 1 of trap gate
    uint32_t seg_off = (KERNEL_CS<<16);           // 32 bits of segment selector and offset for long 0 of trap gate


    // use trap gate for exceptions
    // format of lower bits: P|DPL|0 D 1 1 1 | 0 0 0 | reserved
    // want P = 1 (segment is present), DPL = 00 (highest priority), D = 1 (32 bit gate)
    lower_bits = 0x8f00; 

    // trap gates for all 20 exceptions store the same lower bits for long 1 and the same segment selector
    // skip reserved and NMI interrupt exceptions
    for(i=0; i<20; i++){
        if(i==2||i==9||15){
            continue; 
        }
        idt[i].val[1] = lower_bits;
        idt[i].val[0] = seg_off; 
    }
    
    
    /* set Trap Gate offset fields to point to exception handler for each exception */

    /* divide by 0 */
    SET_IDT_ENTRY(idt[0], divide_error);


}


void divide_error(){

   printf("ERR Trying to divide by 0");
}

