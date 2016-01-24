/* This ECE391OS project is an in class project written by
*Hongchuan Li, Meng Gao, Junqing Deng, Han Jiang in UIUC (spring 2014)
*The current file is modified by Meng Gao and modified by others
*Students who are taking ECE391 should AVOID copying this file
*/


/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"
#define DEBUG 0

/* Interrupt masks to determine which interrupts
 * are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7 */
uint8_t slave_mask; /* IRQs 8-15 */


/*
 * i8259_init
 * DESCRITPION: Initialize the 8259 PIC
 * INPUT: none
 * OUTPUT: none
 * RETURN: none
 * SIDE EFFECT: masks all interrupt
 */
void
i8259_init(void)
{
	uint32_t flags;
	
	// create a mask that masks all interrupt
	master_mask = IR0_OFF | IR1_OFF | IR2_OFF | IR3_OFF |
					IR4_OFF | IR5_OFF | IR6_OFF | IR7_OFF;
	slave_mask = master_mask;
	
	// critical section begins
	cli_and_save(flags);
	
	// mask all interrupt
	outb(master_mask, MASTER_DATA);
	outb(slave_mask, SLAVE_DATA);
	
	// initialize master
	outb(ICW1, MASTER_8259_PORT);		// ICW1: ICW4, cascade, 8bit edge trigger
	outb(ICW2_MASTER, MASTER_DATA);	    // ICW2: IR0 => 0x20... IR7 => 0x27
	outb(ICW3_MASTER, MASTER_DATA);		// ICW3: slave on IR2
	outb(ICW4, MASTER_DATA); 	// 80x86 mode, normal EOI, not buffered, sequential
	
	// initialize slave
	outb(ICW1, SLAVE_8259_PORT);		// ICW1: ICW4, cascade, 8bit edge trigger
	outb(ICW2_SLAVE, SLAVE_DATA);		// ICW2: IR0 => 0x28... IR7 => 0x2f
	outb(ICW3_SLAVE, SLAVE_DATA);		// ICW3: attached to IR2 on master
	outb(ICW4, SLAVE_DATA);		// 80x86 mode, normal EOI, not buffered, sequential
	
	// remove master mask on IR2 (slave)
	master_mask &= ~IR2_OFF;
	
	// mask all interrupt, and wait for device
	outb(master_mask, MASTER_DATA);
	outb(slave_mask, SLAVE_DATA);
	
	// critical section ends
	restore_flags(flags);
}

/*
 * enable_irq
 * DESCRITPION: Enable (unmask) the specified IRQ
 * INPUT: irq_num - the irq to enable, should be a index in IDT
 * OUTPUT: none
 * RETURN: none
 * SIDE EFFECT: unmask interrupt 
 */
void
enable_irq(uint32_t irq_num)
{
	// irq on master
	if (irq_num<PIC_NEXT && irq_num >=0){
		master_mask = inb(MASTER_DATA);
		master_mask &= ~(1<<irq_num);
		outb(master_mask, MASTER_DATA);
	}
	// irq on slave
	else if ((irq_num>=PIC_NEXT && irq_num <PIC_NEXT*2)){
		slave_mask = inb(SLAVE_DATA);
		slave_mask &= ~(1<<(irq_num-PIC_NEXT));
		outb(slave_mask, SLAVE_DATA);
	}
	// outside PIC range
	else{
	}
}


/*
 * unmask_irq
 * DESCRITPION: Enable (unmask) the specified IRQ
 * INPUT: irq_num - the irq to enable, should be a index in IDT
 * OUTPUT: none
 * RETURN: none
 * SIDE EFFECT: unmask interrupt 
 */
void
unmask_irq(uint32_t irq_num)
{
	if(DEBUG)printf("unmask_irq %d                    \n", irq_num);
	// irq on master
	if (irq_num<PIC_NEXT && irq_num >=0){
		master_mask = inb(MASTER_DATA);
		master_mask &= ~(1<<irq_num);
		outb(master_mask, MASTER_DATA);
	}
	// irq on slave
	else if ((irq_num>=PIC_NEXT && irq_num <PIC_NEXT*2)){
		
		slave_mask = inb(SLAVE_DATA);
		slave_mask &= ~(1<<(irq_num-PIC_NEXT));
		outb(slave_mask, SLAVE_DATA);
	}
	// outside PIC range
	else{
	}
}

/*
 * disable_irq
 * DESCRITPION: Disable (mask) the specified IRQ
 * INPUT: irq_num - the irq to disable, should be a index in IDT
 * OUTPUT: none
 * RETURN: none
 * SIDE EFFECT: mask interrupt
 */
void
disable_irq(uint32_t irq_num)
{
	// irq on master
	if (irq_num<PIC_NEXT && irq_num >=0){
		master_mask = inb(MASTER_DATA);
		master_mask &= (1<<irq_num);
		outb(master_mask, MASTER_DATA);
	}
	// irq on slave
	else if ((irq_num>=PIC_NEXT && irq_num <PIC_NEXT*2)){
		slave_mask = inb(SLAVE_DATA);
		slave_mask &= (1<<(irq_num-PIC_NEXT));
		outb(slave_mask, SLAVE_DATA);
	}
	// outside PIC range
	else{
	}
}


/*
 * mask_irq
 * DESCRITPION: Disable (mask) the specified IRQ
 * INPUT: irq_num - the irq to disable, should be a index in IDT
 * OUTPUT: none
 * RETURN: none
 * SIDE EFFECT: mask interrupt
 */
void
mask_irq(uint32_t irq_num)
{
	if(DEBUG)printf("mask_irq %d                    \n", irq_num);
	// irq on master
	if (irq_num<PIC_NEXT && irq_num >=0){
		master_mask = inb(MASTER_DATA);
		master_mask &= (1<<irq_num);
		outb(master_mask, MASTER_DATA);
	}
	// irq on slave
	else if ((irq_num>=PIC_NEXT && irq_num <PIC_NEXT*2)){
		slave_mask = inb(SLAVE_DATA);
		slave_mask &= (1<<(irq_num-PIC_NEXT));
		outb(slave_mask, SLAVE_DATA);
	}
	// outside PIC range
	else{
	}
}

/*
 * send_eoi
 * DESCRITPION: Send end-of-interrupt signal for the specified IRQ
 * INPUT: irq_num - the index in IDT
 * OUTPUT: none
 * RETURN: none
 * SIDE EFFECT: tells the PIC about a end of interrupt
 */
void
send_eoi(uint32_t irq_num)
{
	if(DEBUG)printf("send eoi                    \n");
	
	// offset the IDT index to get IRQ#
	irq_num-=IDT_OFFSET;
	
	// if on master, send EOI to master
	if (irq_num<PIC_NEXT && irq_num >=0)
	{
		outb(EOI|irq_num, MASTER_8259_PORT); 
	}
	// if on slave, send EOI to both master & slave
	else if ((irq_num>=PIC_NEXT && irq_num <PIC_NEXT*2))
	{
		outb(EOI|IRQ_SLAVE, MASTER_8259_PORT); 
		outb(EOI|(irq_num-PIC_NEXT), SLAVE_8259_PORT);
	}
	// outside PIC range
	else{
	}
}

