/*
    Store entries for the 20 exceptions needed in IDT. 
*/

#include "x86_desc.h"
#include "excepts.h"
#include "lib.h"

extern void divide_error_link(); 

void setup_exceptions(){
    
    int i;                                // loop variable 


    // trap gates for all 20 exceptions have all the same bits except for the offsets
    // set reserved0|D|reserved1|reserved2|reserved3|reserved4[8] to 0 1111 0000 0000 for 32-bit trap gate
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

    /* set all offsets to correct exception handler */
    SET_IDT_ENTRY(idt[0], divide_error);
    /*SET_IDT_ENTRY(idt[1], debug);
    SET_IDT_ENTRY(idt[2], nmi_interrupt);
    SET_IDT_ENTRY(idt[3], breakpoint);
    SET_IDT_ENTRY(idt[4], overflow);
    SET_IDT_ENTRY(idt[5], bound_rng_ex);
    SET_IDT_ENTRY(idt[6], invalid_op);
    SET_IDT_ENTRY(idt[7], device_not_avail);
    SET_IDT_ENTRY(idt[8], dbl_fault);
    SET_IDT_ENTRY(idt[9], co_seg_overrun);
    SET_IDT_ENTRY(idt[10], invalid_tss);
    SET_IDT_ENTRY(idt[11], seg_not_present);
    SET_IDT_ENTRY(idt[12], stack_seg_fault);
    SET_IDT_ENTRY(idt[13], gen_prot);
    SET_IDT_ENTRY(idt[14], page_fault);
   
    SET_IDT_ENTRY(idt[16], fp_error);
    SET_IDT_ENTRY(idt[17], align_check);
    SET_IDT_ENTRY(idt[18], mach_check);
    SET_IDT_ENTRY(idt[19], simd_fp);*/

}


void divide_error(){

    clear(); 
    printf("ERR Trying to divide by 0 \n");
    while(1){
        ;
    }
}

/*
extern void debug(){

    clear(); 
    printf("ERR DB exception \n");
    while(1){
        ;
    }

}

extern void nmi_interrupt(){

    clear(); 
    printf("ERR Non-Maskable Interrupt \n");
    while(1){
        ;
    }
}

extern void breakpoint(){

    clear(); 
    printf("ERR breakpoint \n");
    while(1){
        ;
    }
}

extern void overflow(){

    clear(); 
    printf("ERR overflow \n");
    while(1){
        ;
    }
}

extern void bound_rng_ex(){

    clear(); 
    printf("ERR bound range exceeded \n");
    while(1){
        ;
    }
}

extern void invalid_op(){

    clear(); 
    printf("ERR invalid opcode \n");
    while(1){
        ;
    }

}

extern void device_not_avail(){

    clear(); 
    printf("ERR device not available \n");
    while(1){
        ;
    }
}

extern void dbl_fault(){

    clear(); 
    printf("ERR Double Fault \n");
    while(1){
        ;
    }
}

extern void co_seg_overrun(){
    
    clear(); 
    printf("ERR Coprocessor Segment Overrun \n");
    while(1){
        ;
    }
}
extern void invalid_tss(){

    clear(); 
    printf("ERR Invalid TSS \n");
    while(1){
        ;
    }
}

extern void seg_not_present(){
       
    clear(); 
    printf("ERR Segment Not Present \n");
    while(1){
        ;
    }
}

extern void stack_seg_fault(){

    clear(); 
    printf("ERR Stack-Segment Fault \n");
    while(1){
        ;
    }

}


extern void gen_prot(){

    clear(); 
    printf("ERR General Protection Fault \n");
    while(1){
        ;
    }
}


extern void page_fault(){

    clear(); 
    printf("ERR Page Fault \n");
    while(1){
        ;
    }
}


extern void fp_error(){

    clear(); 
    printf("ERR Floating-Point Error \n");
    while(1){
        ;
    }
}


extern void align_check(){

    clear(); 
    printf("ERR Alignment Check Fault \n");
    while(1){
        ;
    }
}

extern void mach_check(){

    clear(); 
    printf("ERR Machine Check \n");
    while(1){
        ;
    }
}


extern void simd_fp(){

    clear(); 
    printf("ERR SIMD Floating-Point Exception \n");
    while(1){
        ;
    }
}*/