# this code creates the assembly linkage for the exception handlers and keyboard and RTC interrupt handlers. 

#  MAKE_LINKAGE
#    Inputs: none
#    Return Value: none
#    Function: macro that defines assembly wrapper handlers for each exception handler. Stack is setup, then control 
#               is passed to the handler defined in .c 
#define MAKE_LINKAGE(linker, handler) \
.GLOBL linker   ;\
linker:     pushal ;\
            pushfl ;\
            call handler    ;\
            popfl       ;\
            popal  ;\

            iret    



#  use MAKE_LINKAGE macro to define an assembly wrapper handler for every exception handler
MAKE_LINKAGE(debug_link, debug)
MAKE_LINKAGE(divide_error_link, divide_error)
MAKE_LINKAGE(nmi_interrupt_link, nmi_interrupt)
MAKE_LINKAGE(breakpoint_link, breakpoint)
MAKE_LINKAGE(overflow_link, overflow)
MAKE_LINKAGE(bound_rng_ex_link, bound_rng_ex)
MAKE_LINKAGE(invalid_op_link, invalid_op)
MAKE_LINKAGE(device_not_avail_link, device_not_avail)
MAKE_LINKAGE(dbl_fault_link, dbl_fault)
MAKE_LINKAGE(co_seg_overrun_link, co_seg_overrun)
MAKE_LINKAGE(invalid_tss_link, invalid_tss)
MAKE_LINKAGE(seg_not_present_link, seg_not_present)
MAKE_LINKAGE(stack_seg_fault_link, stack_seg_fault)
MAKE_LINKAGE(gen_prot_link, gen_prot)
MAKE_LINKAGE(page_fault_link, page_fault)
MAKE_LINKAGE(fp_error_link, fp_error)
MAKE_LINKAGE(align_check_link, align_check)
MAKE_LINKAGE(mach_check_link, mach_check)
MAKE_LINKAGE(simd_fp_link, simd_fp)


#  rtc_link
#    Inputs: none
#    Return Value: none
#    Function: assembly wrapper for rtc_handler. sets up the stack, calls rtc_handler, then does iret
.GLOBL rtc_link
rtc_link:
            pushal 
            pushfl 
            call rtc_handler    
            popfl       
            popal  
            iret  


#  keyboard_link
#    Inputs: none
#    Return Value: none
#    Function: assembly wrapper for keyboard_handler. sets up the stack, calls keyboard_handler, then does iret
.GLOBL keyboard_link
keyboard_link:
            pushal 
            pushfl 
            call keyboard_handler   
            popfl       
            popal  
            iret


#  pit_int_link
#    Inputs: none
#    Return Value: none
#    Function: assembly wrapper for timer. sets up the stack, calls pit_int_handler , then does iret
.GLOBL pit_int_link
pit_int_link:
            pushal 
            pushfl 
            call pit_int_handler   
            popfl       
            popal  
            iret    
