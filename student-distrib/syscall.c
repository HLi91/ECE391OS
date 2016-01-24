/* This ECE391OS project is an in class project written by
*Hongchuan Li, Meng Gao, Junqing Deng, Han Jiang in UIUC (spring 2014)
*The current file is wriiten by Hongchuan Li and modified by others
*Students who are taking ECE391 should AVOID copying this file
*/

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
#include "modex.h"
#include "BMP.h"

#define VIDEO_END_ADDR  0x8400000
#define VIDEO_START_ADDR  0x8000000
#define LARGEST_FILE_DISCRIPTOR_ID  7
#define THIRTY_THREE  33
#define THIRTY_TWO  32
#define OFFSET_TEN_THOUSAND  10000
#define FILE_BUF_SIZE 24
#define STRING_LENGTH 40
#define PWBUF_SIZE_100  100
#define ARRAY_SIZE_10  10
#define FILE_ARRAY_SIZE  8
#define ESP_MASK 0xffffe000
#define SINGAL_HANDLER_2  0x8402000
#define SIGNAL_HANDLER_1  0x8401000
#define SYSCALL_INT_NO  128
#define KERNEL_PCB_OFFSET  0x2000
#define USER_PDE_SIZE  1000
#define MEMO_SPACE  0x400000
#define SIG_NUM_MAX 5


//only for debug use, this should be replaced by the correct value
#define AVGMAX 32

/* for keeping number of track of programs*/
//static int program_num_count = 0;

//static int kernel_stack = SHELL_ESP0;

/*signal default handler storage*/
sig_handler default_sig_act[SIG_NUM_MAX];

/* syscall_init
 * Description: set system call entry in IDT
 * INPUTS:	none
 * OUTPUTS:	None
 * RETURN: none
 * SIDE EFFECTS: write into IDT
 */
void syscall_init(void){
	set_trap_gate(SYSCALL_INT_NO, (unsigned) system_call, USER_LEVEL, SIZE32);

	// initializes default signal action
	default_sig_act[0] = (sig_handler) SIGNAL_HANDLER_1;
	default_sig_act[1] = (sig_handler) SIGNAL_HANDLER_1;
	default_sig_act[2] = (sig_handler) SIGNAL_HANDLER_1;
	default_sig_act[3] = (sig_handler) SINGAL_HANDLER_2;
	default_sig_act[4] = (sig_handler) SINGAL_HANDLER_2;
}



/* init_pcb
 * Description: initialize pcb at input location
 * INPUTS:	mypcb - pointer to the start of pcb
 * OUTPUTS:	None
 * RETURN: none
 * SIDE EFFECTS: clear all fields in pcb
 */
void init_pcb(pcb_t * mypcb)
{
	mypcb->avg1[0] = '\0';
	mypcb->avg2[0] = '\0';
	mypcb->avg3[0] = '\0';
	mypcb->cr3 = 0;
	mypcb->cr3Saved = 0;
	mypcb->sig_map = 0;
	mypcb->sig_block = 0;
	mypcb->sig_act[0] = default_sig_act[0];
	mypcb->sig_act[1] = default_sig_act[1];
	mypcb->sig_act[2] = default_sig_act[2];
	mypcb->sig_act[3] = default_sig_act[3];
	mypcb->sig_act[4] = default_sig_act[4];
	mypcb->file_location = 0;
	int i;
	for (i = 0; i < FILE_ARRAY_SIZE; i++)
	{
		mypcb->file_array[i].file_position = 0;
		mypcb->file_array[i].flags = 0;
		/*
		mypcb->file_array[i].fop_table[0] = NULL;
		mypcb->file_array[i].fop_table[1] = NULL;
		mypcb->file_array[i].fop_table[2] = NULL;
		mypcb->file_array[i].fop_table[3] = NULL;
		*/
		mypcb->file_array[i].fops_pt = NULL;
		mypcb->file_array[i].inode_pt = NULL;
	}

}

/* trap_error
 * Description: invalid handler index
 * INPUTS:	none
 * OUTPUTS:	None
 * RETURN: -1 - indicates failure
 * SIDE EFFECTS: none
 */
int32_t trap_error(void){
	return -1;
}


/* trap_halt
 * Description: Terminates a process, return specified value to parent process
            NOTE: This call NEVER return to caller!!
 * INPUTS:	status - the specified value to return
 *          NOTE: the input is EIGHT(8) bits in BL
 * OUTPUTS:	None
 * RETURN: status - the input to halt.
 *          NOTE: return value is THIRTY-TWO(32) bits, DO NOT return all EBX!!
 * SIDE EFFECTS: none
 */
