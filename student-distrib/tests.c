#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "excepts_s.h"
#include "i8259.h"
#include "rtc.h"
#include "system_s.h"
#include "types.h"
#include "file.h"
#include "filedir.h"

#define PASS 1
#define FAIL 0

#define VIDEO        0xB8000			// starting physical/virtual address of Video memory
#define KERNEL_START 0x400000			// starting physical/virtual address of kernel memory

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

static void read_file(const uint8_t* fname);

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

/* exceptions_test
 *   Inputs: none
 *	 Outputs: an exception message should print corresponding to the exception raised by any uncommented line in the function. 
 *   Return Value: none
 *   Coverage: IDT, exception handlers
 *   Function: Tests exceptions by raising an exception */
void exceptions_test(){
	/* to test, pick ONE section to uncomment */

	// divide_error_link();  

	// // OR 

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

 	//invalid_tss_link();

 	// seg_not_present_link();

 	// stack_seg_fault_link();

 	// gen_prot_link();



 	// page_fault_link();

	// // OR

	// char* p = 0x00;
	// char i = *p;


 	// fp_error_link();

 	// align_check_link();

 	// mach_check_link();

 	// simd_fp_link();


}

/* rtc_test
 *   Inputs: none
 *	 Outputs: the screen should flash between two different sets of characters to indicate the RTC is set up and running
 *   Return Value: none
 * 	 Coverage: RTC
 *   Function: Tests the RTC by enabling its IRQ line */
void rtc_test(){

	rtc_open("Hi");

	enable_irq(RTC_IRQ);

	uint16_t freq_buf[1];

	freq_buf[0] = 1024;
	rtc_write(0, freq_buf, 0);
	int i;
	clear();
	for(i = 0; i < 2048; i++){
		if(rtc_read(0,0,0)){
			printf(" 1024HZ\n");
		}
	}
	clear();	
	freq_buf[0] = 512;
	rtc_write(0, freq_buf, 0);
	for(i = 0; i < 1024; i++){
		if(rtc_read(0,0,0)){
			printf(" 512HZ\n");
		}
	}
	clear();
	freq_buf[0] = 256;
	rtc_write(0, freq_buf, 0);
	for(i = 0; i < 512; i++){
		if(rtc_read(0,0,0)){
			printf(" 256HZ\n");
		}
	}
	clear();
	freq_buf[0] = 128;
	rtc_write(0, freq_buf, 0);
	for(i = 0; i < 256; i++){
		if(rtc_read(0,0,0)){
			printf(" 128HZ\n");
		}
	}
	clear();
	freq_buf[0] = 64;
	rtc_write(0, freq_buf, 0);
	for(i = 0; i < 128; i++){
		if(rtc_read(0,0,0)){
			printf(" 64HZ\n");
		}
	}
	clear();
	freq_buf[0] = 32;
	rtc_write(0, freq_buf, 0);
	for(i = 0; i < 64; i++){
		if(rtc_read(0,0,0)){
			printf(" 32HZ\n");
		}
	}
	clear();
	freq_buf[0] = 16;
	rtc_write(0, freq_buf, 0);
	for(i = 0; i < 32; i++){
		if(rtc_read(0,0,0)){
			printf(" 16HZ\n");
		}
	}
	clear();
	freq_buf[0] = 8;
	rtc_write(0, freq_buf, 0);
	for(i = 0; i < 16; i++){
		if(rtc_read(0,0,0)){
			printf(" 8HZ\n");
		}
	}
	clear();
	freq_buf[0] = 4;
	rtc_write(0, freq_buf, 0);
	for(i = 0; i < 8; i++){
		if(rtc_read(0,0,0)){
			printf("4HZ\n");
		}
	}

	clear();
	freq_buf[0] = 2;
	rtc_write(0, freq_buf, 0);
	for(i = 0; i < 4; i++){
		if(rtc_read(0,0,0)){
			printf("2HZ\n");
		}
	}

}


/* syscall_idt_test
 *   Inputs: none
 *	 Outputs: a message should print to the screen inidcating a system call has occurred
 *   Return Value: none
 * 	 Coverage: IDT, systemcall handler
 *   Function: Tests correctness of the systemcall entry in the IDT by raising a systemcall */
void syscall_idt_test(){
	asm volatile("int $128");	// 128==0x80, vector for systemcall

}


/* test_paging_inaccess
 *   Inputs: none
 *	 Outputs: a message should print to the screen indicating a page fault has occurred
 *   Return Value: none
 * 	 Coverage: paging
 *   Function: Tests that inaccessible memory is truly inaccessable. Uncommenting any section should cause a page fault. */
