#ifndef _EXCEPTS_H
#define _EXCEPTS_H
/* handlers for each exception */

extern void setup_exceptions();

extern void divide_error();
extern void debug();
extern void nmi_interrupt();
extern void breakpoint();
extern void overflow();
extern void bound_rng_ex();
extern void invalid_op();
extern void device_not_avail();
extern void dbl_fault();
extern void co_seg_overrun();
extern void invalid_tss();
extern void seg_not_present();
extern void stack_seg_fault();
extern void gen_prot();
extern void page_fault();
extern void fp_error();
extern void align_check();
extern void mach_check();
extern void simd_fp();



#endif