int32_t trap_halt(uint32_t esp){						//to do: update pid!!! don't forget
														//to do: update page table(slab cache)
	uint32_t flag;
	cli_and_save(flag);													
	int fd;
	pcb_t *curpcb;
	curpcb = (pcb_t*)(esp & DESCRIPTOR_MSK);
	
	// close any unclosed file
	for (fd = 2; fd < FILE_ARRAY_SIZE; fd++) {
		if (curpcb->file_array[fd].flags!=0)
			trap_close(curpcb, fd);
	}
	

	uint32_t thisPid = get_cur_pid();

	ufree_help(kernel_pid[thisPid].mem);

	release_pid(thisPid);
	release_page(kernel_pid[thisPid].cr3);
	release_page(kernel_pid[thisPid].pte);
	wake_up_pid(kernel_pid[thisPid].parent_pid);
	set_cur_pid(kernel_pid[thisPid].parent_pid);

	restore_flags(flag);

	//release_pid(curpcb->pid);

	//wake_up_pid(((pcb_t *)(curpcb->parent_pcb_pt))->pid);
	//set_cur_pid(((pcb_t *)(curpcb->parent_pcb_pt))->pid);
	//naive_kfree(FOUR_K);
	//naive_kfree(FOUR_K);

	return 0;
}


uint32_t trap_modeX(uint32_t data, uint32_t cmd)
{
	BMP256_t * bmp = (BMP256_t *)data;
	switch (cmd)
	{
	case 0 :
		
		//handle_bmp(bmp);
		handle_bmpK(bmp);

		break;
	case 1 :
		start_modex();
			break;
	case 2 :
		end_modex();
		break;

	case 3:
		handle_bmp(bmp);
		break;
	default:
		break;
	}
	
	return 0;


}



/* trap_execute
 * Description: attempt to load & execute a new program
 * INPUTS:	command - space-separated sequence of word
 *                note: 1st word: file name of program to execute
 *                      other: be provided to new program via GETARGS
 *          myesp - esp of the program which invokes int 0x80
 *          myeip - eip of the program which invokes int 0x80
 * OUTPUTS:	None
 * RETURN: -1 - failure, command cannot be executed
 *         256 - program dies by exception
 *         0~255 - program executes HALT (value given by the call to HALT)
 * SIDE EFFECTS: allocate necessary resource for new program
 */
 
 
/* trap_execute
 * Description: helper function to execute a program
 * INPUTS:	command - space-separated sequence of word
 *                note: 1st word: file name of program to execute
 *                      other: be provided to new program via GETARGS
 *          myesp - esp of the program which invokes int 0x80
 *          myeip - eip of the program which invokes int 0x80
 * OUTPUTS:	None
 * RETURN: -1 - failure, command cannot be executed
 *         mypcb - pcb address of the new program
 * SIDE EFFECTS: allocate necessary resource for new program
 */

