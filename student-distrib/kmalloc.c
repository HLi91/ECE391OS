/* This ECE391OS project is an in class project written by
*Hongchuan Li, Meng Gao, Junqing Deng, Han Jiang in UIUC (spring 2014)
*The current file is wriiten by Hongchuan Li
*Students who are taking ECE391 should AVOID copying this file
*/

#include "types.h"
#include "x86_desc.h"
#include "lib.h"
#include "kmalloc.h"
#define BITMASK_31_8 0xFFFFF000
#define BITMASK_8 0x0FFF
#define SIXTEEN_HEX 0x1000
#define FOUR_K 4096
#define M_4MB 0x400
#define NUM_CHARS_ON_STATUS_BAR 20 
static const int SIZE_8 = 8;
static const int SIZE_FOUR = 4;
static uint32_t KmalInd = 0;
/*	
    kmalloc
	description: dynamically allocate space
	input: 
	uint32_t size -- the size of space. size in KB
	uint32_t align -- the alignment  *	0 align to whatever available
                                     *	1 align to 4096
	output: none
	side effect: none;
    

*/
extern int32_t kmalloc(uint32_t size, uint32_t align)
{
	
	uint32_t address = KmalInd;
	uint32_t temp_address;
	switch (align)
	{
	
	case 0 :

		
		break;
	case 1 :

		temp_address = address;
		temp_address &= BITMASK_8;
		if (temp_address != 0)
		{
			//printf("temp_addr is %d \n", temp_address);
			address &= BITMASK_31_8;
			address += SIXTEEN_HEX;
		}
		break;
	default:
		printf("kmalloc input para invalid\n");
		return -1;

	}


	uint32_t start_ind = address;
	address +=(size);
	KmalInd = address;
	//printf("alloc, kmalind is %d\n",KmalInd);
	if (KmalInd >= KMALC)
	{
	
		printf("kmalloc out of boundry\n\n");
		for(;;);
	
	}

	return (int32_t)(&Kmal[start_ind]);


}

/*	
    naive_kfree
	description: dynamically free space, this function must be followed immediately after kmollac
	input: 
	uint32_t size -- the size of space. size in KB
	uint32_t align -- the alignment  *	0 align to whatever available
                                     *	1 align to 4096
	output: none
	side effect: none;
    

*/
extern int32_t naive_kfree(uint32_t size)
{

	KmalInd-=(size);
	//printf("freed, kmalind is %d\n",KmalInd);
	return KmalInd;



}
static uint8_t status[NUM_CHARS_ON_STATUS_BAR];
static uint32_t page_start;


/*	
    initial_paging_sc
	description: initialise paging. 
	input: none
	return: 0 success, -1 failure
	output: none
	side effect: none;
    

*/
extern uint32_t initial_paging_sc()
{

	
	page_start = kmalloc(FOUR_K*NUM_CHARS_ON_STATUS_BAR,1);
	
	int i;
	for (i=0; i< NUM_CHARS_ON_STATUS_BAR; i++)
	{
	
		status[i] = 0;
	
	}

	return 0;

}

/* request_page
 * Description: request page
 * INPUTS:	None
 * OUTPUTS:	None
 * RETURN:  0 success, -1 failure
 * SIDE EFFECTS: none
 */
extern uint32_t request_page()
{

	int i;
	for (i=0; i< NUM_CHARS_ON_STATUS_BAR; i++)
	{
	
		if (status[i] == 0)
		{
			status[i] = 1;
			memset((void *)(page_start + FOUR_K*i), 0, FOUR_K);
			return page_start + FOUR_K*i;
		
		
		}
	
	}
	test_interrupts();
	while(1);

}
/* release_page
 * Description: release page
 * INPUTS:	None
 * OUTPUTS:	None
 * RETURN:  0 success, -1 failure
 * SIDE EFFECTS: none
 */
extern void release_page(uint32_t address)
{

	uint32_t ind = (address-page_start)/FOUR_K;
	if (ind < 0 || ind >=NUM_CHARS_ON_STATUS_BAR)
	{
		test_interrupts();
		while(1);
	}
	status[ind] = 0;

}

/* r_kmalloc
 * Description: malloc space
 * INPUTS:	None
 * OUTPUTS:	None
 * RETURN:  address if success, -1 failure
 * SIDE EFFECTS: none
 */
