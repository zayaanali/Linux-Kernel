/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"


/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* i8259_init
 *   Inputs: none
 *   Return Value: none
 *    Function: Initialize master/slave PIC */
void i8259_init(void) {
    /* Don't need spinlock/or mask interrupt bc we only initialize on start */

    /* Set to cascade mode */
    outb(ICW1, MASTER_8259_PORT);
    outb(ICW1, SLAVE_8259_PORT);

    /* Set interrupt vector offset (where to start interrupts from interrupt table) */
    outb(ICW2_MASTER, MASTER_8259_DATA);
    outb(ICW2_SLAVE, SLAVE_8259_DATA);
    
    /* Set up cascading */
    outb(ICW3_MASTER, MASTER_8259_DATA); // Set slave on IRQ2
    outb(ICW3_SLAVE, SLAVE_8259_DATA); // Set slave on IRQ2

    /* Set to 8086 mode */
    outb(ICW4, MASTER_8259_DATA);
    outb(ICW4, SLAVE_8259_DATA);

    /* enable the slave port */
    enable_irq(IRQ_SLAVE);


}

/* enable_irq
 *   Inputs: irq_num - IRQ number to enable
 *   Return Value: none
 *    Function: Enables the specified IRQ */
void enable_irq(uint32_t irq_num) {
    int irq_line_num;
    
    /* Check if IRQ number is valid */
    if (irq_num >= NUM_IRQS || irq_num < 0)
        return;

    /* Get IRQ line to enable on PIC */
    irq_line_num = irq_num % 8;
    
    /* Send OCW1 command (set IRQ_num bit to 0 to enable) */
    if (irq_num < 8) { // enable on master
        master_mask &= get_mask(irq_line_num, 0);
        outb(master_mask, MASTER_8259_DATA);
    } else { // enable on slave
        slave_mask &= get_mask(irq_line_num, 0);
        outb(slave_mask, SLAVE_8259_DATA);
    }

    return;
}

/* enable_irq
 *   Inputs: irq_num - IRQ number to disable
 *   Return Value: none
 *    Function: disables the specified IRQ */
void disable_irq(uint32_t irq_num) {
    int irq_line_num;
    
    /* Check if IRQ number is valid */
    if (irq_num >= NUM_IRQS || irq_num < 0)
        return;

    /* Get IRQ line to disable on PIC */
    irq_line_num = irq_num % 8;
    
    /* Send OCW1 command (set IRQ_num bit to 1 to disable) */
    if (irq_num < 8) { // mask on master
        master_mask |= get_mask(irq_line_num, 1);
        outb(master_mask, MASTER_8259_DATA);
    } else { // mask on slave
        slave_mask |= get_mask(irq_line_num, 1);
        outb(slave_mask, SLAVE_8259_DATA);
    }

    return;
}

/* send_eoi
 *   Inputs: irq_num - Send EOI for this IRQ
 *   Return Value: none
 *    Function: Sends EOI for specified IRQ. Bottom 4 bits is IRQ signal (format for specific EOI) */
void send_eoi(uint32_t irq_num) {
    int irq_line_num;
    uint8_t master_eoi, slave_eoi;
    
    /* Check if IRQ number is valid */
    if (irq_num >= NUM_IRQS || irq_num < 0)
        return;

    /* Get IRQ line to on PIC (master/secondary) */
    irq_line_num = irq_num % 8;
    
    /* Calculate and send EOI */
    if (irq_num < 8) { // EOI on master
        master_eoi = EOI | irq_line_num;
        outb(master_eoi, MASTER_8259_PORT);
    } else { // EOI on slave. Must send master EOI as well
        slave_eoi = EOI | irq_line_num;
        master_eoi = EOI | IRQ_SLAVE;
        
        outb(slave_eoi, SLAVE_8259_PORT);
        outb(master_eoi, MASTER_8259_PORT);
        
    }

    return;


}

/*  get_mask
 *   Inputs: irq_num - IRQ number to enable, mode - 0 for disable, 1 for enable
 *   Return Value: mask - mask to enable/disable irq_num
 *   Function: Helper function sets a 0/1 in the irq_num(th) bit so that it can be used as
 *              a mask to set that bit  */
uint8_t get_mask(uint32_t irq_num, int mode) {
    if (mode==0) // set zero at bit irq_num
        return ~(1 << irq_num); 
    else if (mode==1) // set one at bit irq_num
        return (1 << irq_num);
    else 
        return -1;
}
