/* This ECE391OS project is an in class project written by
*Hongchuan Li, Meng Gao, Junqing Deng, Han Jiang in UIUC (spring 2014)
*The current file is wriiten by Hongchuan Li
*Students who are taking ECE391 should AVOID copying this file
*/

#ifndef _PAGING_H
#define _PAGING_H

#include "lib.h"
#include "x86_desc.h"
#include "paging_init.h"
#include "sig.h"

#define CR0 0x80000001    //PE=1, PG=1
#define CR4 0x00000010	 // PSE: set to mix 4KB and 4MB
#define CR4_PSE 0x10000010
#define USER 1
#define PRESENT 1
#define NPRESENT 0
#define SUPERUSER 0
#define READ 0
#define WRITE 1
#define PAGE_4MB 1
#define PAGE_4KB 0
#define KERNEL 0
#define USERA 3
#define M_4MB 0x400 /*since the 4mb only use the first 10 bits, 1 is 0x400*/
#define BIT_LENGTH_22 22
#define BIT_LENGTH_12 12
#define LOWER_22_BITS_MASK 0x3fffff
#define LOWER_12_BITS_MASK 0xfff
// initialize paging
extern void paging_init();


//Initializes the Kernel Page Directory Entry. 
void init_kpde();

// function that paging_init uses to fill table entries
void init_kpte();

//initialise a kernel page directory entry which maps to a 4MB page.
extern void init_kpde_4MB(uint32_t index, uint32_t base);

//initialise a user level page directory entry which maps to a 4MB page. 
extern void init_upde(PDE4_t * upde, uint32_t index, uint32_t base, uint32_t us, uint32_t size, PDE4_t * pte, uint32_t pte_a);

//get the physical address by providing linear/virtual address and cr3
extern uint32_t get_physical_add(const uint32_t address, uint32_t cr3);




#endif 



