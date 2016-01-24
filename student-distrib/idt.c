/* This ECE391OS project is an in class project written by
*Hongchuan Li, Meng Gao, Junqing Deng, Han Jiang in UIUC (spring 2014)
*The current file is wriiten by Hongchuan Li
*Students who are taking ECE391 should AVOID copying this file
*/


#include "i8259.h"
#include "lib.h"
#include "x86_desc.h"
#include "idt.h"
#include "types.h"
#include "idt_init.h"
#include "x86_desc.h"
#include "pid.h"
#include "sig.h"
#define DEBUG 0
#define FAKE_EIP 0x12345

/*
 * isrs_install
 * DESCRITPION: fills the IDT table with appropriate values and handler
 * INPUT: none
 * OUTPUT: none
 * RETURN: none
 * SIDE EFFECT: writes the IDT
 */
void isrs_install()
{
	// set entries 1-50
    set_intr_gate(0,(unsigned)isr0, KERNEL_LEVEL, SIZE32);
	set_intr_gate(1,(unsigned)isr1, KERNEL_LEVEL, SIZE32);
	set_intr_gate(2,(unsigned)isr2, KERNEL_LEVEL, SIZE32);
	set_intr_gate(3,(unsigned)isr3, KERNEL_LEVEL, SIZE32);
	set_intr_gate(4,(unsigned)isr4, KERNEL_LEVEL, SIZE32);
	set_intr_gate(5,(unsigned)isr5, KERNEL_LEVEL, SIZE32);
	set_intr_gate(6,(unsigned)isr6, KERNEL_LEVEL, SIZE32);
	set_intr_gate(7,(unsigned)isr7, KERNEL_LEVEL, SIZE32);
	set_intr_gate(8,(unsigned)isr8, KERNEL_LEVEL, SIZE32);
	set_intr_gate(9,(unsigned)isr9, KERNEL_LEVEL, SIZE32);
	set_intr_gate(10,(unsigned)isr10, KERNEL_LEVEL, SIZE32);
	set_intr_gate(11,(unsigned)isr11, KERNEL_LEVEL, SIZE32);
	set_intr_gate(12,(unsigned)isr12, KERNEL_LEVEL, SIZE32);
	set_intr_gate(13,(unsigned)isr13, KERNEL_LEVEL, SIZE32);
	set_intr_gate(14,(unsigned)isr14, KERNEL_LEVEL, SIZE32);
	set_intr_gate(15,(unsigned)isr15, KERNEL_LEVEL, SIZE32);
	set_intr_gate(16,(unsigned)isr16, KERNEL_LEVEL, SIZE32);
	set_intr_gate(17,(unsigned)isr17, KERNEL_LEVEL, SIZE32);
	set_intr_gate(18,(unsigned)isr18, KERNEL_LEVEL, SIZE32);
	set_intr_gate(19,(unsigned)isr19, KERNEL_LEVEL, SIZE32);
	set_intr_gate(20,(unsigned)isr20, KERNEL_LEVEL, SIZE32);
	set_intr_gate(21,(unsigned)isr21, KERNEL_LEVEL, SIZE32);
	set_intr_gate(22,(unsigned)isr22, KERNEL_LEVEL, SIZE32);
	set_intr_gate(23,(unsigned)isr23, KERNEL_LEVEL, SIZE32);
	set_intr_gate(24,(unsigned)isr24, KERNEL_LEVEL, SIZE32);
	set_intr_gate(25,(unsigned)isr25, KERNEL_LEVEL, SIZE32);
	set_intr_gate(26,(unsigned)isr26, KERNEL_LEVEL, SIZE32);
	set_intr_gate(27,(unsigned)isr27, KERNEL_LEVEL, SIZE32);
	set_intr_gate(28,(unsigned)isr28, KERNEL_LEVEL, SIZE32);
	set_intr_gate(29,(unsigned)isr29, KERNEL_LEVEL, SIZE32);
	set_intr_gate(30,(unsigned)isr30, KERNEL_LEVEL, SIZE32);
	set_intr_gate(31,(unsigned)isr31, KERNEL_LEVEL, SIZE32);
	set_intr_gate(32,(unsigned)isr32, KERNEL_LEVEL, SIZE32);
	set_intr_gate(33,(unsigned)isr33, KERNEL_LEVEL, SIZE32);
	set_intr_gate(34,(unsigned)isr34, KERNEL_LEVEL, SIZE32);
	set_intr_gate(35,(unsigned)isr35, KERNEL_LEVEL, SIZE32);
	set_intr_gate(36,(unsigned)isr36, KERNEL_LEVEL, SIZE32);
	set_intr_gate(37,(unsigned)isr37, KERNEL_LEVEL, SIZE32);
	set_intr_gate(38,(unsigned)isr38, KERNEL_LEVEL, SIZE32);
	set_intr_gate(39,(unsigned)isr39, KERNEL_LEVEL, SIZE32);
	set_intr_gate(40,(unsigned)isr40, KERNEL_LEVEL, SIZE32);
	set_intr_gate(41,(unsigned)isr41, KERNEL_LEVEL, SIZE32);
	set_intr_gate(42,(unsigned)isr42, KERNEL_LEVEL, SIZE32);
	set_intr_gate(43,(unsigned)isr43, KERNEL_LEVEL, SIZE32);
	set_intr_gate(44,(unsigned)isr44, KERNEL_LEVEL, SIZE32);
	set_intr_gate(45,(unsigned)isr45, KERNEL_LEVEL, SIZE32);
	set_intr_gate(46,(unsigned)isr46, KERNEL_LEVEL, SIZE32);
	set_intr_gate(47,(unsigned)isr47, KERNEL_LEVEL, SIZE32);
	set_intr_gate(48,(unsigned)isr48, KERNEL_LEVEL, SIZE32);
	set_intr_gate(49,(unsigned)isr49, KERNEL_LEVEL, SIZE32);
	set_intr_gate(50,(unsigned)isr50, KERNEL_LEVEL, SIZE32);
	
}

