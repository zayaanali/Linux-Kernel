
#define MAKE_LINKAGE(linker, handler) \
.GLOBL linker   ;\
linker:     pushl %ebp  ;\
    movl %esp, %ebp ;\
    pushl %eax  ;\
    pushl %ebx  ;\
    pushl %ecx  ;\
    pushl %edx  ;\
    pushl %esi  ;\
    pushl %edi  ;\
    pushf   ;\
    call handler    ;\
    popf    ;\
    popl %edi   ;\
    popl %esi   ;\
    popl %edx   ;\
    popl %ecx   ;\
    popl %ebx   ;\
    popl %eax   ;\
    leave   ;\
    iret    





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


