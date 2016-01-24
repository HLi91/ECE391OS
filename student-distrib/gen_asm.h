/* This ECE391OS project is an in class project written by
*Hongchuan Li, Meng Gao, Junqing Deng, Han Jiang in UIUC (spring 2014)
*The current file is wriiten by Hongchuan Li and modified by others
*Students who are taking ECE391 should AVOID copying this file
*/


#ifndef _GEN_ASM_H
#define _GEN_ASM_H

#include "types.h"
#include "sig.h"

extern uint32_t read_eip();
extern void sr_paging();
extern void graphic_mode();
extern void sig_kill_helper();
extern void sig_ignore_helper();


#endif // !_GEN_AMS_h