extern uint32_t r_kmalloc(uint32_t size)
{
	if (size > FOUR_K)
	{

		return -1;
	}
	int i;
	i = size;
	if (i < SIZE_FOUR)
	{
		i = SIZE_FOUR;
	}


	for ( ; i < KMALCR - size; i = i+size)
	{
		int j = i - SIZE_FOUR;
		int byte = j / SIZE_8;
		int bit = j % SIZE_8;
		if (((KmalRI[byte] >> bit ) &0x1) == 0)
		{
		
			if (check_usable(j, size + SIZE_FOUR))
			{
			
				set_memory_busy(j, size + SIZE_FOUR);
				*((uint32_t *)(&KmalR[j])) = size;
				return (uint32_t)(&KmalR[i]);
			
			
			}

		}
	
	}
	return -1;


}

/* r_kfree
 * Description: free space
 * INPUTS:	uint32_t addr -- address to free
 * OUTPUTS:	None
 * RETURN:  none
 * SIDE EFFECTS: none
 */
extern void r_kfree(uint32_t addr)
{

	uint32_t pos = addr - (uint32_t)&KmalR[0];
	pos -= SIZE_FOUR;
	int i;
	uint32_t size = *((uint32_t *)(&KmalR[pos]));
	for (i = pos; i < pos + size + SIZE_FOUR; i++)
	{
		int byte = i / SIZE_8;
		int bit = i % SIZE_8;
		KmalRI[byte] ^= 0x1 << bit;
		
	
	
	}


}



/* check_usable
 * Description: check if the position is usable
 * INPUTS:	uint32_t pos  -- position address
            uint32_t size -- size 
 * OUTPUTS:	None
 * RETURN:  0 if not usable, 1 if usable
 * SIDE EFFECTS: none
 */

uint32_t check_usable(uint32_t pos, uint32_t size)
{

	int i;
	for (i = pos; i < pos+size; i++)
	{
		int byte = i / SIZE_8;
		int bit = i % SIZE_8;
		if (((KmalRI[byte] >> bit ) &0x1) == 1)
		{
		
			return 0;
		
		}
		
	
	
	}
	return 1;



}

/* set_memory_busy
 * Description: set the position as used.
 * INPUTS:	uint32_t pos  -- position address
            uint32_t size -- size 
 * OUTPUTS:	None
 * RETURN:  0 if not usable, 1 if usable
 * SIDE EFFECTS: none
 */
uint32_t set_memory_busy(uint32_t pos, uint32_t size)
{
	
	int i;
	for (i = pos; i < pos+size; i++)
	{
		int byte = i / SIZE_8;
		int bit = i % SIZE_8;
		KmalRI[byte] |= 0x1 << bit;
		
	
	
	}
	return 0;

}


static uint32_t storePDE_size ;
static PDE4_t * storedPDE;
/* initial_mm
 * Description: initialise memory.
 * INPUTS:	
            uint32_t size -- size 
 * OUTPUTS:	None
 * RETURN: none
 * SIDE EFFECTS: none
 */
extern void initial_mm(uint32_t size)
{
	storedPDE = (PDE4_t *) r_kmalloc(SIZE_FOUR * size);
	storePDE_size = size;
	int i;
	for (i = 0; i < size; i++)
	{
	
		//(uint32_t)(storedPDE[i]) = 0;
		storedPDE[i].Present = 0;
		storedPDE[i].rw = 1;
		storedPDE[i].ps = 1;
		storedPDE[i].us = 1;
		storedPDE[i].base = M_4MB*(i+NUM_CHARS_ON_STATUS_BAR);

	
	
	}


}

/* umalloc_pde
 * Description: malloc pde.
 * INPUTS:	none 
 * OUTPUTS:	None
 * RETURN: none
 * SIDE EFFECTS: none
 */
extern PDE4_t umalloc_pde()
{

	int i;
	PDE4_t zero;
	zero.Present = 0;
	for (i = 0; i < storePDE_size; i++)
	{
	
		if (storedPDE[i].Present == 0)
		{
			storedPDE[i].Present = 1;
			return storedPDE[i];
		
		}

	
	}

	
	
	return zero;


}
/* ufree_pde
 * Description: free pde.
 * INPUTS:	PDE4_t current -- address of the pde to free 
 * OUTPUTS:	None
 * RETURN: none
 * SIDE EFFECTS: none
 */
extern void ufree_pde(PDE4_t current)
{
	int i;
	for (i = 0; i < storePDE_size; i++)
	{
	
		if (storedPDE[i].base == current.base)
			storedPDE[i].Present = 0;
	
	}


}