/*******this is just a helper function of exe*******/
int32_t trap_execute(const uint8_t* command, uint32_t myesp, uint32_t myeip){
//myeip is no longer in use, the halt will be forced to jump to RET_EIP


	static uint32_t top_pbc = KERNEL_PCB - KERNEL_PCB_OFFSET;
	pcb_t* mypcb, *curpcb;
	curpcb = (pcb_t*)(myesp & DESCRIPTOR_MSK);
	uint32_t newCommend;

	uint8_t avg0[THIRTY_TWO];
	uint8_t avg1[THIRTY_TWO];
	uint8_t avg2[THIRTY_TWO];
	uint8_t avg3[THIRTY_TWO];

	uint32_t mypcbss = USER_DS;
	uint32_t mypcbcs = USER_CS;
	
	if (command == NULL){
		return -1;
	}
	
	if ((int)curpcb != KERNEL_PCB)
	{
		// if not kernel, finds the kernel address of arguments 
		uint32_t curCr3 = curpcb->cr3;
		newCommend = get_physical_add((uint32_t)command, curCr3);
		// then get arguments
		get_avg((uint8_t*)newCommend, avg0, avg1, avg2, avg3);
	
	}else
	{
		// else directly get arguments
		get_avg(command, avg0, avg1, avg2, avg3);
	}

	uint8_t su[ARRAY_SIZE_10] = "su";
	uint8_t pd[ARRAY_SIZE_10] = "pd";
	uint8_t kill[ARRAY_SIZE_10] = "kill";
	uint8_t xmode[ARRAY_SIZE_10] = "xmode";
	uint8_t qxmode[ARRAY_SIZE_10] = "qxmode";
	uint8_t ktest[ARRAY_SIZE_10] = "ktest";
	static uint8_t mypw[ARRAY_SIZE_10] = "group01";

	dentry_t dentry;
	uint32_t flag;

	

	if (!strncmp2((const int8_t *) avg0, (const int8_t *) su, ARRAY_SIZE_10))
	{
		//disable_irq(0);
		disable_sched();
		//set_cur_pid(3);
		
		
		printf("password: ");
		cli_and_save(flag);
		sti();
		//while(1);
		uint8_t pwbuf[PWBUF_SIZE_100];
		sc_pw_enable(1);
		stdin_read(0, pwbuf, PWBUF_SIZE_100, NULL);
		restore_flags(flag);
		sc_pw_enable(0);
		if (!strncmp2((const int8_t *) mypw, (const int8_t *) pwbuf,
				ARRAY_SIZE_10))
		//if (1)
		{
		
			printf("password correct\n\n");
			mypcbss = KERNEL_DS;
			mypcbcs = KERNEL_CS;
			strncpy((int8_t *) avg0, (const int8_t *) avg1, THIRTY_TWO);
			strncpy((int8_t *) avg1, (const int8_t *) avg2, THIRTY_TWO);
			strncpy((int8_t *) avg2, (const int8_t *) avg3, THIRTY_TWO);
			
			
		
		}else
		{
		
			printf("password incorrect %d\n\n",
					strncmp2((const int8_t *) mypw, (const int8_t *) pwbuf,
							ARRAY_SIZE_10));

			
			init_sched();
			return -1;
		}
		init_sched();
	
		
		

		
	} else if (!strncmp2((const int8_t *) avg0, (const int8_t *) pd,
			ARRAY_SIZE_10))
	{
	
		
		int i;
		for (i = 0; i < ARRAY_SIZE_10; i++)
		{
		
			printf("PID: %d    ", i);
			if (kernel_pid[i].present == 1 && kernel_pid[i].sleep == 1)
			{
			
				printf("sleep         ");
				if (i >2)
					printf("%s", kernel_pid[i].name);
				else
					printf("%s", "idle");

			
			}else if (kernel_pid[i].present == 1 && kernel_pid[i].sleep == 0)
			{
				
				printf("running       ");
				if (i >2)
					printf("%s", kernel_pid[i].name);
				else
					printf("%s", "idle");
			
			}else
			{
				
				printf("NOT PRESENT   ");
				
			}

			printf("\n");
		
		}
		return 0;
	
	
	} else if (!strncmp2((const int8_t *) avg0, (const int8_t *) kill,
			ARRAY_SIZE_10))
	{
	
	
		uint8_t num_0[3] = "0";
		uint8_t num_1[3] = "1";
		uint8_t num_2[3] = "2";
		uint8_t num_3[3] = "3";
		uint8_t num_4[3] = "4";
		uint8_t num_5[3] = "5";
		uint8_t num_6[3] = "6";
		uint8_t num_7[3] = "7";
		uint8_t num_8[3] = "8";
		uint8_t num_9[3] = "9";

		uint32_t pid_to_kill;

		if (!strncmp((const int8_t *)avg1, (const int8_t *)num_0, 1))
		{
			pid_to_kill = 0;
		
		}else if (!strncmp((const int8_t *)avg1, (const int8_t *)num_1, 1))
		{
			pid_to_kill = 1;
		
		}else if (!strncmp((const int8_t *)avg1, (const int8_t *)num_2, 1))
		{
			pid_to_kill = 2;
		
		}else if (!strncmp((const int8_t *)avg1, (const int8_t *)num_3, 1))
		{
			pid_to_kill = 3;
		
		}else if (!strncmp((const int8_t *)avg1, (const int8_t *)num_4, 1))
		{
			pid_to_kill = 4;
		
		}else if (!strncmp((const int8_t *)avg1, (const int8_t *)num_5, 1))
		{
			pid_to_kill = 5;
		
		}else if (!strncmp((const int8_t *)avg1, (const int8_t *)num_6, 1))
		{
			pid_to_kill = 6;
		
		}else if (!strncmp((const int8_t *)avg1, (const int8_t *)num_7, 1))
		{
			pid_to_kill = LARGEST_FILE_DISCRIPTOR_ID;
		
		}else if (!strncmp((const int8_t *)avg1, (const int8_t *)num_8, 1))
		{
			pid_to_kill = FILE_ARRAY_SIZE;
		
		}else if (!strncmp((const int8_t *)avg1, (const int8_t *)num_9, 1))
		{
			pid_to_kill = 9;
		
		}else 
		{
			printf("invalid input\n");
			return 0;
		
		}

		if (kernel_pid[pid_to_kill].present == 0)
		{
		
			printf("Process is not present\n");
		
		}else if (kernel_pid[pid_to_kill].sleep == 1)
		{
		
			printf("Process is not running\n");
		
		}else
		{
		
			pending_kill = 1;
			pending_kill_id = pid_to_kill;
			printf("Process %d is killed\n", pid_to_kill);
		
		}

		return 0;

		


		
	
	
	
	} else if (!strncmp2((const int8_t *) avg0, (const int8_t *) xmode,
			ARRAY_SIZE_10))
	{
	
		disable_sched();
		set_mode_X();
		update_statusBar(0,0,0);
		return 0;
		
	
	} else if (!strncmp2((const int8_t *) avg0, (const int8_t *) qxmode,
			ARRAY_SIZE_10))
	{
	
		clear_mode_X();
		init_sched();
		clear_buffer();
		clear();
		return 0;
	
	} else if (!strncmp2((const int8_t *) avg0, (const int8_t *) ktest,
			ARRAY_SIZE_10))
	{

		uint32_t ret = trap_malloc();
		printf("return value %x\n", ret);
		const uint8_t buf[ARRAY_SIZE_10] = "jojo.bmp";

		uint32_t fd = trap_open(curpcb, buf);
		printf ("fd is %d\n", fd);

		//num is not used, so Han commented it to avoid warning. 
		//uint32_t num = trap_read(curpcb, fd, (void *)ret, 1000000);

		
		
		//stdout_write(1, (void *)buf, 100, curpcb);

		


		return 0;
	
	}



	if (0)
	{
		
		
		return -1;

		
	}else
	{
			// allocate PCB 8kb above program kernel stack
			
			int thisPid = get_cur_pid();
			int pid = request_new_pid();
			if (pid == -1)
			{
				printf("pid assign error\n\n");
				return -1;
			
			}


			//top_pbc-=0x2000;
			mypcb = (pcb_t*)(top_pbc-ESP0_INTERVAL*pid);
			//top_pbc += 0x2000;
			init_pcb(mypcb);
			// find file
			if (read_dentry_by_name(avg0, &dentry)==0)
			{
				


				uint8_t buf[STRING_LENGTH];
				// get header && check elf
			    read_data(dentry.index_node, 0, buf, STRING_LENGTH);
				if (buf[0]==ELF_MAGIC_0 && buf[1]==ELF_MAGIC_1 && buf[2]==ELF_MAGIC_2 && buf[3]==ELF_MAGIC_3)
				{
		
					uint32_t file_location = *((uint32_t*) &buf[FILE_BUF_SIZE]);
					uint32_t saved_fl = file_location;
					//file_location = LOAD_ADD2;
					//file_location = LOAD_ADD + (PROGRAM_SIZE * program_num_count);
					//program_num_count++;
					file_location = LOAD_ADD + (PROGRAM_SIZE * (pid-2));
					
					uint32_t offset = 0;
					// load program image
				uint32_t retval = read_data(dentry.index_node, offset,
						(uint8_t*) file_location, OFFSET_TEN_THOUSAND);
					while (retval!=0)
					{
			
						offset += OFFSET_TEN_THOUSAND;
					file_location += OFFSET_TEN_THOUSAND;
					retval = read_data(dentry.index_node, offset,
							(uint8_t*) file_location, OFFSET_TEN_THOUSAND);
			
					}
					// save pcb
					strcpy((int8_t*)(mypcb->avg1), (const int8_t*)avg1);
					strcpy((int8_t*)(mypcb->avg2), (const int8_t*)avg2);
					strcpy((int8_t*)(mypcb->avg3), (const int8_t*)avg3);
					mypcb->eip = myeip;										//not in use
					mypcb->esp = SHELL_ESP;
					mypcb->espSaved = myesp;
					//mypcb->cr3 = (uint32_t)upde2;
					//PDE4_t * exeupde = (PDE4_t *)kmalloc(FOUR_K, 1);			//to do: the new malloc data is not initialized to 0. probably will improve this when implement real malloc, cache slab.
					//PDE4_t * exeupte = (PDE4_t *)kmalloc(FOUR_K, 1);

					PDE4_t * exeupde = (PDE4_t *)request_page();
					PDE4_t * exeupte = (PDE4_t *)request_page();
					
					//printf("upde is %x\n\n", exeupde);
					init_upde(exeupde, 0, 0, KERNEL, PAGE_4MB, NO_PTE, NO_PTE);
					init_upde(exeupde, 1, 1, KERNEL, PAGE_4MB, NO_PTE, NO_PTE);
				    init_upde(exeupde, THIRTY_TWO, pid, USER, PAGE_4MB,
						NO_PTE, NO_PTE);
				    init_upde(exeupde, THIRTY_THREE, 0, USER, PAGE_VID_MAP, exeupte,
				   		(uint32_t) exeupte);
					mypcb->cr3 = (int)exeupde;
					mypcb->pid = pid;


					int kernel_esp0 = SHELL_ESP0 - pid*ESP0_INTERVAL;

					

					mypcb->ss = mypcbss;
					mypcb->cs = mypcbcs;
					mypcb->pdir = exeupde;
					mypcb->parent_pcb_pt = myesp&DESCRIPTOR_MSK;
					mypcb->file_location = saved_fl;
					exe_helper_set_tss(mypcb->ss, kernel_esp0);
					std_install(mypcb->file_array);

					kernel_pid[pid].cr3 = (uint32_t)exeupde;
					kernel_pid[pid].pte = (uint32_t)exeupte;
					kernel_pid[pid].present = 1;
					kernel_pid[pid].ticks = 1;
					kernel_pid[pid].tss_esp0 = kernel_esp0;
					kernel_pid[pid].tss_ss0 = mypcb->ss;
					kernel_pid[pid].parent_pid = thisPid;
					kernel_pid[pid].terminal_num = kernel_pid[thisPid].terminal_num;
					sleep_cur();
					set_cur_pid(pid);
					kernel_pid[pid].name = (uint8_t *)&avg0;

					//printf("pid # %d, parent pid # %d\n", pid, thisPid);
					return (int32_t)mypcb;
			
		
				}else
				{
				// if not executable
					printf("file is NOT executable\n");
					release_pid(pid);
					return -1;
				
				}
	
			}else
			// if file not found
			{
				printf("file NOT found\n");
				release_pid(pid);
				return -1;
			}

	}

	// sanity check, return fail for all cases not considered
	return -1;
}

