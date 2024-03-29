#ifndef _RTC_H_
#define _RTC_H_

#define RTC_CMD_PORT 0x70
#define RTC_DATA_PORT 0x71
#define RTC_A 0x8A
#define RTC_B 0x8B
#define RTC_C 0x8C
#define RTC_IRQ 8

#include "types.h"

void rtc_init();
uint8_t freq_to_rate(uint16_t freq);
void rtc_set_freq(uint16_t freq);

extern void rtc_handler();

extern int32_t rtc_open(const uint8_t* filename);
extern int32_t rtc_close(int32_t fd);
extern int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes);
extern int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes);

#endif
