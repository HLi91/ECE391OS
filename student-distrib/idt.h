/* This ECE391OS project is an in class project written by
*Hongchuan Li, Meng Gao, Junqing Deng, Han Jiang in UIUC (spring 2014)
*The current file is wriiten by Hongchuan Li
*Students who are taking ECE391 should AVOID copying this file
*/
#ifndef _IDR_H
#define _IDR_H

#include "x86_desc.h"
#include "lib.h"
#include "types.h"
#include "i8259.h"
#include "idt_init.h"

/* Privilege levels */
#define KERNEL_LEVEL 0
#define USER_LEVEL 3

/* Gate Size*/
#define SIZE32 1
#define SIZE16 0
#define PAGE_FAULT_NUM 14
#define IRQ_BASE 32
#define IRQ_MAX 16

/* a IRQ descriptor table that keeps track of interrupt handlers */
void *irq_records[NUM_VEC];

/* adds an entry to the IRQ descriptor table*/
void irq_install_handler(int irq, void (*handler)(struct regs r));

/* removes an entry to IRQ descriptor table*/
void irq_uninstall_handler(int irq);

/* sets ONE idt entry to appropriate value*/
extern void set_trap_gate(int32_t num, uint32_t base, int32_t dpl, int32_t size);
void set_intr_gate(int32_t num, uint32_t base, int32_t dpl, int32_t size);
void set_task_gate(int32_t num, int32_t dpl);

/*calls the appropriate handler */
extern void do_isr(regs r);
extern void do_irq(regs* r);

/*fills the IDT*/
void isrs_install();



#endif /* _IDR_H */