/* ufree_help
 * Description: trap for malloc
 * INPUTS:	none
 * OUTPUTS:	None
 * RETURN: address of assigned memory.
 * SIDE EFFECTS: none
 */
uint32_t trap_malloc()
{

	uint32_t pid = get_cur_pid();
	mmPDE4_t * mmpde = (mmPDE4_t *)r_kmalloc(sizeof (mmPDE4_t));
	mmpde->next = 0;
	mmpde->curPDE = umalloc_pde();
	if (mmpde->curPDE.Present == 0)
	{
	
		r_kfree((uint32_t)mmpde);
		return -1;

	
	}

	mmPDE4_t* pidmm = (mmPDE4_t *)(kernel_pid[pid].mem);
	if (pidmm == 0)
	{
		
		kernel_pid[pid].mem = (uint32_t)mmpde;

	}else{
	
		mmPDE4_t * next = (mmPDE4_t *)(pidmm->next);
		while (next!=0)
		{
		
			pidmm = next;
			next = (mmPDE4_t *)(pidmm->next);
		
		
		}

		pidmm->next = (uint32_t)mmpde;
	
	
	}
	

	PDE4_t * userPDE  = (PDE4_t *)kernel_pid[pid].cr3;
	int i;
	for (i = 3; i < USER_PDE_SIZE; i++)
	{
	
		if (userPDE[i].Present == 0)
		{
		
			userPDE[i] = mmpde->curPDE;
			return i * MEMO_SPACE;
		
		}
	
	
	}
	return -1;


}