void test_paging_inaccess(){
	/* running any of these sections should cause a page fault*/
	
	// //check null ptr; 
	// char* p = (char*)0x00;
	// char i = *p;
	// printf("passed dereferencing null \n");

	// //read a byte right before video mem location
	// char* p = (char*)(VIDEO-1);
	// char i = *p; 
	// printf("dereferencing video mem byte -1, read: %c \n", i);

	// read a byte right after video mem location
	char* p = (char*)(VIDEO+4096);
	char i = *p; 
	printf("dereferencing mem physcial address 0xb9000, read: %c \n", i);

	// //read a byte right before kernel mem location
	// char* p = (char*)(KERNEL_START-1);
	// char i = *p; 
	// printf("dereferencing kernel mem byte -1, read: %c \n", i);

	// // read a byte right after video mem location
	// char* p = (char*)(KERNEL_START+4194304);
	// char i = *p; 
	// printf("dereferencing kernel physcial address 0x500000, read: %c \n", i);

}


/* test_paging_access
 *   Inputs: none
 *	 Outputs: Prints test progress to the screen. Test should finish without any page faults occurring.
 *   Return Value: none
 * 	 Coverage: paging
 *   Function: Tests that all of video and kernel memory can be accessed. */
void test_paging_access(){

	int j; 

	char* p = (char*)VIDEO;
	char i = *p; 

	clear(); 
	printf(" \n");
	printf(" \n");
	printf(" Testing reading from every byte of video memory... \n");
	// test reading from every video mem location
	for(j=VIDEO; j<(VIDEO+4096); j++){
	 	p = (char*)(j);
	 	i = *p; 
	 	//printf("dereferencing video mem byte %x, read: %c \n", j, i);
	}

	printf(" Testing reading from every byte of kernel memory... \n");
	// test reading from every kernel code location
	for(j=KERNEL_START; j<(KERNEL_START+4194304); j++){
	  	p = (char*)(j);
	  	i = *p; 
	  	//printf("dereferencing kernel mem byte %x, read: %c \n", j, i);
	}

	/*
	// test reading byte 0 of kernel code
	p = (char*)(KERNEL_START);
	i = *p;
	printf("dereferencing kernel address %x, read: %x \n", p, i);

	// test reading last byte of kernel code
	p = (char*)(KERNEL_START+4194303);
	i = *p;
	printf("dereferencing kernel address %x, read: %x \n", p, i);

	// test reading byte 0 of video mem
	p = (char*)(VIDEO);
	i = *p;
	printf("dereferencing video address %x, read: %x \n", p, i);

	// test reading last byte of video mem
	p = (char*)(VIDEO+4095);
	i = *p;
	printf("dereferencing video address %x, read: %x \n", p, i);*/

	printf(" Test finished. \n");

}
/* Checkpoint 2 tests */

static void test_filesys(){

	

	//read_file("frame0.txt");

	// if(-1==file_read("frame0.txt", buf, 80)){
	// 	printf("file read failed \n");
	// }
	// printf(" %s \n", buf);

	// if(-1==file_read("frame0.txt", buf, 80)){
	// 	printf("file read failed \n");
	// }
	// printf(" %s \n", buf);


	// i = file_read("ls", buf, 32);

	// buf[32]='\0';
	// clear(); 
	
	
	// printf("attempting to print buf \n");
	// printf("reading from ls: %s", (buf+1));



}

static void read_file(const uint8_t* fname){

	uint8_t buf[81]; 
	int k; 

	buf[80]='\n';
	clear();

	file_open(fname);

	uint32_t cnt; 
	while (0 != (cnt = file_read(fname, buf, 80))) {
        if (-1 == cnt) {
			printf(" file read failed\n");
	        break; 
	    }

		
		//printf("%s", buf);
		//puts(buf);
		for(k=0; k<cnt; k++){
			if(buf[k]!='\0'){
				putc(buf[k]);
			}
			
		}

		//putc('\n');
		//printf("\n");
    
	}

	printf("file: %s \n", fname);


}

void test_dir_read(){
	uint8_t buf[32]; 
	int i;

	clear(); 

	dir_open(".");


	// do an ls. that is, print all file names in directory 
	uint32_t cnt; 
	while (0 != (cnt = dir_read(".", buf, 32))) {
        if (-1 == cnt) {
			printf(" directory entry read failed\n");
	        break; 
	    }

		printf(" %s \n", buf);
    
	}
}

/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	
	// launch your tests here

	/* checkpoint 2 */
	//test_dir_read(); 

	//rtc_test();

	//test_filesys(); 

	// test reading small files
	//read_file("frame0.txt");
	// read_file("frame1.txt");

	// test reading executables
	//read_file("verylargetextwithverylongname.tx");
	//read_file("frame0.txt");
	read_file("ls");

	/* checkpoint 1 */
	//TEST_OUTPUT("idt_test", idt_test());

	//exceptions_test();

	//rtc_test(); 

	//syscall_idt_test(); 

	//test_paging_access(); 

	//test_paging_inaccess(); 

	

}