/*
 * set_trap_gate
 * DESCRITPION: sets a trap entry in IDT
 * INPUT: num - the IDT entry index to set
 *        base - the pointer to linkage, used to calculate offset
 *        dpl - descriptor privilege level
 *        size - size of gate (0 for 16 bits, 1 for 32 bits)
 * OUTPUT: none
 * RETURN: none
 * SIDE EFFECT: writes the IDT
 */
void set_trap_gate(int32_t num, uint32_t base, int32_t dpl, int32_t size)
{

	SET_IDT_ENTRY(idt[num], base);
	idt[num].seg_selector = KERNEL_CS;
	idt[num].reserved4 = 0; // reserved bits
	idt[num].reserved3 = 1;
	idt[num].reserved2 = 1;
	idt[num].reserved1 = 1;
	idt[num].size = size;
	idt[num].reserved0 = 0;
	idt[num].dpl = dpl;
	idt[num].present = 1;
	
}


/*
 * set_intr_gate
 * DESCRITPION: sets an interrupt entry in IDT
 * INPUT: num - the IDT entry index to set
 *        base - the pointer to linkage, used to calculate offset
 *        dpl - descriptor privilege level
 *        size - size of gate (0 for 16 bits, 1 for 32 bits)
 * OUTPUT: none
 * RETURN: none
 * SIDE EFFECT: writes the IDT
 */
void set_intr_gate(int32_t num, uint32_t base, int32_t dpl, int32_t size)
{

	SET_IDT_ENTRY(idt[num], base);
	idt[num].seg_selector = KERNEL_CS;
	idt[num].reserved4 = 0; // reserved bits
	idt[num].reserved3 = 0;
	idt[num].reserved2 = 1;
	idt[num].reserved1 = 1;
	idt[num].size = size;
	idt[num].reserved0 = 0;
	idt[num].dpl = dpl;
	idt[num].present = 1;
	
}