/* ufree_help
 * Description: helper function for ufree
 * INPUTS:	uint32_t cur -- current 
 * OUTPUTS:	None
 * RETURN: none
 * SIDE EFFECTS: none
 */
void ufree_help(uint32_t cur)
{

	if (cur == 0)
		return;
	mmPDE4_t* curmm = (mmPDE4_t *) cur;
	mmPDE4_t * next = (mmPDE4_t *)(curmm->next);
	while (next != 0)
	{
	
		ufree_pde(curmm->curPDE);
		curmm = next;
		next =  (mmPDE4_t *)curmm->next;
	
	}


}


/* exe_helper_set_tss
 * Description: helper function for execute
 * INPUTS:	ss - not used
 *          esp - kernel stack pointer 
 * OUTPUTS:	None
 * RETURN: none
 * SIDE EFFECTS: writes to TSS
 */
void exe_helper_set_tss(uint32_t ss, uint32_t esp)
{
	tss.ss0 = KERNEL_DS;
	tss.esp0 = esp;
	tss.cs = KERNEL_CS;
	tss.ds = KERNEL_DS;


/*****test code*****/
	//tss.esp0 = 0x800000;
	/*
	tss.fs = KERNEL_DS;
	tss.es = KERNEL_CS;
	tss.gs = KERNEL_DS;
	*/
}


