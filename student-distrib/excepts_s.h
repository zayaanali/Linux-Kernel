#ifndef _EXCEPTS_S_H
#define _EXCEPTS_S_H

/*
    Title of the file is slightly misleading. This is the header file for the .S file that creates the assembly linkage for the exception handlers and keyboard and RTC
    interrupt handlers. 

*/

/* assembly-linked functions for exception handlers */
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

/* assembly-linked functions for interrupt handlers */
extern void rtc_link(); 
extern void keyboard_link(); 

#endif
