/* This ECE391OS project is an in class project written by
*Hongchuan Li, Meng Gao, Junqing Deng, Han Jiang in UIUC (spring 2014)
*The current file is modified by Meng Gao
*Students who are taking ECE391 should AVOID copying this file
*/

#ifndef _SIG_H
#define _SIG_H

#include "idt.h"
#include "syscall.h"
#include "syscall_wrap.h"
#include "pcb.h"
#include "filesys.h"
#include "x86_desc.h"
#include "lib.h"
#include "lbuffer.h"
#include "paging.h"
#include "file_op.h"
#include "kmalloc.h"
#include "pid.h"
#include "sched.h"
#include "sig.h"

#define RET_CODE_LENGTH   8
#define SIG_DIVZ 0
#define SIG_EXCEPT 1
#define SIG_INTERRUPT 2
#define SIG_ALARM 3
#define SIG_NEVER 4


typedef struct sig_frame{
	uint32_t* ret_addr;
	int sig_num;
	sig_regs_t context;	// hardware context
	uint8_t ret_code[RET_CODE_LENGTH];
} sig_frame_t;


extern volatile uint32_t pending_sigint;

extern int sig_init();
extern int sig_ignore();
extern int sig_kill();
extern int write_signal(unsigned int esp, int sig_num);
extern int block_signal(unsigned int esp, int sig_num);
extern int restore_sigcontext(sys_reg* r);
extern void do_signal(regs* r);
//extern void do_signal_syscall(sys_reg* r);
extern int check_signal(uint32_t esp);
extern void do_signal_syscall(sys_reg r);


#endif /* _SIG_H */