/* restore_tss
 * Description: restores the tss SS0 and ESP0
 * INPUTS:	curpcb - pointer to current pcb
 * OUTPUTS:	None
 * RETURN: oldPcb - the pointer to the pcb of the parent of current process
 * SIDE EFFECTS: writes to TSS
 */
pcb_t * restore_tss(pcb_t * curpcb)
{
	// get parent PCB
	pcb_t * oldPcb = (pcb_t *)(curpcb->parent_pcb_pt);
	/*if (oldPcb == NULL)
	{
	
		
		for (;;);
		return NULL;
	
	
	}*/
	
	// restore TSS
	tss.esp0 = curpcb->espSaved;
	tss.ss0 = curpcb->ssSaved;
	//tss.esp0 = oldPcb->espSaved;
	//tss.ss0 = oldPcb->ssSaved;
	return oldPcb;


}



/* get_avg
 * Description: get arguments from command
 * INPUTS:	command - pointer to the command to decode
 *          avg0...3 - buffers to write the arguments to
 * OUTPUTS:	None
 * RETURN: none
 * SIDE EFFECTS: none
 */
void get_avg(const uint8_t* command, uint8_t* avg0, uint8_t*avg1, uint8_t*avg2, uint8_t*avg3)
{
	// initialize all fields
	avg0[0] = avg1[0] = avg2[0] = avg3[0] = '\0';
	int i = 0;
	int j = i;
	uint8_t next_char = command[j];
	i = 0;
	
	// get file name (read until space or EOS)
	while (next_char!=' ' && next_char!='\0' && i<AVGMAX)
	{
		avg0[i] = next_char;
		j++;
		next_char = command[j];
		i++;
		
	}
	avg0[i] = '\0';

	// skip space
	if (next_char == ' ')
	{
	
		//i++;
		j++;
		next_char = command[j];
	
	}


	// get arg1 (read until space or EOS)
	i = 0;
	while (next_char!=' ' && next_char!='\0' && i<AVGMAX)
	{
		avg1[i] = next_char;
		j++;
		next_char = command[j];
		i++;
		
	}
	avg1[i] = '\0';

	// skip space
	if (next_char == ' ')
	{
	
		//i++;
		j++;
		next_char = command[j];
	
	}

	// get arg2 (read until space or EOS)
	i = 0;
	while (next_char!=' ' && next_char!='\0' && i<AVGMAX)
	{
		avg2[i] = next_char;
		j++;
		next_char = command[j];
		i++;
		
	}
	avg2[i] = '\0';

	// skip space
	if (next_char == ' ')
	{
	
		//i++;
		j++;
		next_char = command[j];
	
	}

	// get arg3 (read until space or EOS)
	i = 0;
	while (next_char!=' ' && next_char!='\0' && i<AVGMAX)
	{
		avg3[i] = next_char;
		j++;
		next_char = command[j];
		i++;
		
	}
	avg3[i] = '\0';

	

}



/* trap_read
 * Description: reads data from directory/file/keyboard/RTC
 *
 * INPUTS:	None
 * OUTPUTS:	None
 * RETURN: -1 - failure, non-existent file or invalid index
 *          0 - end of file is reached (for normal files & directory only)
 *         other - number of bytes read
 * SPECIAL CASES: RTC - always return 0 (only after interrupt)
 *                FILE - read until end of file, or until buffer is full
 *                DIRECTORY - ONLY filename should be provided
 *                KEYBOARD - return data from an ENTER terminated line
 */
int32_t trap_read(pcb_t* mypcb, int32_t fd, void* buf, int32_t nbytes){
	// if file descriptor outside range
	if ((fd < 0) || (fd > LARGEST_FILE_DISCRIPTOR_ID)) {
		return -1;
	}
	// if file descriptor is not in use (invalid)
	if (mypcb->file_array[fd].flags == 0){
		return -1;
	}
	// if buffer is not valid
	if (buf == NULL){
		return -1;
	}

	return mypcb->file_array[fd].fops_pt->read(fd, buf, nbytes, (uint32_t*)mypcb);
}



/* trap_write
 * Description: writes data to terminal or to device
 * INPUTS:	None
 * OUTPUTS:	None
 * RETURN: -1 - failure, invalid input or read-only file system
 *          0 - success
 * SPECIAL CASES: TERMINAL - all data should display on screen immediately
 *                RTC - set the RTC interrupt rate
 *                FILE SYSTEM - read-only, always return -1
 */
