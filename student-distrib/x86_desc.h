/* This ECE391OS project is an in class project written by
*Hongchuan Li, Meng Gao, Junqing Deng, Han Jiang in UIUC (spring 2014)
*The current file is wriiten by Hongchuan Li and modified by others
*Students who are taking ECE391 should AVOID copying this file
*/


/* x86_desc.h - Defines for various x86 descriptors, descriptor tables, 
 * and selectors
 * vim:ts=4 noexpandtab
 */

#ifndef _X86_DESC_H
#define _X86_DESC_H

#include "types.h"


/* Segment selector values */
#define KERNEL_CS 0x0010
#define KERNEL_DS 0x0018
#define USER_CS 0x0023
#define USER_DS 0x002B
#define KERNEL_TSS 0x0030
#define KERNEL_LDT 0x0038
#define KMALC 0x40000
#define KMALCIND 0x100
#define KMALCR 0x40000
#define KMALCRI 0x8000



/* Size of the task state segment (TSS) */
#define TSS_SIZE 104

/* Number of vectors in the interrupt descriptor table (IDT) */
#define NUM_VEC 256

#define PAGE_SIZE 1024
#define BUFFER_SIZE 128

#define PID_NUM 11



/* Number of entries in page directory (PD) and page table (PT)*/
#define NUM_PG_ENTRY 1024

#ifndef ASM

/* This structure is used to load descriptor base registers
 * like the GDTR and IDTR */
typedef struct x86_desc {
	uint16_t padding;
	uint16_t size;
	uint32_t addr;
} x86_desc_t;

/* This is a segment descriptor.  It goes in the GDT. */
typedef struct seg_desc {
	union {
		uint32_t val;
		struct {
			uint16_t seg_lim_15_00;
			uint16_t base_15_00;
			uint8_t base_23_16;
			uint32_t type : 4;
			uint32_t sys : 1;
			uint32_t dpl : 2;
			uint32_t present : 1;
			uint32_t seg_lim_19_16 : 4;
			uint32_t avail : 1;
			uint32_t reserved : 1;
			uint32_t opsize : 1;
			uint32_t granularity : 1;
			uint8_t base_31_24;
		} __attribute__((packed));
	};
} seg_desc_t;

typedef struct __attribute__((packed)) PDE4_t
{
    uint32_t Present : 1;
	uint32_t rw : 1;
	uint32_t us : 1;
	uint32_t pwt : 1;
	uint32_t pcd : 1;
	uint32_t a : 1;
	uint32_t d : 1;
	uint32_t ps : 1;			//page size
	uint32_t g : 1;
	uint32_t avail : 3;
	uint32_t base : 20;
	
	
} PDE4_t;





typedef struct PID{
	uint32_t present;
	uint32_t eip;
	uint32_t esp;
	uint32_t ebp;
	uint32_t cr3;
	uint32_t pte;
	uint32_t tss_esp0;
	uint32_t tss_ss0;
	int32_t ticks;
	uint32_t sleep;
	uint32_t terminal_num;
	uint32_t parent_pid;
	uint32_t mem;
	uint8_t * name;



	//other things like cursor should be added
	//variable defined in pcb.h

}PID;


typedef struct mmPDE4_t{
	uint32_t  next;
	PDE4_t curPDE;
}mmPDE4_t;



typedef int (*sig_handler)(void);

typedef struct __attribute__((packed)) sig_regs
{
    unsigned int ebx, ecx, edx, esi, edi, ebp, eax; 
	unsigned int ds, es, fs; 
    unsigned int int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss;
	unsigned int gs, esp; /*not used*/
} sig_regs_t;

typedef struct __attribute__((packed)) regs
{
    unsigned int gs, fs, es, ds;      /* pushed the segs last */
    unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;  /* pushed by 'pusha' */
    unsigned int int_no, err_code;    /* our 'push byte #' and ecodes do this */
    unsigned int eip, cs, eflags, useresp, ss;   /* pushed by the processor automatically */ 
} regs;


typedef struct __attribute__((packed)) syscall_regs
{
    unsigned int ebx, ecx, edx;      /* pushed arguments */
    unsigned int esi, edi, ebp, eax, ds, es;  /* pushed by syscall */
    unsigned int fake_eax;    /* NOT USED */
	unsigned int fs, gs, esp, int_no, err_code;
    unsigned int eip, cs, eflags, useresp, ss;   /* pushed by the processor automatically */ 
} sys_reg;

/* TSS structure */
typedef struct __attribute__((packed)) tss_t {
	uint16_t prev_task_link;
	uint16_t prev_task_link_pad;

	uint32_t esp0;
	uint16_t ss0;
	uint16_t ss0_pad;

	uint32_t esp1;
	uint16_t ss1;
	uint16_t ss1_pad;

	uint32_t esp2;
	uint16_t ss2;
	uint16_t ss2_pad;

	uint32_t cr3;

	uint32_t eip;
	uint32_t eflags;

	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;

	uint16_t es;
	uint16_t es_pad;

	uint16_t cs;
	uint16_t cs_pad;

	uint16_t ss;
	uint16_t ss_pad;

	uint16_t ds;
	uint16_t ds_pad;

	uint16_t fs;
	uint16_t fs_pad;

	uint16_t gs;
	uint16_t gs_pad;

	uint16_t ldt_segment_selector;
	uint16_t ldt_pad;

	uint16_t debug_trap : 1;
	uint16_t io_pad : 15;
	uint16_t io_base_addr;
} tss_t;


