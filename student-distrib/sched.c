/* This ECE391OS project is an in class project written by
*Hongchuan Li, Meng Gao, Junqing Deng, Han Jiang in UIUC (spring 2014)
*The current file is wriiten by Hongchuan Li
*Students who are taking ECE391 should AVOID copying this file
*/

#include "sched.h"
#include "syscall.h"
#include "gen_asm.h"
#include "lib.h"

#include "syscall_wrap.h"

#define SWITCH_PROGRAM 0
#define SWITCH_SYSCALL 1

#define TIME_QUANTUM 10
#define TICK_DIVISION 10000

//static int tick=0;

/* init_sched
 * Description: initialize scheduling: enable irq0, install sched handler
 * INPUTS:	None
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: None
 */
void init_sched(){
	irq_install_handler(IDT_OFFSET + IRQ_TIMER, (void *) update_tick);
	enable_irq(IRQ_TIMER);
	/*sti();*/
	
}
/* disable_sched
 * Description: disable scheduling
 * INPUTS:	None
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: None
 */
void disable_sched()
{

	irq_install_handler(IDT_OFFSET + IRQ_TIMER, 0);


}

/* check_pid
 * Description: checks pid availability
 * INPUTS:	none
 * OUTPUTS:	None
 * RETURN: 0 on sucess
 *        -1 if fail
 *         1 if no program running
 * SIDE EFFECTS: write into IDT
 */
int check_pid(int pid){
	int pid_map = get_pid_map();
	if (pid_map == 0)
		return 1;
	if (((pid_map >> pid) & 0x1) == 1)
		return 0;

	return -1;
}

/* update_tick
 * Description: update tick of the kernel pid. 
 * INPUTS:	regs registers -- the struct that contains registers. 
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: None
 */
void update_tick(regs * registers)
{
	static uint32_t signal_tick;
	uint32_t pid_ind = get_cur_pid();
	kernel_pid[pid_ind].ticks--;
	display_video(cur_terminal);
	if (pending_halt == 1 && cur_terminal == kernel_pid[pid_ind].terminal_num)
	{
		
		printf("program terminated by keyboard\n");
		//exception_block = 0;
		set_vm_stutus();
		pending_halt = 0;
		halt();
		return;
	
	}
	
	// send interrupt signal to currently displaying program
	if (pending_sigint == 1 && cur_terminal == kernel_pid[pid_ind].terminal_num)
	{
		
		//printf("program terminated by keyboard\n");
		//set_vm_stutus();
		pending_sigint = 0;
		write_signal(registers->esp, SIG_INTERRUPT);
		return;
	
	}

	if (pending_kill == 1 && pending_kill_id == pid_ind)
	{
		
		printf("program is killed\n");
		//exception_block = 0;
		set_vm_stutus();
		pending_kill = 0;
		halt();
		return;
	
	}

	// send alarm signal every 10 sec
	if (++signal_tick > TICK_DIVISION && cur_terminal == kernel_pid[pid_ind].terminal_num){
		write_signal(registers->esp, SIG_ALARM);
		signal_tick = 0;
	}
	
	if (kernel_pid[pid_ind].ticks <= 0)
	{
	
		scheduler(registers);
		kernel_pid[pid_ind].ticks = 1;
	
	}
	

}

#define FAKE_EIP  0x12345
/* scheduler
 * Description: handling scheduling. 
 * INPUTS:	regs registers -- the struct that contains registers. 
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: None
 */
void scheduler(regs * registers){
	
	
	uint32_t pid_ind = get_cur_pid();
	uint32_t next_pid_ind = get_next_pid();
	if (pid_ind == next_pid_ind)
		return;

	

	uint32_t esp, ebp, eip;
	asm volatile("mov %%esp, %0" : "=r"(esp));
	asm volatile("mov %%ebp, %0" : "=r"(ebp));
	
	/**********************
	 * IMPORTANT: 
	 * The following code uses C calling convention to get eip
	 * this value is saved in eax as defined in C calling convention
	 **********************
	 * Further implication:
	 * eip == FAKE_EIP checks cmpl $FAKE_EIP, %eax
	 **********************/
	eip = read_eip();

	if (eip == FAKE_EIP)
	{
		
       return;
	}
	

	kernel_pid[pid_ind].ebp = ebp;
	kernel_pid[pid_ind].esp = esp;
	kernel_pid[pid_ind].eip = eip;

	eip = kernel_pid[next_pid_ind].eip;
	esp = kernel_pid[next_pid_ind].esp;
	ebp = kernel_pid[next_pid_ind].ebp;

	set_cur_pid(next_pid_ind);

	
	tss.esp0 = kernel_pid[next_pid_ind].tss_esp0;
	


	asm volatile("         \
     mov %0, %%ecx;       \
     mov %1, %%esp;       \
     mov %2, %%ebp;       \
     mov %3, %%cr3;       \
     mov $0x12345, %%eax; \
     jmp *%%ecx           "
	 : : "r"(eip), "r"(esp), "r"(ebp), "r"(kernel_pid[next_pid_ind].cr3));


	 

				
	
}

/* context_switch
 * Description: handling operations for the context switch
 * INPUTS:	int type  -- the type of switch
            pcb_t* mypcb -- pointer to the process control block. 
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: None
 */
void context_switch(int type, pcb_t* mypcb){
/*
	// restore register value when switching in
	asm volatile(
		"movl %0, %%eax	;"
		"movl %1, %%ebx	;"
		"movl %2, %%ecx	;"
		"movl %3, %%edx	;"
		"movl %4, %%esi	;"
		"movl %5, %%edi	;"
		"movl %6, %%esp	;"
		"movl %7, %%ebp	;"
		: // no output
		:"g"(mypcb->eax), "g"(mypcb->ebx), "g"(mypcb->ecx), "g"(mypcb->edx),
			"g"(mypcb->esi), "g"(mypcb->edi), "g"(mypcb->esp), "g"(mypcb->ebp)
		: "eax", "ebx", "ecx", "edx", "edi", "esi"
		);

		
	switch(type){
	case SWITCH_PROGRAM: 
		asm volatile(
			"cli					;"
			//save eax
			"pushl %%eax			;"
			//restore segment reg
			"movl  %0, %%eax		;"
			"movl  %%eax, %%fs		;"
			"movl  %%eax, %%gs		;"
			"movl  %%eax, %%ds		;"
			
			"pushl %1				;"
			"pushl %2				;"
			"pushl %3				;"
			"pushl %4				;"
			"iret					;"
			: // no output
			: "g"(mypcb->ds), "g"(mypcb->esp), "g"(mypcb->eflags), "g"(mypcb->cs), "g"(mypcb->eip)
			: "eax","memory"
			);
		break;
	case SWITCH_SYSCALL: break;
	default: break;
	}
*/
}

