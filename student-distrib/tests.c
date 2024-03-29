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
#include "terminal.h"
#include "systemcall.h"
#include "pcb.h"

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
 *	 Outputs: the screen should print the current frequency the RTC is set to. The frequency of the printing should match the frequency indicated in the text printed.
 *				This test cycles through all the possible frequencies the RTC can be set to. 
 *   Return Value: none
 * 	 Coverage: RTC (rtc_open, rtc_read, rtc_write)
 *   Function: Tests the RTC by enabling its IRQ line and changing the frequency to all possible frequencies*/
void rtc_test(){

	//uint8_t* fname = (uint8_t*)"Hi";
	int32_t fd;
	fd = open((const uint8_t*)"rtc");



	enable_irq(RTC_IRQ);

	uint16_t freq_buf[1];

	freq_buf[0] = 1024;
	write(fd, (void*)freq_buf, 0);
	int i;

	for(i = 0; i < 2048; i++){
		if(read(fd,0,0)){
			printf(" 1024HZ\n");
		}
	}

	freq_buf[0] = 512;
	write(fd, freq_buf, 0);
	for(i = 0; i < 1024; i++){
		if(read(fd,0,0)){
			printf(" 512HZ\n");
		}
	}

	freq_buf[0] = 256;
	write(fd, freq_buf, 0);
	for(i = 0; i < 512; i++){
		if(read(fd,0,0)){
			printf(" 256HZ\n");
		}
	}

	freq_buf[0] = 128;
	write(fd, freq_buf, 0);
	for(i = 0; i < 256; i++){
		if(read(fd,0,0)){
			printf(" 128HZ\n");
		}
	}

	freq_buf[0] = 64;
	write(fd, freq_buf, 0);
	for(i = 0; i < 128; i++){
		if(read(fd,0,0)){
			printf(" 64HZ\n");
		}
	}

	freq_buf[0] = 32;
	write(fd, freq_buf, 0);
	for(i = 0; i < 64; i++){
		if(read(fd,0,0)){
			printf(" 32HZ\n");
		}
	}

	freq_buf[0] = 16;
	write(fd, freq_buf, 0);
	for(i = 0; i < 32; i++){
		if(read(fd,0,0)){
			printf(" 16HZ\n");
		}
	}

	freq_buf[0] = 8;
	write(fd, freq_buf, 0);
	for(i = 0; i < 16; i++){
		if(read(fd,0,0)){
			printf(" 8HZ\n");
		}
	}

	freq_buf[0] = 4;
	write(fd, freq_buf, 0);
	for(i = 0; i < 8; i++){
		if(read(fd,0,0)){
			printf("4HZ\n");
		}
	}


	freq_buf[0] = 2;
	write(fd, freq_buf, 0);
	for(i = 0; i < 4; i++){
		if(read(fd,0,0)){
			printf("2HZ\n");
		}
	}

	close(fd);
}


/* rtc_neg_test
 *   Inputs: none
 *	 Outputs: The screen should print that the chosen frequencies are invalid 
 *   Return Value: none
 * 	 Coverage: RTC (rtc_open, rtc_write)
 *   Function: Tests the ability to reject invalid frequencies*/
