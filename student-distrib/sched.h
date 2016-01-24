/* This ECE391OS project is an in class project written by
*Hongchuan Li, Meng Gao, Junqing Deng, Han Jiang in UIUC (spring 2014)
*The current file is wriiten by Hongchuan Li
*Students who are taking ECE391 should AVOID copying this file
*/


#ifndef _SCHED_H
#define _SCHED_H

#include "types.h"
#include "filesys.h"
#include "i8259.h"
#include "lib.h"
#include "x86_desc.h"
#include "idt.h"
#include "rtc.h"
#include "idt.h"
#include "types.h"
#include "pcb.h"
#include "sig.h"

extern void init_sched();
void scheduler(regs * registers);
void context_switch(int type, pcb_t* mypcb);
void update_tick(regs * registers);
extern void disable_sched();

#endif /* _SCHED_H */