/*
 * set_task_gate
 * DESCRITPION: sets a task entry in IDT
 * INPUT: num - the IDT entry index to set
 *        base - the pointer to linkage, used to calculate offset
 *        dpl - descriptor privilege level
 *        size - size of gate (0 for 16 bits, 1 for 32 bits)
 * OUTPUT: none
 * RETURN: none
 * SIDE EFFECT: writes the IDT
 */
void set_task_gate(int32_t num, int32_t dpl)
{
	idt[num].offset_15_00 = 0; // reserved bits
	idt[num].seg_selector = KERNEL_CS;
	idt[num].reserved4 = 0; // reserved bits
	idt[num].reserved3 = 1;
	idt[num].reserved2 = 0;
	idt[num].reserved1 = 1;
	idt[num].size = 0;
	idt[num].reserved0 = 0;
	idt[num].dpl = dpl;
	idt[num].present = 1;
	idt[num].offset_31_16 = 0; // reserved bits

}

/*
 * do_isr
 * DESCRITPION: calls the irq handler, or prints blue screen & halt
 * INPUT: r - the idt index of the exception that occurs
 * OUTPUT: exception number if an exception occurs
 * RETURN: none
 * SIDE EFFECT: calls do_irq if no exception
 */
extern void do_isr(regs r)
{
	int number = r.int_no;
	int real_address;

	// if the interrupt number falls within the PIC range
	if (number >= IRQ_BASE && number < IRQ_BASE+IRQ_MAX)
	{
		do_irq(&r);
	}
	// else it is exception/unknown interrupt
	else
	{
	
	if (r.cs == USER_CS){ // if user generates exception

	// send signal to kill program
		if (r.err_code == 0)
			write_signal(r.esp, SIG_DIVZ);
		else
			write_signal(r.esp, SIG_EXCEPT);
	}
	else{ // if kernel generates exception
	
	// extra help for page fault
		if(number==PAGE_FAULT_NUM){
		asm("movl %%cr2, %0 ;":"=r"(real_address));
		printf("page fault at 0x%x\n", real_address);
		printf("pid# is %d\n", get_cur_pid());
		}
	
	// print blue screen
		printf("exception no.%d \n", number);
		printf("eip is %x \nerror code is %x \n", r.eip, r.err_code);
		exception_block = 1;
		sti();
		for(;;);
	}
	}
	
	if (r.eip != FAKE_EIP && r.cs != KERNEL_CS)
		do_signal(&r);
}

/*
 * do_irq
 * DESCRITPION: calls the irq handler
 * INPUT: r - the idt index of the interrupt that occurs
 * OUTPUT: none
 * RETURN: none
 * SIDE EFFECT: calls interrupt handler
 */
extern void do_irq(regs* r){
	/* mask interrupt so that we can send EOI */
	mask_irq(r->int_no);
	
	/* send EOI (note: interrupt is not handled yet) */
	send_eoi(r->int_no);
	
	//printf("irq %d \n", r.int_no);
	
	/* call interrupt handler*/
	void (*handler)(struct regs* r);
	handler = irq_records[r->int_no];
	if (handler)
    {
        handler(r);
    }
	
	/* ok to unmask interrupt */
	unmask_irq(r->int_no);
	if(DEBUG)printf("                                                 \n");

}

/*
 * irq_install_handler
 * DESCRITPION: add a handler to the IRQ descriptor table
 * INPUT: irq - the irq number 
 * OUTPUT: none
 * RETURN: none
 * SIDE EFFECT: writes to irq_records[]
 */
void irq_install_handler(int irq, void (*handler)(struct regs r))
{
    irq_records[irq] = handler;
}

/*
 * irq_uninstall_handler
 * DESCRITPION: remove a handler from the IRQ descriptor table
 * INPUT: irq - the irq number 
 * OUTPUT: none
 * RETURN: none
 * SIDE EFFECT: writes to irq_records[]
 */
void irq_uninstall_handler(int irq)
{
    irq_records[irq] = 0;
}