int32_t trap_write(pcb_t* mypcb, int32_t fd, const void* buf, int32_t nbytes){
	// if file descriptor outside range
	if ((fd < 0) || (fd > LARGEST_FILE_DISCRIPTOR_ID)) {
		return -1;
	}
	// if file descriptor is not in use (invalid)
	if (mypcb->file_array[fd].flags == 0){
		return -1;
	}
	// if buffer is not valid
	if (buf == NULL){
		return -1;
	}

	return mypcb->file_array[fd].fops_pt->write(fd, buf, nbytes, (uint32_t*)mypcb);
}



/* trap_open
 * Description: provide access to the file system
 * INPUTS:	None
 * OUTPUTS:	-1 - fail, file name does not exist / no free descriptor
 *           
 *			 fd - success
 * RETURN: -1 - failure, non-existent file or invalid index
 * SIDE EFFECTS: find the directory entry, allocate unused file descriptor,
 *               set up necessary data for the file type
 */
int32_t trap_open(pcb_t* mypcb, const uint8_t* buf){
	dentry_t dentry;
	int error;
	int i;



	// if buffer is not valid
	if (buf == NULL){
		return -1;
	}
	
	
	// find open file descriptor
	// file_array should only contain 8 entries(0~7)
	for (i = 2; i<=FILE_ARRAY_SIZE;i++){
		if (i == FILE_ARRAY_SIZE)
			return -1;
		if (mypcb->file_array[i].flags == 0){
			break;
		}
	}
	
	// get file type
	error = read_dentry_by_name(buf, &dentry);
	if (error != 0)
		return -1;

	switch (dentry.file_type){
		case TYPE_RTC:
			mypcb->file_array[i].fops_pt = &RTC_FOPS;
			// reset position, mark in-use, and record inode
			mypcb->file_array[i].file_position = 0;
			mypcb->file_array[i].flags = 1;
			mypcb->file_array[i].inode_pt = (inode_t *)dentry.index_node;			//this is not a point but the index number
			break;
		case TYPE_DIR:
			mypcb->file_array[i].fops_pt = &DIR_FOPS;
			// reset position, mark in-use, and record inode
			mypcb->file_array[i].file_position = 0;
			mypcb->file_array[i].flags = 1;
			mypcb->file_array[i].inode_pt = (inode_t *)dentry.index_node;			//this is not a point but the index number
			break;
		case TYPE_FILE:
			mypcb->file_array[i].fops_pt = &FILE_FOPS;
			// reset position, mark in-use, and record inode
			mypcb->file_array[i].file_position = 0;
			mypcb->file_array[i].flags = 1;
			mypcb->file_array[i].inode_pt = (inode_t *)dentry.index_node;			//this is not a point but the index number
			break;
		default: 
			printf ("i am here\n");
			break;
	}

	if (mypcb->file_array[i].fops_pt->open(buf, (uint32_t*)mypcb) == 0)
		return i;
	else
		return -1;
}



/* trap_close
 * Description: close file descriptor & make it available for OPEN
 *          NOTE - Default descriptors (0 for input & 1 for output)
 *                 CANNOT be closed
 * INPUTS:	mypcb - pointer to pcb
 *          fd - file descriptor number of the file to close
 * OUTPUTS:	-1 - fail, invalid descriptor
 *           0 - success
 * RETURN: -1 - failure, non-existent file or invalid index
 * SIDE EFFECTS: pass the file name, file type, inode number to input "dentry"
 */
int32_t trap_close(pcb_t* mypcb, int32_t fd)
{
	// check for invalid inputs
	if (fd == 1 || fd == 0)
		return -1;
	if (fd < 0 || fd > LARGEST_FILE_DISCRIPTOR_ID)
		return -1;
	// check for in-use status
	if (mypcb->file_array[fd].flags == 0)
		return -1;
		
	// close file (reset in-use status)
	else
	{
		mypcb->file_array[fd].flags = 0;
	}

	return 0;
	
}



/* trap_getargs
 * Description: reads command line arguments into user-level buffer
 * INPUTS:	mypcb - pointer to pcb
 *          buf - pointer to user level buffer
 *          nbytes - number of bytes to read
 * OUTPUTS:	None
 * RETURN: -1 - fail, arguments/terminal NULL do not fit in buffer
 * SIDE EFFECTS: None
 */
int32_t trap_getargs(pcb_t* mypcb, int8_t *buf, int32_t nbytes){
	uint32_t n = nbytes;
	// if buffer is not valid
	if (buf == NULL){
		return -1;
	}
	// check for length (should be <= filename max length)
	if (n > AVGMAX)
		n = AVGMAX;
	
	// if argument is too long
	if (strlen((int8_t *)mypcb->avg1)+1 > nbytes)
		return -1;
	
	// copy arguments to buffer
	strncpy(buf, (int8_t *)mypcb->avg1, n);

	return 0;
}



