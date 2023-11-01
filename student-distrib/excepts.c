/*
    This file holds the function to set up the 20 exception IDT entries as well as the unlinked, c handler code for each exception.
*/

#include "x86_desc.h"
#include "excepts.h"
#include "lib.h"
#include "excepts_s.h"
#include "systemcall.h"

extern void divide_error_link(); 
extern void debug_link();
extern void nmi_interrupt_link();
extern void breakpoint_link();
extern void overflow_link();
extern void bound_rng_ex_link();
extern void invalid_op_link();
extern void device_not_avail_link();
extern void dbl_fault_link();
extern void co_seg_overrun_link();
extern void invalid_tss_link();
extern void seg_not_present_link();
extern void stack_seg_fault_link();
extern void gen_prot_link();
extern void page_fault_link();
extern void fp_error_link();
extern void align_check_link();
extern void mach_check_link();
extern void simd_fp_link();
int exception_flag = 0;

/* setup_exceptions
 *   Inputs: none
 *   Return Value: none
 *   Function: Initialize top 20 IDT entries to handle all 20 exceptions except exception 15 which is reserved by Intel */
void setup_exceptions(){
    
    int i;                               // loop variable 


    /* trap gates for all 20 exceptions have all the same bits except for the offsets, so set all concurrently */ 
    for(i=0; i<20; i++){

        if(i == 15){
            continue;                   // intel reserved, skip
        } 

        /* set reserved0|size|reserved1|reserved2|reserved3|reserved4[8] to 0 1111 0000 0000 for 32-bit trap gate */ 
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
    

    /* set Trap Gate offset fields to point to exception handler for each exception. This is the assembly linkage handler defined in */
    /*    excepts_s.S. It sets up the stack and then calls the corresponding c handler defined below */ 
    SET_IDT_ENTRY(idt[0], divide_error_link);
    SET_IDT_ENTRY(idt[1], debug_link);
    SET_IDT_ENTRY(idt[2], nmi_interrupt_link);
    SET_IDT_ENTRY(idt[3], breakpoint_link);
    SET_IDT_ENTRY(idt[4], overflow_link);
    SET_IDT_ENTRY(idt[5], bound_rng_ex_link);
    SET_IDT_ENTRY(idt[6], invalid_op_link);
    SET_IDT_ENTRY(idt[7], device_not_avail_link);
    SET_IDT_ENTRY(idt[8], dbl_fault_link);
    SET_IDT_ENTRY(idt[9], co_seg_overrun_link);
    SET_IDT_ENTRY(idt[10], invalid_tss_link);
    SET_IDT_ENTRY(idt[11], seg_not_present_link);
    SET_IDT_ENTRY(idt[12], stack_seg_fault_link);
    SET_IDT_ENTRY(idt[13], gen_prot_link);
    SET_IDT_ENTRY(idt[14], page_fault_link);
   
    SET_IDT_ENTRY(idt[16], fp_error_link);
    SET_IDT_ENTRY(idt[17], align_check_link);
    SET_IDT_ENTRY(idt[18], mach_check_link);
    SET_IDT_ENTRY(idt[19], simd_fp_link);

}

/* divide_error
 *   Inputs: none
 *   Return Value: none
 *   Function: Exception handler for divide exception.  */
void divide_error(){

    cli(); 
    exception_flag = 1; 
    printf("ERR Trying to divide by 0 \n");
    halt(0);

    sti(); 
}

/* debug
 *   Inputs: none
 *   Return Value: none
 *   Function: Exception handler for debug exception.  */
extern void debug(){

    cli();
    exception_flag = 1; 
    printf("ERR DB exception \n");
    halt(0);

    sti(); 

}

/* nmi_interrupt
 *   Inputs: none
 *   Return Value: none
 *   Function: Exception handler for nmi interrupt.  */
extern void nmi_interrupt(){

    cli(); 
    exception_flag = 1;  
    printf("ERR Non-Maskable Interrupt \n");
    halt(0);

    sti(); 
}

/* breakpoint
 *   Inputs: none
 *   Return Value: none
 *   Function: Exception handler for breakpoint exception.  */
extern void breakpoint(){

    cli(); 
    exception_flag = 1;  
    printf("ERR breakpoint \n");
    halt(0);

    sti(); 

}

/* overflow
 *   Inputs: none
 *   Return Value: none
 *   Function: Exception handler for overflow exception.  */
extern void overflow(){

    cli(); 
    exception_flag = 1;  
    printf("ERR overflow \n");
    halt(0);

    sti(); 
}


/* bound_rng_ex
 *   Inputs: none
 *   Return Value: none
 *   Function: Exception handler for bound range exceeded exception.  */
extern void bound_rng_ex(){

    cli();
    exception_flag = 1;  
    printf("ERR bound range exceeded \n");
    halt(0);

    sti(); 
}


/* invalid_op
 *   Inputs: none
 *   Return Value: none
 *   Function: Exception handler for invalid opcode exception.  */
extern void invalid_op(){

    cli();
    exception_flag = 1;  
    printf("ERR invalid opcode \n");
    halt(0);

    sti(); 

}


/* device_not_avail
 *   Inputs: none
 *   Return Value: none
 *   Function: Exception handler for device not available exception.  */
extern void device_not_avail(){

    cli();
    exception_flag = 1;  
    printf("ERR device not available \n");
    halt(0);

    sti(); 
}


/* dbl_fault
 *   Inputs: none
 *   Return Value: none
 *   Function: Exception handler for double fault exception.  */
extern void dbl_fault(){

    cli(); 
    exception_flag = 1;  
    printf("ERR Double Fault \n");
    halt(0);

    sti(); 
}


/* co_seg_overrun
 *   Inputs: none
 *   Return Value: none
 *   Function: Exception handler for Coprocessor Segment Overrun exception.  */
extern void co_seg_overrun(){
    
    cli(); 
    exception_flag = 1;  
    printf("ERR Coprocessor Segment Overrun \n");
    halt(0);

    sti(); 
}

/* invalid_tss
 *   Inputs: none
 *   Return Value: none
 *   Function: Exception handler for invalid tss exception.  */
extern void invalid_tss(){

    cli(); 
    exception_flag = 1;  
    printf("ERR Invalid TSS \n");
    halt(0);

    sti();

}


/* seg_not_present
 *   Inputs: none
 *   Return Value: none
 *   Function: Exception handler for Segment Not Present exception.  */
extern void seg_not_present(){
       
    cli();
    exception_flag = 1;  
    printf("ERR Segment Not Present \n");
    halt(0);

    sti();
}


/* stack_seg_fault
 *   Inputs: none
 *   Return Value: none
 *   Function: Exception handler for Stack Segment Fault.  */
extern void stack_seg_fault(){

    cli();
    exception_flag = 1;  
    printf("ERR Stack-Segment Fault \n");
    halt(0);

    sti();
}


/* gen_prot
 *   Inputs: none
 *   Return Value: none
 *   Function: Exception handler for General Protection Fault.  */
extern void gen_prot(){

    cli(); 
    exception_flag = 1;  
    printf("ERR General Protection Fault \n");
    halt(0);

    sti();

}


/* page_fault
 *   Inputs: none
 *   Return Value: none
 *   Function: Exception handler for Page Fault.  */
extern void page_fault(){

    cli();
    exception_flag = 1;  
    printf("ERR Page Fault \n");
    halt(0);

    sti(); 

}


/* fp_error
 *   Inputs: none
 *   Return Value: none
 *   Function: Exception handler for Floating-Point error.  */
extern void fp_error(){

    cli(); 
    exception_flag = 1;  
    printf("ERR Floating-Point Error \n");
    halt(0);

    sti(); 

}


/* align_check
 *   Inputs: none
 *   Return Value: none
 *   Function: Exception handler for alignment check fault.  */
extern void align_check(){

    cli();
    exception_flag = 1;  
    printf("ERR Alignment Check Fault \n");
    halt(0);

    sti();
}


/* mach_check
 *   Inputs: none
 *   Return Value: none
 *   Function: Exception handler for machine check fault.  */
extern void mach_check(){

    cli(); 
    exception_flag = 1;  
    printf("ERR Machine Check \n");
    halt(0);
    sti();
}


/* simd_fp
 *   Inputs: none
 *   Return Value: none
 *   Function: Exception handler for SIMD floating-point exception.  */
extern void simd_fp(){

    cli(); 
    exception_flag = 1;  
    printf("ERR SIMD Floating-Point Exception \n");
    halt(0);
    sti(); 
    
}
