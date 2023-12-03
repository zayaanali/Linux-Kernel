#include "rtc.h"
#include "x86_desc.h"
#include "i8259.h"
#include "lib.h"
#include "pcb.h"
#include "terminal.h"
#include "scheduler.h"

// to ensure only 1 process enables/disables irq line for RTC
uint32_t RTC_EN_FLAG =0; 

extern void rtc_link(); 


/* rtc_init
 *   Inputs: none
 *   Return Value: none
 *    Function: Initialize RTC */

void rtc_init(){

    outb(RTC_B, RTC_CMD_PORT);		// select register B, and disable NMI
    char prev = inb(RTC_DATA_PORT);	// read the current value of register B
    outb(RTC_B, RTC_CMD_PORT);		// set the index again (a read will reset the index to register D)
    outb(prev | 0x40, RTC_DATA_PORT);	// write the previous value ORed with 0x40. This turns on bit 6 of register B

    // put interrupt gate in idt
    // set reserved0|D|reserved1|reserved2|reserved3|reserved4[8] to 0|1|1|1|0|0000 0000 for 32-bit interrupt gate
    idt[40].present = 1;             // segment is present
    idt[40].dpl = 0;                 // DPL = 00 (highest priority)
    idt[40].reserved0 = 0;           
    idt[40].size = 1;                // size (D) = 1 (32 bit gate)
    idt[40].reserved1 = 1;
    idt[40].reserved2 = 1;
    idt[40].reserved3 = 0;
    idt[40].reserved4 = 0;
    idt[40].seg_selector = KERNEL_CS;

    // link idt entry to rtc liner function
    SET_IDT_ENTRY(idt[40], rtc_link);

    rtc_set_freq(1024);

}

/* frequency_to_rate
 *   Inputs: frequency
 *   Return Value: rate
 *    Function: frequency =  32768 >> (rate-1);  */
uint8_t freq_to_rate(uint16_t freq) {

    //Check if frequency is in range
    if (freq == 0) {
        return 1;
    }

    uint8_t rate = 1;
    uint16_t base = 32768;

    while (base > freq) {
        rate++;
        base >>= 1;

        if (rate > 13) {
            return 13;
        }
    }

    return rate;
}

/* rtc_set_freq
 *   Inputs: none
 *   Return Value: none
 *    Function: set frequency of RTC interrupts */
void rtc_set_freq(uint16_t freq) {
    uint8_t new_freq = freq_to_rate(freq); 

    uint8_t old_freq;
    cli(); 
    outb(RTC_A, RTC_CMD_PORT); 
    old_freq = inb(RTC_DATA_PORT);
    outb(RTC_A, RTC_CMD_PORT);
    outb((old_freq & 0xF0) | new_freq, RTC_DATA_PORT);
    sti();  
}


/* rtc_open
 *   Inputs: filename
 *   Return Value: fd if successfully insert into file array, -1 for failure
 *    Function: "open" rtc by setting default frequency, enabling interrupt, and inserting entry into file array  */
int32_t rtc_open(const uint8_t* filename){
    // rtc_set_freq(2);

    /*Set Virtual Frequency to 2HZ*/
    terminals[active_tid].V_FREQ_NUM = (1024 / 2);

    if(RTC_EN_FLAG==0){
        enable_irq(RTC_IRQ);
        RTC_EN_FLAG=1; 
    }

    return insert_into_file_array(&rtc_funcs, -1);      // inode not relevant for rtc, send invalid value
}


/* rtc_close
 *   Inputs: filename
 *   Return Value: 0 if successfully closed, else -1
 *    Function: "close" rtc by removing its file array entry  */
int32_t rtc_close(int32_t fd){

    if(RTC_EN_FLAG==1){
        disable_irq(RTC_IRQ);
        RTC_EN_FLAG=0;
    }
   

    return remove_from_file_array(fd); 
}


/* rtc_read
 *   Inputs: fd, buf, nbytes (none of these used)
 *   Return Value: 0 when read
 *    Function: reads from RTC by waiting for interrupt  */
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes){
    terminals[active_tid].INT_FLAG = 0;
    while(terminals[active_tid].INT_FLAG == 0){
        ;
    }
    terminals[active_tid].INT_FLAG = 0; 
    return 0;
}


/* rtc_write
 *   Inputs: fd, buf, nbytes  (only buf used)
 *   Return Value: 0 
 *    Function: writes a frequency to RTC indicated by value in buf  */
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes){
    uint16_t buf_freq = (uint16_t)* (uint16_t*)buf;
    
    //Check if frequency is a power of 2
    if(((buf_freq & (buf_freq - 1)) != 0)){
        return -1;
    }
    //Check if frequency is in range
    if (buf_freq == 0 || buf_freq > 1024) {
        return -1;
    }
    // rtc_set_freq(buf_freq);
    terminals[active_tid].V_FREQ_NUM = (1024 / buf_freq);
    return 0;
}

/* rtc_handler
 *   Inputs: none
 *   Return Value: none
 *    Function: what to do during RTC interrupts */
void rtc_handler(){
    if(terminals[active_tid].INT_COUNT > terminals[active_tid].V_FREQ_NUM){
        terminals[active_tid].INT_FLAG = 1;
        terminals[active_tid].INT_COUNT = 0;
    }
    terminals[active_tid].INT_COUNT++; //
    outb(RTC_C, RTC_CMD_PORT);
    inb(RTC_DATA_PORT);
    // test_interrupts(); 
    send_eoi(RTC_IRQ);

    
}
