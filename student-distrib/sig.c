/* This ECE391OS project is an in class project written by
*Hongchuan Li, Meng Gao, Junqing Deng, Han Jiang in UIUC (spring 2014)
*The current file is modified by Meng Gao
*Students who are taking ECE391 should AVOID copying this file
*/

#include "sig.h"

#define SIGRETURN_VAL   10

#define SIGNAL_ARRAY_SIZE  5

// variable check is NOT used in any meaningful way anymore.
// it is replaced by the sig_block bit map in pcb
static volatile int check;

// if there is any interrupt signal pending
volatile uint32_t pending_sigint;


/*
 * write_signal
 * Description: modifies the signal bitmap to block a signal
 * INPUTS: esp - used to find pcb
 *         sig_num - the signal to invoke
 * OUTPUTS:	None. 
 * RETURN: 0 - sucess
 *        -1 - fail
 * SIDE EFFECTS: writes to pcb
 */
int write_signal(unsigned int esp, int sig_num){
	// called from interrupt
	/* Implicit assumption: r->esp is NOT changed!!!!! */
	
	// get pcb from esp
	pcb_t* mypcb = (pcb_t*)(esp & DESCRIPTOR_MSK);
	
	// update signal bitmap
	(mypcb->sig_map) |= (1<<sig_num );
	
	return 0;
}

/*
 * block_signal
 * Description: modifies the block bitmap to block a signal
 * INPUTS: esp - used to find pcb
 *         sig_num - the signal to block
 * OUTPUTS:	None. 
 * RETURN: None
 * SIDE EFFECTS: writes to pcb
 * Other note: this function is not used in current system.
 *          please offer a unblock function if anyone wants to 
 *          use it.
 */
int block_signal(unsigned int esp, int sig_num){
	// called from interrupt
	/* Implicit assumption: r->esp is NOT changed!!!!! */
	
	// get pcb from esp
	pcb_t* mypcb = (pcb_t*)(esp & DESCRIPTOR_MSK);
	
	// update blocked signal bitmap
	(mypcb->sig_block) |= (1<<sig_num );
	
	return 0;
}

/*
 * setup_frame
 * Description: 
 * INPUTS: sig_num - signal number
 *         r - pointer to the kernel copy of hardware context 
 *         mypcb - the pcb where signal information/handler is stored 
 *                 (note that this can be calculated even if there's no such input)
 * OUTPUTS:	None. 
 * RETURN: None
 * SIDE EFFECTS: None
 */
int setup_frame(int sig_num, regs* r, pcb_t* mypcb){
	sig_frame_t* temp_frame;
	
	//check = 1;
	// get stack frame top from user's esp, and align to 8
	temp_frame = (sig_frame_t*)((r->useresp-sizeof(sig_frame_t)) & (~0x7));	
	//sig_frame_t* frame = get_physical_add((uint32_t)temp_frame, (uint32_t)mypcb->cr3);
	sig_frame_t* frame = temp_frame;	
	
	// push return address
	frame->ret_addr = (uint32_t*) (r->useresp - sizeof(uint8_t[RET_CODE_LENGTH]));
	// push signal number
	frame->sig_num = sig_num;
	
	// save hardware context
	frame->context.gs = r->gs;
	frame->context.fs = r->fs;
	frame->context.es = r->es;
	frame->context.ds = r->ds;
	
	frame->context.edi = r->edi;
	frame->context.esi = r->esi;
	frame->context.ebp = r->ebp;
	frame->context.esp = r->esp;
	frame->context.ebx = r->ebx;
	frame->context.edx = r->edx;
	frame->context.ecx = r->ecx;
	frame->context.eax = r->eax;
	
	frame->context.int_no = r->int_no;
	frame->context.err_code = r->err_code;
	
	frame->context.eip = r->eip;
	frame->context.cs = r->cs;
	frame->context.eflags = r->eflags;
	frame->context.useresp = r->useresp;
	frame->context.ss = r->ss;
	
	
	
	/* invoke sigreturn
	 * popl %eax		# pop out signal number
	 * movl $10, %eax	# save syscall number
	 * int 0x80			# no other parameter needed, invoke syscall
	 */
	/*************following code taken from linux source***************/
		static const struct { 
			uint16_t poplmovl;
			uint32_t val;
			uint16_t int80;    
			uint16_t pad; 
		} __attribute__((packed)) code = { 
			0xb858,		 /* popl %eax ; movl $...,%eax */
	SIGRETURN_VAL,   
			0x80cd,		/* int $0x80 */
			0,
		}; 
	/*************above code taken from linux source******************/
	//memcpy(&(frame->ret_code[0]), &code, 8);
	
	memcpy(frame->ret_addr, &code, RET_CODE_LENGTH);
	
	/**************************************************************
	 * Alternatively, pre-set eax, and just call int 0x80
	 * Note that this code has NEVER been tested
	 **************************************************************/
	//uint32_t code2 = 0x80cd;
	//memcpy(frame->ret_code, &code2, 2);
	
	
		
	// set up registers for signal handler
	// values will be popped into registers by interrupt wrapper
	r->useresp = (unsigned int)temp_frame;
	r->eip = (unsigned int)mypcb->sig_act[sig_num];
	
	r->eax = SIGRETURN_VAL;
	r->ebx = 0;
	r->ecx = 0;
	r->edx = 0;

	r->ds = USER_DS;
	r->es = USER_DS;
	r->ss = USER_DS;
	r->cs = USER_CS;

	return 0;
}


