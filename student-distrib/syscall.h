#ifndef _SYSCALL_H
#define _SYSCALL_H
#include "x86_desc.h"
#include "pcb.h"
#include "sig.h"

#define TYPE_RTC 0
#define TYPE_DIR 1
#define TYPE_FILE 2

#define PAGE_VID_MAP 3
#define NO_PTE 0

/*****************/
#define SHELL_CR3 0

#define TESTP_CR3 0
#define SHELL_ESP 0x83ffffc		//vertual address are the same for all program

#define SHELL_PCB 0x7fc000
#define KERNEL_PCB 0x7fe000
#define TEST_PCB 0x7fa000
#define SHELL_ESP0 0x7fdffc
#define TEST_ESP0 0x7fbffc
#define LOAD_ADD 0x848000
#define PROGRAM_SIZE 0x400000
#define LOAD_ADD2 0xc48000
#define ESP0_INTERVAL 0x2000

#define DESCRIPTOR_MSK 0xffffe000
#define ELF_MAGIC_0 0x7f
#define ELF_MAGIC_1 0x45
#define ELF_MAGIC_2 0x4c
#define ELF_MAGIC_3 0x46
#define FOUR_K 4096

/********************/

extern void syscall_init(void);

/* a descriptor table that keeps track of system call handlers */
//void *sys_call_table[10];

// kernel level system calls
extern int32_t trap_halt(uint32_t esp);
extern int32_t trap_execute(const uint8_t* command, uint32_t myesp, uint32_t myeip);
extern int32_t trap_read(pcb_t* mypcb, int32_t fd, void* buf, int32_t nbytes);
extern int32_t trap_write(pcb_t* mypcb, int32_t fd, const void* buf, int32_t nbytes);
extern int32_t trap_open(pcb_t* mypcb, const uint8_t* buf);
extern int32_t trap_close(pcb_t* mypcb, int32_t fd);
extern int32_t trap_getargs(pcb_t* mypcb, int8_t *buf, int32_t nbytes);
extern int32_t trap_vidmap(uint32_t myesp, uint8_t** screen_start);
extern int32_t trap_set_handler(pcb_t* mypcb, int32_t signum, void* handler_address); // extra credit
extern int32_t trap_sigreturn(sys_reg r); // extra credit
extern uint32_t trap_malloc();
extern uint32_t trap_modeX(uint32_t data, uint32_t cmd);


//int32_t exeStrnCmp(const uint8_t* command, uint8_t * stn);
void get_avg(const uint8_t* command, uint8_t* avg0, uint8_t*avg1, uint8_t*avg2, uint8_t*avg3);
void exe_helper_set_tss(uint32_t ss, uint32_t esp);
pcb_t * restore_tss(pcb_t * curpcb);

// initialize a fops table for each file type
extern void fops_init();

// standard installation
extern void std_install(fd_t* file_array);

// passes current bit map of bit
//extern int get_pid_map();

// pid assignment
// extern int kernel_stack_aval;

/*
typedef int32_t (*rw_t)(int32_t, const void*, int32_t, pcb_t*);
//int32_t (*handler)(int32_t fd, const void* buf, int32_t nbytes, pcb_t* mypcb);
rw_t handler;

extern fops_t STDIN_FOPS;
extern fops_t STDOUT_FOPS;
extern fops_t RTC_FOPS;
extern fops_t FILE_FOPS;
extern fops_t DIR_FOPS;
*/

void ufree_help(uint32_t cur);

// distinct fops tables for every file type
fops_t STDIN_FOPS;
fops_t STDOUT_FOPS;
fops_t RTC_FOPS;
fops_t FILE_FOPS;
fops_t DIR_FOPS;

#endif /* _SYSCALL_H */

