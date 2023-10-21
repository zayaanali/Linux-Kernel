#include "rtc.h"
#include "x86_desc.h"
#include "i8259.h"
#include "lib.h"

static volatile int INT_FLAG;

extern void rtc_link(); 

/* rtc_init
 *   Inputs: none
 *   Return Value: none
 *    Function: Initialize RTC */

void rtc_init(){

    //cli();			                // disable interrupts .. this function called within clearing of interrupts
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

   

   // enable_irq(RTC_IRQ);
   

    // sti();
}

/* frequency_to_rate
 *   Inputs: frequency
 *   Return Value: rate
 *    Function: frequency =  32768 >> (rate-1);  */
uint8_t freq_to_rate(uint16_t freq) {
    if (freq == 0) {
        return 0;
    }

    uint8_t rate = 1;
    uint16_t base = 32768;

    while (base > freq) {
        rate++;
        base >>= 1;

        if (rate > 15) {
            return 0;
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

int32_t rtc_open(const uint8_t* filename){
    rtc_set_freq(2);
    return 0;
}
int32_t rtc_close(int32_t fd){
    return 0;
}
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes){
    while(INT_FLAG == 0){
        ;
    }
    INT_FLAG = 0; 
    return 1;
}
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes){
    uint16_t buf_freq = (uint16_t)* (uint16_t*)buf;
    rtc_set_freq(buf_freq);
    return 0;
}

/* rtc_handler
 *   Inputs: none
 *   Return Value: none
 *    Function: what to do during RTC interrupts */
void rtc_handler(){
    INT_FLAG = 1;
    outb(RTC_C, RTC_CMD_PORT);
    inb(RTC_DATA_PORT);
    // test_interrupts(); 
    send_eoi(RTC_IRQ);

    
}