void rtc_neg_test(){

	uint8_t* fname = (uint8_t*)"Hi";
	rtc_open(fname);

	enable_irq(RTC_IRQ);

	uint16_t freq_buf[1];

	freq_buf[0] = 0;
	int32_t i = rtc_write(0, freq_buf, 0);
	if(i == -1){
		printf("%d HZ is invalid \n", freq_buf[0]);
	}

	freq_buf[0] = 32769;
	i = rtc_write(0, freq_buf, 0);
	if(i == -1){
		printf("%d HZ is invalid \n", freq_buf[0]);
	}

	freq_buf[0] = 11;
	i = rtc_write(0, freq_buf, 0);
	if(i == -1){
		printf("%d HZ is invalid \n", freq_buf[0]);
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


/* test_filesys_fails
 *   Inputs: none
 *	 Outputs: messages print to the screen indicating what the program is attempting to do and the result. Opening non-existant file and directory should fail. Opening
 *				an existing file and directory should work, but writing to either should fail. 
 *   Return Value: none
 * 	 Coverage: filesystem. file_open, file_write, dir_open, dir_write
 *   Function: Tests that things that should fail in regards to filesystem do fail */
static void test_filesys_fails(){

	uint8_t* buf[80];
	uint8_t* fname; 

	int32_t fd; 

	printf("attempting to open non-existant file: %s \n", "abcde");

	fname = (uint8_t*)"abcde";
	fd = file_open(fname);
	if(fd==-1){
		printf("file open failed \n");
	}else{
		printf("success?!?! \n");
	}
	close(fd);

	printf("attempting to open non-existant directory: %s \n", "z");

	fname = (uint8_t*)"z";
	fd = dir_open(fname);
	if(fd==-1){
		printf("directory open failed \n");
	}else{
		printf("success?!?! \n");
	}
	close(fd);


	printf("attempting to write to file: %s ", "frame0.txt");

	fname = (uint8_t*)"frame0.txt";
	fd = file_open(fname);
	if(fd==-1){
		printf("file open failed ?!?! \n");
	}else{
		printf("frame0.txt opened \n");
	}


	if(-1==file_write(fd, (void*)buf, 80)){
		printf("writing to frame0.txt failed \n");
	}else{
		printf("success?!?!? \n");
	}
	close(fd);

	printf("attempting to write to dir: %s ", ".");

	fname = (uint8_t*)".";
	fd = dir_open(fname);
	if(fd==-1){
		printf("dir open failed ?!?! \n");
	}else{
		printf("director '.' opened \n");
	}

	if(-1==dir_write(fd, buf, 80)){
		printf("writing to directory failed \n");
	}else{
		printf("success?!?!? \n");
	}
	

}


/* read_file
 *   Inputs: fname:	name of file to read contents of and print to terminal
 *	 Outputs: file contents print to screen (except null chars) and a line indicating the file read
 *   Return Value: none
 * 	 Coverage: filesystem. file_open, file_read
 *   Function: Tests that files can be opened and read */
static void read_file(const uint8_t* fname){

	uint8_t buf[81]; 
	int k; 
	int fd; 


	fd = open(fname);
	//file_open(fname);

	uint32_t cnt; 
	while (0 != (cnt = read(fd, buf, 80))) {
        if (-1 == cnt) {
			printf(" file read failed\n");
	        break; 
	    }

		for(k=0; k<(cnt); k++){
			if(buf[k]!='\0'){
				putc(buf[k]);
			}
			
		}

    
	}

	printf(" \nfile: %s \n", fname);

	close(fd);
}


/* test_dir_read
 *   Inputs: none
 *	 Outputs: all the file names stored in the directory
 *   Return Value: none
 * 	 Coverage: filesystem. dir_open, dir_read
 *   Function: Tests that directory can be opened and read */
void test_dir_read(){
	uint8_t buf[32]; 
	uint8_t* fname=(uint8_t*)".";
	int fd; 

	clear(); 



	//dir_open(fname);
	fd = open(fname);


	// do an ls. that is, print all file names in directory 
	uint32_t cnt; 
	while (0 != (cnt = read(fd, buf, 32))) {
        if (-1 == cnt) {
			printf(" directory entry read failed\n");
	        break; 
	    }

		printf("%s \n", buf);
    
	}

	close(fd);
}


/* term_driver_test
 *   Inputs: none
 *	 Outputs: none
 *   Return Value: none
 * 	 Coverage: terminal driver
 *   Function: tests terminal driver read/write */
int term_driver_test(){
    
    int nbytes;
    char buf[128];

	terminal_open((const uint8_t*)"terminal");
    while (1) {
        /* Test 1 */
        printf("TEST 1: Max number of bytes = 10 \n");
        write(1, "Enter a string: \n",  20);
        
		nbytes = read(0, buf, 10);
        printf("Number of bytes written: %d\n", nbytes);

        /* Test 2 */
        printf("TEST 2: Max number of bytes = 128 \n");
        write(1, "Enter a string: \n",  20);

        nbytes = read(0, buf, 128);
        printf("Number of bytes written: %d\n", nbytes);

        /* Test 3 */
        printf("TEST 3: Max number of bytes = 0 \n");
        write(1, "Enter a string: \n",  20);

        nbytes = read(0, buf, 0);
        printf("Number of bytes written: %d\n", nbytes);

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

	rtc_test();
	//rtc_neg_test();

	//term_driver_test();

	// test reading small files
	//read_file((uint8_t*)"frame0.txt");
	//read_file((uint8_t*)"frame1.txt");
	//read_file((uint8_t*)"created.txt");

	// test reading executables
	//read_file((uint8_t*)"ls");
	//read_file((uint8_t*)"grep");
	//read_file((uint8_t*)"shell");
	//read_file((uint8_t*)"syserr");
	//read_file((uint8_t*)"sigtest");
	//read_file((uint8_t*)"cat");
	//read_file((uint8_t*)"pingpong");
	//read_file((uint8_t*)"counter");
	//read_file((uint8_t*)"hello");
	//read_file((uint8_t*)"testprint");
	read_file((uint8_t*)"frame0.txt");

	// test large files
	//read_file((uint8_t*)"verylargetextwithverylongname.tx");
	//read_file((uint8_t*)"fish");


	//test_filesys_fails();

	/* checkpoint 1 */
	//TEST_OUTPUT("idt_test", idt_test());

	//exceptions_test();

	//rtc_test(); 

	//syscall_idt_test(); 

	//test_paging_access(); 

	//test_paging_inaccess(); 

	//test_paging_access(); 


}
