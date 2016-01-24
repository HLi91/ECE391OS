/* This ECE391OS project is an in class project written by
*Hongchuan Li, Meng Gao, Junqing Deng, Han Jiang in UIUC (spring 2014)
*The current file is wriiten by Hongchuan Li
*Students who are taking ECE391 should AVOID copying this file
*/

#ifndef _PID_H
#define _PID_H
#include "types.h"
#include "x86_desc.h"









uint32_t cur_pid;

extern void initial_all_pid(void * handler);
extern int get_pid_map();
extern int request_new_pid();
extern int release_pid(int pid_num);
extern uint32_t get_cur_pid();
extern uint32_t get_next_pid();
extern void set_cur_pid(uint32_t pid);
extern void sleep_cur();
extern uint32_t wake_up_pid(uint32_t pid_num);




#endif // !_PID_H



