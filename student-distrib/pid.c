/* This ECE391OS project is an in class project written by
*Hongchuan Li, Meng Gao, Junqing Deng, Han Jiang in UIUC (spring 2014)
*The current file is wriiten by Hongchuan Li
*Students who are taking ECE391 should AVOID copying this file
*/

#include "types.h"
#include "pid.h"
#include "x86_desc.h"


#include "pcb.h"

#define ESP_START 0x7fdffc
#define ESP0_INTERVAL 0x2000

#define DEBUG 0



static int kernel_stack_aval = 0;


/*
 * initial_all_pid
 * Description: initialize all of the pids. 
 * INPUTS: void * handler -- the handler function pointer. 
 * OUTPUTS:	None. 
 * RETURN: None
 * SIDE EFFECTS: None
 */
extern void initial_all_pid(void * handler)
{
	int i = 0;
	for (i=0; i< PID_NUM;i++)
	{
	
		kernel_pid[i].present = 0;
		kernel_pid[i].cr3 = 0;
		kernel_pid[i].ebp = 0;
		kernel_pid[i].eip = 0;
		kernel_pid[i].esp = 0;
		kernel_pid[i].tss_esp0 = 0;
		kernel_pid[i].tss_ss0 = 0;
		kernel_pid[i].ticks = 1;
		kernel_pid[i].sleep = 0;
	
	}
	//kernel_stack_aval = 0;

	kernel_pid[0].present = 1;
	kernel_pid[1].present = 1;
	kernel_pid[2].present = 1;

	kernel_pid[0].eip = (uint32_t)handler;
	kernel_pid[1].eip = (uint32_t)handler;
	kernel_pid[2].eip = (uint32_t)handler;

	kernel_pid[0].esp = ESP_START - 0*ESP0_INTERVAL;
	kernel_pid[0].ebp = ESP_START - 0*ESP0_INTERVAL;
	kernel_pid[0].terminal_num = 0;
	kernel_pid[0].tss_esp0 = ESP_START - 0*ESP0_INTERVAL;
	kernel_pid[1].esp = ESP_START - 1*ESP0_INTERVAL;
	kernel_pid[1].ebp = ESP_START - 1*ESP0_INTERVAL;
	kernel_pid[1].terminal_num = 1;
	kernel_pid[1].tss_esp0 = ESP_START - 1*ESP0_INTERVAL;
	kernel_pid[2].esp = ESP_START - 2*ESP0_INTERVAL;
	kernel_pid[2].ebp = ESP_START - 2*ESP0_INTERVAL;
	kernel_pid[2].terminal_num = 2;
	kernel_pid[2].tss_esp0 = ESP_START - 2*ESP0_INTERVAL;
	


	

	kernel_pid[0].cr3 = (uint32_t)kpde;
	kernel_pid[1].cr3 = (uint32_t)kpde;
	kernel_pid[2].cr3 = (uint32_t)kpde;
	
	cur_pid = 8;
	//cur_terminal = 0;





}


/* get_pid_map
 * Description: passes current bit map of bit
 * INPUTS:	none
 * OUTPUTS:	None
 * RETURN: kernel_stack_aval
 * SIDE EFFECTS: none
 */
int get_pid_map(){
	return kernel_stack_aval;
}

/* request_new_pid
 * Description: Create and Return a New Process ID. 
 * INPUTS:	none
 * OUTPUTS:	None
 * RETURN: New Process ID. 
 * SIDE EFFECTS: none
 */
int request_new_pid()
{
	int pid;
	for (pid = 0; pid < PID_NUM-1; pid ++)
	{
	
		if (kernel_pid[pid].present == 0)
		{
		
			kernel_pid[pid].present = 1;
			return pid;
		
		}
	
	}
	return -1;
	

}

/* release_pid
 * Description: Release the ID number after exiting the process. 
 * INPUTS:	int pid_num -- the process id
 * OUTPUTS:	None
 * RETURN: 0 -- success
           -1 -- failure
 * SIDE EFFECTS: none
 */




int release_pid(int pid_num)
{
	if (kernel_pid[pid_num].present == 0)
		return -1;
	kernel_pid[pid_num].present = 0;
	return 0;

}


/*
 * get_cur_pid
 * Description: get current pid. 
 * INPUTS: none;
 * OUTPUTS:	None. 
 * RETURN: current pid
 * SIDE EFFECTS: None
 */
uint32_t get_cur_pid()
{

	return cur_pid;

}


/*
 * initial_all_pid
 * Description: initialize all of the pids. 
 * INPUTS: void * handler -- the handler function pointer. 
 * OUTPUTS:	None. 
 * RETURN: Next PID Number
           -1 if fail;
 * SIDE EFFECTS: None
 */
uint32_t get_next_pid()
{
	int i;
	for (i=1; i <= PID_NUM; i++)
	{
	
		uint32_t pid_num = (cur_pid+i)%PID_NUM;
		if (kernel_pid[pid_num].present == 1 && kernel_pid[pid_num].sleep == 0)
			return pid_num;
	
	
	}
	return -1;
	
}

/*
 * set_cur_pid
 * Description: set current pid to the value passed in. 
 * INPUTS: uint32_t pid -- pid number.
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: None
 */
void set_cur_pid(uint32_t pid)
{

	cur_pid = pid;

}

/*
 * sleep_cur
 * Description: set the current pid to sleep. 
 * INPUTS: None
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: None
 */
void sleep_cur()
{
	if (DEBUG)printf("%d sleep\n", cur_pid);
	kernel_pid[cur_pid].sleep = 1;

}

/*
 * wake_up_pid
 * Description:  wake up a process according to the pid passed in. 
 * INPUTS: uint32_t pid_num  -- the pid number
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: None
 */
uint32_t wake_up_pid(uint32_t pid_num)
{

	if (kernel_pid[pid_num].present == 0)
		return -1;
	if (kernel_pid[pid_num].sleep == 0)
		return -1;

	if (DEBUG)printf("%d wake\n", pid_num);
	kernel_pid[pid_num].sleep = 0;
		return 0;

}

