#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "excepts_s.h"

#define PASS 1
#define FAIL 0
#define LLONG_MAX 9223372036854775807

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int i;
	int result = PASS;
	for (i = 0; i < 20; ++i){
		if(i==15){
			continue; 
		}
		
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}

// add more tests here

void exceptions_test(){

	// divide_error_link(); 
	// debug_link();
 	// nmi_interrupt_link();
 	// breakpoint_link();
 	// overflow_link();
  	// bound_rng_ex_link();
  	// invalid_op_link();
  	//device_not_avail_link();
 	// dbl_fault_link();
 	// co_seg_overrun_link();
 	// invalid_tss_link();
 	// seg_not_present_link();
 	// stack_seg_fault_link();
 	// gen_prot_link();
 	//page_fault_link();
 	// fp_error_link();
 	// align_check_link();
 	// mach_check_link();
 	// simd_fp_link();


}

/* Checkpoint 2 tests */
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	//TEST_OUTPUT("idt_test", idt_test());
	// launch your tests here
	
	//exceptions_test();

}
