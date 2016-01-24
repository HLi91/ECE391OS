/* This ECE391OS project is an in class project written by
*Hongchuan Li, Meng Gao, Junqing Deng, Han Jiang in UIUC (spring 2014)
*The current file is wriiten by Hongchuan Li and modified by others
*Students who are taking ECE391 should AVOID copying this file
*/

#ifndef FILE_OP_H
#define FILE_OP_H
#include "types.h"
#include "pcb.h"

//read standard input
extern int32_t stdin_read(int32_t fd, void* buf, int32_t nbytes, pcb_t* mypcb);
//write standard input
extern int32_t stdin_write(int32_t fd, const void* buf, int32_t nbytes, pcb_t* mypcb);
//open standard input
extern int32_t stdin_open(const uint8_t* filename, pcb_t* mypcb);
//close standard input
extern int32_t stdin_close(int32_t fd, pcb_t* mypcb);
//read standard output
extern int32_t stdout_read(int32_t fd, void* buf, int32_t nbytes, pcb_t* mypcb);
//write standard output
extern int32_t stdout_write(int32_t fd, const void* buf, int32_t nbytes, pcb_t* mypcb);
//open standard output
extern int32_t stdout_open(const uint8_t* filename, pcb_t* mypcb);
//close standard output
extern int32_t stdout_close(int32_t fd, pcb_t* mypcb);

//read file system
extern int32_t filesys_read(int32_t fd, void* buf, int32_t nbytes, pcb_t* mypcb);
//write file system
extern int32_t filesys_write(int32_t fd, const void* buf, int32_t nbytes, pcb_t* mypcb);
//open file system
extern int32_t filesys_open(const uint8_t* filename, pcb_t* mypcb);
//close file system
extern int32_t filesys_close(int32_t fd, pcb_t* mypcb);

//read system RTC
extern int32_t sys_rtc_read(int32_t fd, void* buf, int32_t nbytes, pcb_t* mypcb);
//write system RTC
extern int32_t sys_rtc_write(int32_t fd, const void* buf, int32_t nbytes, pcb_t* mypcb);
// open system RTC
extern int32_t sys_rtc_open(const uint8_t* filename, pcb_t* mypcb);
// close system RTC
extern int32_t sys_rtc_close(int32_t fd, pcb_t* mypcb);

//read directory
extern int32_t dir_read(int32_t fd, void* buf, int32_t nbytes, pcb_t* mypcb);
//write directory
extern int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes, pcb_t* mypcb);
//open directory
extern int32_t dir_open(const uint8_t* filename, pcb_t* mypcb);
//close directory
extern int32_t dir_close(int32_t fd, pcb_t* mypcb);





#endif // !FILE_OP_H

