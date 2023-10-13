/*
    Store entries for the 20 exceptions needed in IDT. 
*/

#include "x86_desc.h"
#include "excepts.h"
#include "lib.h"

void setup_exceptions(){
    
    int i;                                        // loop variable 


    // trap gates for all 20 exceptions store the same lower bits for long 1 and the same segment selector
    // skip reserved and NMI interrupt exceptions
    for(i=0; i<20; i++){

        if(i == 15){
            continue;   // intel reserved, skip
        } 

        
        idt[i].present = 1;             // segment is present
        idt[i].dpl = 0;                 // DPL = 00 (highest priority)
        idt[i].reserved0 = 0;           
        idt[i].size = 1;                // size (D) = 1 (32 bit gate)
        idt[i].reserved1 = 1;
        idt[i].reserved2 = 1;
        idt[i].reserved3 = 1;
        idt[i].reserved4 = 0;
        idt[i].seg_selector = KERNEL_CS;

    }
    

    /* set Trap Gate offset fields to point to exception handler for each exception */

    /* set all offset to divide by 0 handler */
    for(i=0; i<20; i++){
        if(i==15){
            continue; 
        }

        SET_IDT_ENTRY(idt[i], divide_error);
    }


}


void divide_error(){

    clear(); 
    printf("ERR Trying to divide by 0 \n");
    while(1){
        ;
    }
}

