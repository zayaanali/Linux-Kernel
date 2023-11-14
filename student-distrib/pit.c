#include "pit.h"

#define FREQ 18.222
#define LOWER_BYTE_MASK 0xFF
#define UPPER_BYTE_SHIFT 8

void pit_init(){
    uint32_t divisor = 1193180 / FREQ;

    outb(0x36, 0x43);  /* Set our command byte 0x36 */
    outb(divisor & MASK, 0x40);  /* Set low byte of divisor */
    outb(divisor >> SHIFT, 0x40);  /* Set high byte of divisor */
}
void pit_int_handler(){
    return;
}