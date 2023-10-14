#ifndef _RTC_H_
#define _RTC_H_

#include "i8259.h"
#include "lib.h"

#define RTC_CMD_PORT 0x70
#define RTC_DATA_PORT 0x71
#define RTC_A 0x8A
#define RTC_B 0x8B
#define RTC_C 0x8C
#define RTC_IRQ 8

void rtc_init();
uint8_t freq_to_rate(uint16_t freq);
void rtc_set_freq(uint16_t freq);
void rtc_interrupt();

#endif