#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "excepts_s.h"
#include "i8259.h"
#include "rtc.h"
#include "system_s.h"

#define PASS 1
#define FAIL 0

#define VIDEO       0xB8000

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
	// OR 
	//int i=5/0; 

	// debug_link();
 	// nmi_interrupt_link();
 	// breakpoint_link();
 	// overflow_link();
  	// bound_rng_ex_link();
  	// invalid_op_link();
  	// device_not_avail_link();
 	// dbl_fault_link();
 	// co_seg_overrun_link();
 	// invalid_tss_link();
 	// seg_not_present_link();
 	// stack_seg_fault_link();
 	// gen_prot_link();

 	// page_fault_link();
	// OR
	// int* p = 0x00;
	// int i = *p;


 	// fp_error_link();
 	// align_check_link();
 	// mach_check_link();
 	// simd_fp_link();


}


void rtc_test(){

	enable_irq(RTC_IRQ);

}

void syscall_idt_test(){
	//printf("in syscall test");
	asm volatile("int $128");

}

void test_paging_inaccess(){

	// check null ptr; 
	// int* p = 0x00;
	// int i = *p;

	// read a byte before video mem location
	// int* p = (VIDEO-1);
	// int i = *p; 
	// printf("dereferencing video mem byte -1, read: %c \n", i);

}

void test_paging_access(){

	int* p = (int*)VIDEO;

	int i = *p; 
	printf("dereferencing video mem byte 0, read: %c \n", i);

	// p = (VIDEO+4093);
	// i = *p; 
	// printf("dereferencing video mem byte 4094, read: %c \n", i);

	p = (int*)0x400000;
	i = *p;
	printf("dereferencing kernel byte 0, read: %x \n", i);

	p = (int*)(0x400000+4194301);
	i = *p;
	printf("dereferencing kernel byte 4,194,302 read: %x \n", i);

	// printf("dereferencing kernel byte 0, read: %x \n", i);


}
/* Checkpoint 2 tests */
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	
	// launch your tests here
	
	//TEST_OUTPUT("idt_test", idt_test());

	//exceptions_test();

	//rtc_test(); 

	//syscall_idt_test(); 

	//test_paging_inaccess(); 

	test_paging_access(); 

}
