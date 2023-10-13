/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"
#include "spinlock.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void i8259_init(void) {
    /* Don't need spinlock/or mask interrupt bc we only initialize on start */

    /* Set to cascade mode */
    outb(ICW1, MASTER_8259_PORT);
    outb(ICW1, SLAVE_8259_PORT);

    /* Set interrupt vector offset (where to start interrupts from interrupt table) */
    outb(ICW2_MASTER, MASTER_8259_PORT);
    outb(ICW2_SLAVE, SLAVE_8259_PORT);
    
    /* Set up cascading */
    outb(ICW3_MASTER, MASTER_8259_PORT); // Set slave on IRQ2
    outb(ICW3_SLAVE, SLAVE_8259_PORT); // Set slave on IRQ2

    /* Set to 8086 mode */
    outb(ICW4, MASTER_8259_PORT);
    outb(ICW4, SLAVE_8259_PORT);


}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
}
