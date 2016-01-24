/* This ECE391OS project is an in class project written by
*Hongchuan Li, Meng Gao, Junqing Deng, Han Jiang in UIUC (spring 2014)
*The current file is wriiten by Hongchuan Li
*Students who are taking ECE391 should AVOID copying this file
*/

#ifndef _PCB_H
#define _PCB_H

#include "types.h"
#include "filesys.h"
#include "x86_desc.h"
#include "pid.h"
#define FILE_ARRAY_SIZE   8
#define ARGU_ARRAY_SIZE   32
#define SIGNAL_ARRAY_SIZE   5
#define PID_ARRAY_SIZE   11
// file operation jump table
/*
typedef	int32_t (*fops_open)(const uint8_t*, pcb_t*);
typedef	int32_t (*fops_close)(int32_t, pcb_t*);
typedef	int32_t (*fops_read)(int32_t, uint32_t*, int32_t, pcb_t*);
typedef	int32_t (*fops_write)(int32_t, const uint32_t*, int32_t, pcb_t*);
*/

typedef	int32_t (*fops_open)(const uint8_t*, uint32_t*);
typedef	int32_t (*fops_close)(int32_t, uint32_t*);
typedef	int32_t (*fops_read)(int32_t, uint32_t*, int32_t, uint32_t*);
typedef	int32_t (*fops_write)(int32_t, const uint32_t*, int32_t, uint32_t*);

typedef struct file_ops{
	fops_open open;
	fops_close close;
	fops_read read;
	fops_write write;
} fops_t;




// file descriptor table
typedef struct file_descriptor{
	fops_t* fops_pt;
	//int32_t * fop_table[4];	// file operation jump table
	
	inode_t* inode_pt;		// actually used for the inode number, NOT a pointer
	uint32_t file_position; // where the user is currently reading.
	                        // Every READ should update file_position
	uint32_t flags;
} fd_t;

// process control block, stores per-task data structure
// Process identification data, Processor state data, Process control data
typedef struct process_ctl_blk{
	
	/************************/
	/*
	 * some of the following are directly referenced by using offset
	 * review the code carefully, otherwise do not change.
	 */
	uint32_t eip;			//0 not used
	uint32_t eflags;		//4 not used, system call doesn't guarantee the flag doesn't change
	uint32_t esp;			//8 virtual address of the user stack pointer
	uint32_t ss;			//12 SS of user
	uint32_t cs;			//16 CS of user
	uint32_t parent_pcb_pt;	//20 pointer to parent pcb (Note: already have espSaved, this may be removed if no one uses it)
	uint32_t file_location;	//24 location to load the program
	uint32_t cr3;			//28 pointer to program page directory (to load into cr3)
	uint32_t espSaved;		//32 parent's kernel stack esp (to restore TSS)
	uint32_t ssSaved;		//36 parent's ss (to restore TSS)
	uint32_t eflagSaved;	//40 not used (should be safe to remove)
	uint32_t eipSaved;		//44 not used (should be safe to remove)
	uint32_t csSaved;		//48 not used (should be safe to remove)
	uint32_t cr3Saved;		//52 not used (should be safe to remove)
	uint32_t pid;			//54 PID of the user program
	/************************/

	//uint32_t signal_info;				// not used
	fd_t file_array[FILE_ARRAY_SIZE];	// file descriptor array for the user program
	
	uint8_t avg1[ARGU_ARRAY_SIZE];		// saved copy of pre-processed cmd arguments
	uint8_t avg2[ARGU_ARRAY_SIZE];
	uint8_t avg3[ARGU_ARRAY_SIZE];
	
	uint8_t sig_map;		// signal bit map, low five bits indicates pending signal, 1=pending
	uint8_t sig_block;		// signal blocking bit map, low five indicates signal is blocked, 1=blocked
	
	sig_handler sig_act[SIGNAL_ARRAY_SIZE];	// signal handlers associated with the particular program
	
	PDE4_t * pdir;	// pointer to the page directory of the program.
					// note: this is the same as pcb.cr3...
					// vidmap no long uses this , but leaving in pcb just in case

	
	
	
} pcb_t;

extern void fops_init();
extern PID kernel_pid[PID_ARRAY_SIZE];


#endif

