/* This ECE391OS project is an in class project written by
*Hongchuan Li, Meng Gao, Junqing Deng, Han Jiang in UIUC (spring 2014)
*The current file is modified by Meng Gao and modified by others
*Students who are taking ECE391 should AVOID copying this file
*/

/* i8259.h - Defines used in interactions with the 8259 interrupt
 * controller
 * vim:ts=4 noexpandtab
 */

#ifndef _I8259_H
#define _I8259_H

#include "types.h"

/* Ports that each PIC sits on */
#define MASTER_8259_PORT 0x20
#define SLAVE_8259_PORT  0xA0
#define MASTER_DATA 0x21
#define SLAVE_DATA 0xA1

/* constants for IR masks*/
#define IR0_OFF 0x01
#define IR1_OFF 0x02
#define IR2_OFF 0x04
#define IR3_OFF 0x08 
#define IR4_OFF 0x10
#define IR5_OFF 0x20
#define IR6_OFF 0x40
#define IR7_OFF 0x80

/* IRQ Number on PIC */
#define IRQ_TIMER 0
#define IRQ_KEYBOARD 1
#define IRQ_SLAVE 2
#define IRQ_RTC 8

/* used to offset IDT index to get IRQ number */
#define IDT_OFFSET 32

/* used to offset slave IR number to 0~7 */
#define PIC_NEXT 8

/* Initialization control words to init each PIC.
 * See the Intel manuals for details on the meaning
 * of each word */
#define ICW1    0x11
#define ICW2_MASTER   0x20
#define ICW2_SLAVE    0x28
#define ICW3_MASTER   0x04
#define ICW3_SLAVE    0x02
#define ICW4          0x01

/* End-of-interrupt byte.  This gets OR'd with
 * the interrupt number and sent out to the PIC
 * to declare the interrupt finished */
#define EOI             0x60

/* Externally-visible functions */

/* Initialize both PICs */
void i8259_init(void);
/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num);
void mask_irq(uint32_t irq_num);
/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num);
void unmask_irq(uint32_t irq_num);
/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num);

#endif /* _I8259_H */
