/* This ECE391OS project is an in class project written by
*Hongchuan Li, Meng Gao, Junqing Deng, Han Jiang in UIUC (spring 2014)
*The current file is wriiten by Hongchuan Li and modified by others
*Students who are taking ECE391 should AVOID copying this file
*/

/* kernel.c - the C part of the kernel
 * vim:ts=4 noexpandtab
 */

/*
 * NOTE:
 * A simple CTRL + C is implemented for testing purpose (primarily for pingpong), 
 * it is able to terminate ANY process, but it should NOT be used on shell.
 */
#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "debug.h"
#include "idt.h"
#include "rtc.h"
#include "keyboard.h"
#include "paging.h"
#include "filesys.h"
#include "pcb.h"
#include "syscall.h"
#include "syscall_wrap.h"
#include "kmalloc.h"
#include "syscall_wrap.h"
#include "pit.h"
#include "sched.h"
#include "pid.h"
#include "gen_asm.h"
#include "modex.h"

#define FOUR_K 4096
#define LEVEL_0 0;
#define TSS_SIZE_MASK_19_16 0x000F0000
#define TSS_SIZE_MASK_15_00 0x0000FFFF
#define NAME_LENGTH 10
#define BUFFER_SIZE_100 100
#define PIT_INIT_VAL 1193

/* Macros. */
/* Check if the bit BIT in FLAGS is set. */
#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))
static const int INDEX_33 = 33;
static const int START_NUM_2 = 2;
static const int BITS_THIRTY_TWO = 32;

void exception_test(int idt_index);
void run_exe();

/* Check if MAGIC is valid and print the Multiboot information structure
   pointed by ADDR. */