/*
 * do_signal
 * Description: handles signal. This function only get called when interrupt
 *              returns to user level (NOT syscall)
 * INPUTS: r - pointer to the kernel copy of hardware context 
 * OUTPUTS:	None. 
 * RETURN: None
 * SIDE EFFECTS: None
 */
void do_signal(regs* r){
	// called from interrupt
	/* Implicit assumption: r->esp is NOT changed!!!!! */
	int sig_num;
	uint32_t flags;
	cli_and_save(flags);
	
	if (r->cs == KERNEL_CS || check == 1)
		return;
	
	// get pcb from esp
	pcb_t* mypcb = (pcb_t*)(r->esp & DESCRIPTOR_MSK);
	
	// check if there is any pending signal
	if ((mypcb->sig_map) == 0)
		return;
	
	//check signal 0~4 & get signal info
	for (sig_num = 0; sig_num < SIGNAL_ARRAY_SIZE; sig_num++) {
		if (((mypcb->sig_map)>>sig_num) & 1)
			break;
	}
	
	// check if signal is blocked
	if ((mypcb->sig_block)>>sig_num)
		return;
	
	// mark signal as blocked BEFORE handle signal
	mypcb->sig_block |= ( 1 <<sig_num);
	
	// block all signal
	// mypcb->sig_block = 0xff;
	
	// mark signal as executed
	mypcb->sig_map &= ~( 1 <<sig_num);
	


	
	
	
	
	
	setup_frame(sig_num, r, mypcb);
	
	
	restore_flags(flags);
	// other signal won't be executed until next do_signal call
	return;
}

/*
 * setup_frame_syscall
 * Description: sets up the user level stack frame for signal handling
 * INPUTS: r - pointer to the kernel copy of hardware context
 * OUTPUTS:	None. 
 * RETURN: eax - the saved copy of the eax, since syscall does not restore eax
 * SIDE EFFECTS: tear down to user level stack, restore hardware context
 */
int restore_sigcontext(sys_reg* r){

	uint32_t flags;
	cli_and_save(flags);
	
	
	pcb_t* mypcb = (pcb_t*)(r->esp & DESCRIPTOR_MSK);
	
	// get kernel frame address
	sig_frame_t* temp_frame = (sig_frame_t*)((r->useresp-sizeof(uint32_t)-sizeof(uint32_t)) & (~0x7));
	//sig_frame_t* frame = get_physical_add((uint32_t)temp_frame, (uint32_t)mypcb->cr3);
	sig_frame_t* frame = temp_frame;
	
	// restore hardware context
	/***************************************************************
	 * NOTE: syscall linkage stack and interrupt linkage stack are DIFFERENT!!!!!
	 **************************************************************/
	
	r->gs = frame->context.gs;
	r->fs = frame->context.fs;
	r->es = frame->context.es;
	r->ds = frame->context.ds;
	
	r->edi = frame->context.edi;
	r->esi = frame->context.esi;
	r->ebp = frame->context.ebp;
	r->esp = frame->context.esp;
	r->ebx = frame->context.ebx;
	r->edx = frame->context.edx;
	r->ecx = frame->context.ecx;
	//r->eax = frame->context.eax;	//return value, syscall do not restore eax
	
	r->int_no = frame->context.int_no;
	r->err_code = frame->context.err_code;
	
	r->eip = frame->context.eip;
	r->cs = frame->context.cs;
	r->eflags = frame->context.eflags;
	r->useresp = frame->context.useresp;
	r->ss = frame->context.ss;
	
	//mypcb->sig_block = 0;
	//check = 0;
	mypcb->sig_block &= ~( 1 <<(frame->sig_num));
	restore_flags(flags);
	return frame->context.eax;	// restore eax by return its old value
}






/*******************************************************************/
/*
 * check_signal
 * Description: check if there is any unblocked signal pending
 * INPUTS: esp - used to find the program's pcb, where signal info is saved
 * OUTPUTS:	None. 
 * RETURN: 0 - no pending signal
 *         1 - signal pending
 * SIDE EFFECTS: None
 */
int check_signal(uint32_t esp){
	pcb_t* mypcb = (pcb_t*)(esp & DESCRIPTOR_MSK);
	
	if (mypcb == 0)
		return 0;
	
	if (((mypcb->sig_map) & ~(mypcb->sig_block)) != 0)
		return 1;
	
	return 0;
}

