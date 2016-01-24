/* This ECE391OS project is an in class project written by
*Hongchuan Li, Meng Gao, Junqing Deng, Han Jiang in UIUC (spring 2014)
*The current file is wriiten by Hongchuan Li
*Students who are taking ECE391 should AVOID copying this file
*/

#include "paging.h"
#include "lib.h"
#include "x86_desc.h"
#include "gen_asm.h"

/* paging_init
 * Description: fills in the page table and enable paging on hardware
 * INPUTS:	None
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: writes into PD & PT
 */
extern void paging_init()
{
	init_kpde();
	init_kpte();
	enable_paging();

}

/* init_kpde
 * Description: function that paging_init uses to fill table entries 
 * INPUTS:	None
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: writes PDE; set entry 0 & 1 in PD
 */
extern void init_kpde()
{
    // set entry 0
	int ikpte = (int)&kpte;
	kpde[0].Present = PRESENT;
	kpde[0].rw = WRITE;
	kpde[0].us = USER;				//maybe I should use KERNEL instead of USER
	kpde[0].base = ikpte >> BIT_LENGTH_12;
	// kpde[0].base = 0;
	// kpde[0].ps = PAGE_4MB;
	
	// set entry 1
	kpde[1].Present = PRESENT;
	kpde[1].rw = WRITE;
	kpde[1].ps = PAGE_4MB;
	kpde[1].avail = KERNEL;
	kpde[1].base = M_4MB;
	
	// others defaults to 0 (ie. Present = 0)
}


/*
 * init_kpde_4MB
 * DESCRITPION: initialise a kernel page directory entry which maps to a 4MB page. 
 * INPUT: 
 * uint32_t index: the directory entry index that is going to be changed. 
 * uint32_t base: the page's physical position, in the unit of 4MB.  
 * OUTPUT: none
 * RETURN: none
 * SIDE EFFECT: none. 
 */
extern void init_kpde_4MB(uint32_t index, uint32_t base)
{

	kpde[index].Present = PRESENT;
	kpde[index].rw = WRITE;
	kpde[index].ps = PAGE_4MB;
	kpde[index].avail = KERNEL;
	kpde[index].base = M_4MB*base;


}

/*
 * init_upde
 * DESCRITPION: initialise a user level page directory entry which maps to a 4MB page. 
 * INPUT: 
 * PDE4_t * upde: the starting address of the user page directory. 
 * uint32_t index: the index of the user directory entry that is going to be initialised. 
 * uint32_t base: the page's physical position, in the unit of 4MB. 
 * uint32_t us: when 0, indicate only kernel can access
 *	            when 1, indicate user priv 
 * uint32_t size: when = 1, indicate 4mb
 *	              when = 0, indicate 4kb
 * OUTPUT: none
 * RETURN: none
 * SIDE EFFECT: none. 
 */
extern void init_upde(PDE4_t * pde, uint32_t index, uint32_t base, uint32_t us, uint32_t size, PDE4_t * pte, uint32_t pte_a)
{
	if (size == PAGE_4MB)
	{
		// sanity check, in case of illegal input
		if (us !=0 && us != 1)
			return;
	
	
		pde[index].Present = PRESENT;
		pde[index].rw = WRITE;
		pde[index].us = us;
		//upde[index].avail = USERA;
		pde[index].ps = PAGE_4MB;
		pde[index].base = M_4MB*base;
	
	
	}
	else if (size == PAGE_4KB){
	}
	else{
		if (us !=0 && us != 1)
		return;
		
		pde[index].Present = PRESENT;
		pde[index].rw = WRITE;
		pde[index].us = us;
		pde[index].ps = PAGE_4KB;
		pde[index].base = pte_a >> BIT_LENGTH_12;
		
		// map video memory to 0x8400000
		pte[0].rw = WRITE;
		pte[0].us = USER;
		pte[0].Present = PRESENT;
		//pte[0].base = 0xB8000 >> BIT_LENGTH_12;		
		
		if(cur_terminal == 0){
			pte[0].base = VIDEO_T0 >> BIT_LENGTH_12;
		}else if(cur_terminal == 1){
			pte[0].base = VIDEO_T1 >> BIT_LENGTH_12;
		}else if(cur_terminal == 2){
			pte[0].base = VIDEO_T2 >> BIT_LENGTH_12;
		}
		
		pte[1].rw = READ;
		pte[1].us = USER;
		pte[1].Present = PRESENT;
		pte[1].base = (uint32_t)sig_kill_helper >> BIT_LENGTH_12;
		
		pte[2].rw = READ;
		pte[2].us = USER;
		pte[2].Present = PRESENT;
		pte[2].base = (uint32_t)sig_ignore_helper >> BIT_LENGTH_12;

	
		//printf("upde page size = 4kb, not done yet");
		//for(;;);
	
	
	}


}

/* init_kpte
 * Description: Initializes the Kernel Page Directory Entry.  
 * INPUTS:	None
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: Writes to Page Entries
 */
extern void init_kpte()
{
	// fills Page Entries
	int i;
	for(i = 0; i < PAGE_SIZE; i++)
	{
	
		kpte[i].rw = WRITE;
		kpte[i].us = USER;
		kpte[i].Present = PRESENT;
		kpte[i].avail = USERA;
		kpte[i].base = i;
	}
	
	// Set first to non-present
	kpte[0].Present = NPRESENT;

}
/*
 * get_physical_add
 * Description: get the physical address by providing linear/virtual address and cr3
 * INPUTS:	const uint32_t address: the linear/virtual address. 
            uint32_t cr3: the value of cr3 register, or PDBR, which points to the beginning of the 
			              page directory. 
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: NONE;
 */
extern uint32_t get_physical_add(const uint32_t address, uint32_t cr3)
{

	PDE4_t* mypde = (PDE4_t*)cr3;
	uint32_t pde_ind = address >> BIT_LENGTH_22; // get the 10-bit base of CR3 physical address. 
	                                             // which will be the PDE index. 
	// uint32_t pte_ind = (address >> 12) & 0x3ff;

	if (mypde[pde_ind].Present == NPRESENT)
		return 0;
	if (mypde[pde_ind].ps == PAGE_4MB)
	{
	    //form a physical address by connecting base address in PDE and lower 22 bits in linear address. 
		return (mypde[pde_ind].base << BIT_LENGTH_12) | (address & LOWER_22_BITS_MASK);
	
	}else	// else it is a 4kb page
	{
		printf("get_physical_add 4kb is not implemented");
		while(1){}
	}

	return 0;
}