void
entry (unsigned long magic, unsigned long addr)
{
	multiboot_info_t *mbi;

	/* Clear the screen. */
	clear();

	/* Am I booted by a Multiboot-compliant boot loader? */
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
	{
		printf ("Invalid magic number: 0x%#x\n", (unsigned) magic);
		return;
	}

	/* Set MBI to the address of the Multiboot information structure. */
	mbi = (multiboot_info_t *) addr;

	/* Print out the flags. */
	printf ("flags = 0x%#x\n", (unsigned) mbi->flags);

	/* Are mem_* valid? */
	if (CHECK_FLAG (mbi->flags, 0))
		printf ("mem_lower = %uKB, mem_upper = %uKB\n",
				(unsigned) mbi->mem_lower, (unsigned) mbi->mem_upper);

	/* Is boot_device valid? */
	if (CHECK_FLAG (mbi->flags, 1))
		printf ("boot_device = 0x%#x\n", (unsigned) mbi->boot_device);

	/* Is the command line passed? */
	if (CHECK_FLAG (mbi->flags, 2))
		printf ("cmdline = %s\n", (char *) mbi->cmdline);

	if (CHECK_FLAG (mbi->flags, 3)) {
		int mod_count = 0;
		int i;
		module_t* mod = (module_t*)mbi->mods_addr;
		mods_addrs = (uint32_t)((mod_t*)mbi->mods_addr)->mod_start;
		while(mod_count < mbi->mods_count) {
			printf("Module %d loaded at address: 0x%#x\n", mod_count, (unsigned int)mod->mod_start);
			printf("Module %d ends at address: 0x%#x\n", mod_count, (unsigned int)mod->mod_end);
			printf("First few bytes of module:\n");
			for(i = 0; i<16; i++) {
				printf("0x%x ", *((char*)(mod->mod_start+i)));
			}
			printf("\n");
			mod_count++;
		}
	}
	/* Bits 4 and 5 are mutually exclusive! */
	if (CHECK_FLAG (mbi->flags, 4) && CHECK_FLAG (mbi->flags, 5))
	{
		printf ("Both bits 4 and 5 are set.\n");
		return;
	}

	/* Is the section header table of ELF valid? */
	if (CHECK_FLAG (mbi->flags, 5))
	{
		elf_section_header_table_t *elf_sec = &(mbi->elf_sec);

		printf ("elf_sec: num = %u, size = 0x%#x,"
				" addr = 0x%#x, shndx = 0x%#x\n",
				(unsigned) elf_sec->num, (unsigned) elf_sec->size,
				(unsigned) elf_sec->addr, (unsigned) elf_sec->shndx);
	}

	/* Are mmap_* valid? */
	if (CHECK_FLAG (mbi->flags, 6))
	{
		memory_map_t *mmap;

		printf ("mmap_addr = 0x%#x, mmap_length = 0x%x\n",
				(unsigned) mbi->mmap_addr, (unsigned) mbi->mmap_length);
		for (mmap = (memory_map_t *) mbi->mmap_addr;
				(unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
				mmap = (memory_map_t *) ((unsigned long) mmap
					+ mmap->size + sizeof (mmap->size)))
			printf (" size = 0x%x,     base_addr = 0x%#x%#x\n"
					"     type = 0x%x,  length    = 0x%#x%#x\n",
					(unsigned) mmap->size,
					(unsigned) mmap->base_addr_high,
					(unsigned) mmap->base_addr_low,
					(unsigned) mmap->type,
					(unsigned) mmap->length_high,
					(unsigned) mmap->length_low);
	}

	/* Construct an LDT entry in the GDT */
	{
		seg_desc_t the_ldt_desc;
		the_ldt_desc.granularity    = 0;
		the_ldt_desc.opsize         = 1;
		the_ldt_desc.reserved       = 0;
		the_ldt_desc.avail          = 0;
		the_ldt_desc.present        = 1;
		the_ldt_desc.dpl            = LEVEL_0;
		the_ldt_desc.sys            = 0;
		the_ldt_desc.type           = 0x2;

		SET_LDT_PARAMS(the_ldt_desc, &ldt, ldt_size);
		ldt_desc_ptr = the_ldt_desc;
		lldt(KERNEL_LDT);
	}

	/* Construct a TSS entry in the GDT */
	{
		seg_desc_t the_tss_desc;
		the_tss_desc.granularity    = 0;
		the_tss_desc.opsize         = 0;
		the_tss_desc.reserved       = 0;
		the_tss_desc.avail          = 0;
		the_tss_desc.seg_lim_19_16  = TSS_SIZE & TSS_SIZE_MASK_19_16;
		the_tss_desc.present        = 1;
		the_tss_desc.dpl            = LEVEL_0;
		the_tss_desc.sys            = 0;
		the_tss_desc.type           = 0x9;
		the_tss_desc.seg_lim_15_00  = TSS_SIZE & TSS_SIZE_MASK_15_00;

		SET_TSS_PARAMS(the_tss_desc, &tss, tss_size);

		tss_desc_ptr = the_tss_desc;

		tss.ldt_segment_selector = KERNEL_LDT;
		tss.ss0 = KERNEL_DS;
		tss.esp0 = 0x800000;
		ltr(KERNEL_TSS);
	}
	
	// fill the IDT entries
	isrs_install();

	exception_block = 0;			//initial exception ctrl+L
	call_fill_p();
	


	//////////////////r_kmal test///////////

	/*uint32_t addr1 = r_kmalloc(2);
	uint32_t addr2 = r_kmalloc(2);
	printf("addr1 is %x\n", addr1);
	printf("addr2 is %x\n", addr2);
	r_kfree(addr1);
	addr1 = r_kmalloc(2);
	printf("addr1 is %x\n", addr1);

	


	while(1);*/



	/////////////graphic mode test/////////////////

	//graphic_mode();
	//set_mode_X();
	//update_statusBar(0,0,0);
	//while(1);
	









	//////////////////////////////////////
	
	/**********************************/
	/*CP2 READ_DATA demo code*/
	dentry_t* dentry;

	read_dentry_by_index(1, dentry);
	uint8_t buf[400];
	int a = read_data(dentry->index_node, 0, buf, 200);
	initial_paging_sc();
	printf("%s \n", buf);
	printf("Length Read: %d \n", a);
	
	//while(1){}
	
	/**********************************/
	
	// enable paging & set up pages
	paging_init();
	
	/* Initialize devices, memory, filesystem, enable device interrupts on the
	 * PIC, any other initialization stuff... */
	
	// Initialize the PIC
	i8259_init();
	
	// Initialize the keyboard
	keyboard_install();

	// Initialize a fop table for each file type
	fops_init();

	// initializes syscall entry & signal action table
	syscall_init();
	
	// Initialize the rtc
	rtc_init();
		
	/* Enable interrupts */
	/* Do not enable the following until after you have set up your
	 * IDT correctly otherwise QEMU will triple fault and simple close
	 * without showing you any output */
	printf("Enabling Interrupts\n");

	clear();
	uint8_t shell[NAME_LENGTH] = "shell";
	dentry_t mydentry;
	read_dentry_by_name(shell, &mydentry);
	uint8_t buf1[BUFFER_SIZE_100];

	read_data(mydentry.index_node, 0, buf1, BUFFER_SIZE_100 );
	//printf("%s", buf1);
	sti();
	
	//rtc_cp2_test();

	PDE4_t * myUpde = (PDE4_t *)kmalloc(FOUR_K, 1);
	PDE4_t * myUpde2 = (PDE4_t *)kmalloc(FOUR_K, 1);
	printf ("myUpde is %x\n", myUpde);
	printf ("myUpde2 is %x\n", myUpde2);


	//initial kpdt
	int i;
	for (i = START_NUM_2; i < BITS_THIRTY_TWO; i++) {
	init_kpde_4MB(i, i);
	}
	
	init_upde(upde, 0, 0, KERNEL, PAGE_4MB, NO_PTE, NO_PTE);
	init_upde(upde, 1, 1, KERNEL, PAGE_4MB, NO_PTE, NO_PTE);
	init_upde(upde, BITS_THIRTY_TWO, START_NUM_2, USER, PAGE_4MB, NO_PTE,
			NO_PTE);
	init_upde(upde, INDEX_33, 0, USER, PAGE_VID_MAP, upte, (uint32_t) &upte);

	init_upde(upde2, 0, 0, KERNEL, PAGE_4MB, NO_PTE, NO_PTE);
	init_upde(upde2, 1, 1, KERNEL, PAGE_4MB, NO_PTE, NO_PTE);
	init_upde(upde2, BITS_THIRTY_TWO, 3, USER, PAGE_4MB, NO_PTE, NO_PTE);
	init_upde(upde2, INDEX_33, 0, USER, PAGE_VID_MAP, upte2, (uint32_t) &upte2);

	initial_pit(PIT_INIT_VAL);
	initial_mm(4);

	initial_all_pid(run_exe);
	init_sched();

	//run_exe();

	
	

	printf("done!!\n");

	/* test exception */
	//exception_test(14);

	/* Execute the first program (`shell') ... */

	/* Spin (nicely, so we don't chew up cycles) */
	asm volatile(".1: hlt; jmp .1;");
}

/*
 * run_exe
 * Description: start a new shell. 
 * INPUTS: None
 * OUTPUTS:	Print out a message after exiting a shell. 
 * RETURN: None
 * SIDE EFFECTS: None
 */
void run_exe()
{
	//int8_t userNameBuf[NUM_PROCESS_ALLOW];
	//int8_t user[NUM_PROCESS_ALLOW] = "user";
	//static uint8_t mypw[NUM_PROCESS_ALLOW] = "group01";
	uint8_t shell[NAME_LENGTH] = "shell";
	sti();
	while(1){
/*
		printf ("userName: ");
		stdin_read(0, userNameBuf, 10, 0);
		if (!strncmp2(userNameBuf, user, 10))
		{
			printf("Password: ");
			uint8_t pwbuf[100];
			sc_pw_enable(1);
			stdin_read(0, pwbuf, 100, NULL);
			
			sc_pw_enable(0);
			if (!strncmp2((const int8_t *)mypw, (const int8_t *)pwbuf, 10))
			{
				printf("\n");
			
			
			}else
			{
			
				printf("password incorrect\n try again...\n");
				continue;
			
			}
		}else
			continue;
*/


		myexe(shell);
		printf("Just exited from shell. Restarting...\n");
	}

}
/*
void
exception_test(int idt_index){
	int test;
	int* test_pt;
	
	switch (idt_index){
	//divide by 0
	case 0:	test = 5/0; break;
	// page fault
	case 14: test_pt = NULL; test = *(test_pt); break;
	default: break;
	}
	
}
*/