/* trap_vidmap
 * Description: maps text-mode video memory into user space at pre-set virtual address
 * INPUTS:	None
 * OUTPUTS:	None
 * RETURN: -1 - failure, invalid address (falls within the user-level page?)
 * SIDE EFFECTS: written into the memory location provided by the caller
 */
int32_t trap_vidmap(uint32_t myesp, uint8_t** screen_start){
	uint8_t** kernel_addr;
	uint32_t cr3;
	pcb_t* curpcb;
	
	
	if ((int32_t) screen_start < VIDEO_START_ADDR
			|| (int32_t) screen_start > VIDEO_END_ADDR)
		return -1;
		
	// find kernel address of screen_start
	curpcb = (pcb_t*) (myesp & ESP_MASK);
	cr3 = curpcb->cr3;
	/*
	PDE4_t * exeupte = (PDE4_t *)request_page();
	PDE4_t * exeupde = curpcb -> pdir;
	init_upde(exeupde, 33, 0, USER, PAGE_VID_MAP, exeupte, (uint32_t)exeupte);
	*/
	kernel_addr = (uint8_t**)get_physical_add((uint32_t)screen_start, cr3);
	
	
	// pass back the start of virtual address for video memory in user space
	*kernel_addr = (uint8_t*) VIDEO_END_ADDR;
	
	return 0;
}



// extra credit
/* trap_set_handler
 * Description: trap set handler
 * INPUTS:	int32_t signum -- an integer number
            void* handler_address -- address of handler
 * OUTPUTS:	None
 * RETURN: 0 success, -1 fail. 
 * SIDE EFFECTS: none
 */
int32_t trap_set_handler(pcb_t* mypcb, int32_t signum, void* handler_address){
	uint32_t flags;
	// error check allows exception handler to be changed
	if (signum>4 || signum < 0)
		return -1;
	if ((int32_t) handler_address < VIDEO_START_ADDR
			|| (int32_t) handler_address > VIDEO_END_ADDR)
		return -1;
	
	cli_and_save(flags);
	if (handler_address == NULL)
		mypcb->sig_act[signum] = default_sig_act[signum];
	else
		mypcb->sig_act[signum] = handler_address;
	
	restore_flags(flags);
	return 0;
}

// extra credit
/* trap_sigreturn
 * Description: Sign return 
 * INPUTS:	None
 * OUTPUTS:	None
 * RETURN: 0 success, -1 fail. 
 * SIDE EFFECTS: none
 */
int32_t trap_sigreturn(sys_reg r){
	return restore_sigcontext(&r);
}


/*
 * fops_init
 * Description: initialize file opening operations. 
 * INPUTS: None
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: None
 */
void fops_init(){
	STDIN_FOPS.open = (fops_open)stdin_open;
	STDIN_FOPS.close = (fops_close)stdin_close;
	STDIN_FOPS.read = (fops_read)stdin_read;
	STDIN_FOPS.write = (fops_write)stdin_write;
	
	STDOUT_FOPS.open = (fops_open)stdout_open;
	STDOUT_FOPS.close = (fops_close)stdout_close;
	STDOUT_FOPS.read = (fops_read)stdout_read;
	STDOUT_FOPS.write = (fops_write)stdout_write;
	
	RTC_FOPS.open = (fops_open)sys_rtc_open;
	RTC_FOPS.close = (fops_close)sys_rtc_close;
	RTC_FOPS.read = (fops_read)sys_rtc_read;
	RTC_FOPS.write = (fops_write)sys_rtc_write;
	
	FILE_FOPS.open = (fops_open)filesys_open;
	FILE_FOPS.close = (fops_close)filesys_close;
	FILE_FOPS.read = (fops_read)filesys_read;
	FILE_FOPS.write = (fops_write)filesys_write;
	
	DIR_FOPS.open = (fops_open)dir_open;
	DIR_FOPS.close = (fops_close)dir_close;
	DIR_FOPS.read = (fops_read)dir_read;
	DIR_FOPS.write = (fops_write)dir_write;
	
}

/*
 * std_install
 * Description: standard installation
 * INPUTS:	
		 fd_t* file_array -- file array
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: None
 */
extern void std_install(fd_t* file_array)
{
	file_array[0].fops_pt = &STDIN_FOPS;
	file_array[0].flags = 1;
	file_array[1].fops_pt = &STDOUT_FOPS;
	file_array[1].flags = 1;
	
}


