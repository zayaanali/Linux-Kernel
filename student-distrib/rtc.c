#include "rtc.h"

/* rtc_init
 *   Inputs: none
 *   Return Value: none
 *    Function: Initialize RTC */

void rtc_init(){

    cli();			// disable interrupts
    outb(RTC_B, RTC_CMD_PORT);		// select register B, and disable NMI
    char prev = inb(RTC_DATA_PORT);	// read the current value of register B
    outb(RTC_B, RTC_CMD_PORT);		// set the index again (a read will reset the index to register D)
    outb(prev | 0x40, RTC_DATA_PORT);	// write the previous value ORed with 0x40. This turns on bit 6 of register B
    sti();

    enable_irq(RTC_IRQ);
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
    uint8_t new_freq = rtc_freq_to_config(freq); 
    uint8_t old_freq;
    cli(); 
    outb(RTC_A, RTC_CMD_PORT); 
    old_freq = inb(RTC_DATA_PORT);
    outb(RTC_A, RTC_CMD_PORT);
    outb((old_freq & 0xF0) | new_freq, RTC_DATA_PORT);
    sti();  
}

/* rtc_interrupt
 *   Inputs: none
 *   Return Value: none
 *    Function: what to do during RTC interrupts */
void rtc_interrupt(){
    outb(RTC_C, RTC_CMD_PORT);
    inb(RTC_DATA_PORT);
    send_eoi(RTC_IRQ);
}