/* Some external descriptors declared in .S files */
extern x86_desc_t gdt_desc;

extern uint16_t ldt_desc; 
extern uint32_t ldt_size;
extern seg_desc_t ldt_desc_ptr;
extern seg_desc_t gdt_ptr;
extern uint32_t ldt;

extern uint32_t tss_size;
extern seg_desc_t tss_desc_ptr;
extern tss_t tss;

extern PDE4_t kpde[PAGE_SIZE];
extern PDE4_t kpte[PAGE_SIZE];
extern PDE4_t upde[PAGE_SIZE];
extern PDE4_t upde2[PAGE_SIZE];
extern PDE4_t upte[PAGE_SIZE];
extern PDE4_t upte2[PAGE_SIZE];
extern uint8_t Kmal[KMALC];
extern uint8_t KmalR[KMALCR];
extern uint8_t KmalRI[KMALCRI];

extern uint32_t pending_kill;
extern uint32_t pending_kill_id;

extern uint32_t pending_kill;

extern uint32_t mods_addrs;

extern uint32_t gdt_size;
extern uint32_t exception_block;

extern PID kernel_pid[PID_NUM];

extern volatile uint32_t cur_terminal;

extern int modex_line;
extern int display_pic;



extern volatile uint32_t pending_halt;


extern volatile uint32_t modeX_enabled;


/* Sets runtime-settable parameters in the GDT entry for the LDT */
#define SET_LDT_PARAMS(str, addr, lim) \
do { \
	str.base_31_24 = ((uint32_t)(addr) & 0xFF000000) >> 24; \
		str.base_23_16 = ((uint32_t)(addr) & 0x00FF0000) >> 16; \
		str.base_15_00 = (uint32_t)(addr) & 0x0000FFFF; \
		str.seg_lim_19_16 = ((lim) & 0x000F0000) >> 16; \
		str.seg_lim_15_00 = (lim) & 0x0000FFFF; \
} while(0)

/* Sets runtime parameters for the TSS */
#define SET_TSS_PARAMS(str, addr, lim) \
do { \
	str.base_31_24 = ((uint32_t)(addr) & 0xFF000000) >> 24; \
		str.base_23_16 = ((uint32_t)(addr) & 0x00FF0000) >> 16; \
		str.base_15_00 = (uint32_t)(addr) & 0x0000FFFF; \
		str.seg_lim_19_16 = ((lim) & 0x000F0000) >> 16; \
		str.seg_lim_15_00 = (lim) & 0x0000FFFF; \
} while(0)

/* An interrupt descriptor entry (goes into the IDT) */


typedef union idt_desc_t {
	uint32_t val;
	struct {
		uint16_t offset_15_00;
		uint16_t seg_selector;
		uint8_t reserved4;
		uint32_t reserved3 : 1;
		uint32_t reserved2 : 1;
		uint32_t reserved1 : 1;
		uint32_t size : 1;
		uint32_t reserved0 : 1;
		uint32_t dpl : 2;
		uint32_t present : 1;
		uint16_t offset_31_16;
	} __attribute__((packed));
} idt_desc_t;

/*
typedef struct lbuffer_t {
	unsigned char data[BUFFER_SIZE];
	int size;
} lbuffer_t;

// line buffers for terminal #1,2,3
extern lbuffer_t buffer[3], buffer2[3], c_buffer[3];
*/
/*
// line buffers for terminal #1
extern lbuffer_t buffer, buffer2, c_buffer;
// line buffers for terminal #2
extern lbuffer_t buffer, buffer2, c_buffer;
// line buffers for terminal #3
extern lbuffer_t buffer, buffer2, c_buffer;
*/

/* The IDT itself (declared in x86_desc.S */
extern idt_desc_t idt[NUM_VEC];
/* The descriptor used to load the IDTR */
extern x86_desc_t idt_desc_ptr;

/* Sets runtime parameters for an IDT entry */
#define SET_IDT_ENTRY(str, handler) \
do { \
	str.offset_31_16 = ((uint32_t)(handler) & 0xFFFF0000) >> 16; \
		str.offset_15_00 = ((uint32_t)(handler) & 0xFFFF); \
} while(0)

/* Load task register.  This macro takes a 16-bit index into the GDT,
 * which points to the TSS entry.  x86 then reads the GDT's TSS
 * descriptor and loads the base address specified in that descriptor
 * into the task register */
#define ltr(desc)                       \
do {                                    \
	asm volatile("ltr %w0"              \
			:                           \
			: "r" (desc)                \
			: "memory", "cc" );         \
} while(0)

/* Load the interrupt descriptor table (IDT).  This macro takes a 32-bit
 * address which points to a 6-byte structure.  The 6-byte structure
 * (defined as "struct x86_desc" above) contains a 2-byte size field
 * specifying the size of the IDT, and a 4-byte address field specifying
 * the base address of the IDT. */
#define lidt(desc)                      \
do {                                    \
	asm volatile("lidt (%0)"            \
			:                           \
			: "g" (desc)                \
			: "memory");                \
} while(0)

/* Load the local descriptor table (LDT) register.  This macro takes a
 * 16-bit index into the GDT, which points to the LDT entry.  x86 then
 * reads the GDT's LDT descriptor and loads the base address specified
 * in that descriptor into the LDT register */
#define lldt(desc)                      \
do {                                    \
	asm volatile("lldt %%ax"            \
			:                           \
			: "a" (desc)                \
			: "memory" );               \
} while(0)

#endif /* ASM */

#endif /* _x86_DESC_H */


