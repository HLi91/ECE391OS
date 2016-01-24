/* This ECE391OS project is an in class project written by
*Hongchuan Li, Meng Gao, Junqing Deng, Han Jiang in UIUC (spring 2014)
*The current file is wriiten by Hongchuan Li
*Students who are taking ECE391 should AVOID copying this file
*/


#ifndef _KMALLOC_H
#define _KMALLOC_H
#include "types.h"
#include "x86_desc.h"



extern int32_t kmalloc(uint32_t size, uint32_t align);
extern int32_t naive_kfree(uint32_t size);

extern uint32_t request_page();
extern void release_page(uint32_t address);
extern uint32_t initial_paging_sc();
extern uint32_t r_kmalloc(uint32_t size);
extern void r_kfree(uint32_t addr);
uint32_t set_memory_busy(uint32_t pos, uint32_t size);
uint32_t check_usable(uint32_t pos, uint32_t size);
extern void initial_mm(uint32_t size);
extern PDE4_t umalloc_pde();
extern void ufree_pde(PDE4_t current);



#endif // !KMALLOC_H