/*
 * setup_frame_syscall
 * Description: sets up the user level stack frame for signal handling
 * INPUTS: sig_num - the signal number
 *         r - pointer to the kernel copy of hardware context
 *                 (note that this can be calculated even if there's no such input)
 * OUTPUTS:	None. 
 * RETURN: None
 * SIDE EFFECTS: writes to user level stack
 */
int setup_frame_syscall(int sig_num, sys_reg* r, pcb_t* mypcb){
	sig_frame_t* temp_frame;
	
	//check = 1;
	// get stack frame top from user's esp, and align to 8
	temp_frame = (sig_frame_t*)((r->useresp-sizeof(sig_frame_t)) & (~0x7));	
	//sig_frame_t* frame = get_physical_add((uint32_t)temp_frame, (uint32_t)mypcb->cr3);
	sig_frame_t* frame = temp_frame;	
	
	// push return address
	frame->ret_addr = (uint32_t*) (r->useresp - sizeof(uint8_t[RET_CODE_LENGTH]));
	// push signal number
	frame->sig_num = sig_num;
	
	// save hardware context
	frame->context.gs = r->gs;
	frame->context.fs = r->fs;
	frame->context.es = r->es;
	frame->context.ds = r->ds;
	
	frame->context.edi = r->edi;
	frame->context.esi = r->esi;
	frame->context.ebp = r->ebp;
	frame->context.esp = r->esp;
	frame->context.ebx = r->ebx;
	frame->context.edx = r->edx;
	frame->context.ecx = r->ecx;
	frame->context.eax = r->eax;
	
	frame->context.int_no = r->int_no;
	frame->context.err_code = r->err_code;
	
	// offset eip to reissue the system call
	frame->context.eip = r->eip-2;
	frame->context.cs = r->cs;
	frame->context.eflags = r->eflags;
	frame->context.useresp = r->useresp;
	frame->context.ss = r->ss;
	
	
	
	/* invoke sigreturn
	 * popl %eax		# pop out signal number
	 * movl $10, %eax	# save syscall number
	 * int 0x80			# no other parameter needed, invoke syscall
	 */
	/*************following code taken from linux source***************/
		static const struct { 
			uint16_t poplmovl;
			uint32_t val;
			uint16_t int80;    
			uint16_t pad; 
		} __attribute__((packed)) code = { 
			0xb858,		 /* popl %eax ; movl $...,%eax */
	SIGRETURN_VAL,   
			0x80cd,		/* int $0x80 */
			0,
		}; 
	/*************above code taken from linux source******************/
	//memcpy(&(frame->ret_code[0]), &code, 8);
	
	memcpy(frame->ret_addr, &code, RET_CODE_LENGTH);
	
	/**************************************************************
	 * Alternatively, pre-set eax, and just call int 0x80
	 * Note that this code has NEVER been tested
	 **************************************************************/
	//uint32_t code2 = 0x80cd;
	//memcpy(frame->ret_code, &code2, 2);
	
	
		
	// set up registers for signal handler
	// values will be popped into registers by interrupt wrapper
	r->useresp = (unsigned int)temp_frame;
	r->eip = (unsigned int)mypcb->sig_act[sig_num];
	
	r->eax = SIGRETURN_VAL;
	r->ebx = 0;
	r->ecx = 0;
	r->edx = 0;

	r->ds = USER_DS;
	r->es = USER_DS;
	r->ss = USER_DS;
	r->cs = USER_CS;

	return 0;
}

/*
 * do_signal_syscall
 * Description: handles signal, note that this function is only called from syscall handler
 *              when it returns to user level
 * INPUTS: r - the hardware context saved by syscall handler
 * OUTPUTS:	None. 
 * RETURN: None
 * SIDE EFFECTS: jumps to signal handler when syscall irets
 */
void do_signal_syscall(sys_reg r){
	// called from syscall handler
	/* Implicit assumption: r->esp is NOT changed!!!!! */
	int sig_num;
	uint32_t flags;
	cli_and_save(flags);
	
	if (r.cs == KERNEL_CS || check == 1)
		return;
	
	// get pcb from esp
	pcb_t* mypcb = (pcb_t*)(r.esp & DESCRIPTOR_MSK);
	
	// check if there is any pending signal
	if ((mypcb->sig_map) == 0)
		return;
	
	//check signal 0~4 & get signal info
	for (sig_num = 0; sig_num < SIGNAL_ARRAY_SIZE; sig_num++) {
		if (((mypcb->sig_map)>>sig_num) & 1)
			break;
	}
	
	// check if signal is blocked
	if ((mypcb->sig_block)>>sig_num)
		return;
	
	// mark signal as blocked BEFORE handle signal
	mypcb->sig_block |= ( 1 <<sig_num);
	
	// block all signal
	// mypcb->sig_block = 0xff;
	
	// mark signal as executed BEFORE handle signal
	mypcb->sig_map &= ~( 1 <<sig_num);
	

	
	
	
	setup_frame_syscall(sig_num, &r, mypcb);
	
	
	restore_flags(flags);
	// other signal won't be executed until next do_signal call
	return;
}




