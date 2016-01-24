/* This ECE391OS project is an in class project written by
*Hongchuan Li, Meng Gao, Junqing Deng, Han Jiang in UIUC (spring 2014)
*The current file is modified by Meng Gao
*Students who are taking ECE391 should AVOID copying this file
*/

/* 
 * RTC:
 * Write CMOS address to read or write to port 70h
 * Read/write port 71h to get/set data
 * Reference: http://stanislavs.org/helppc/cmos_ram.html
 * Reference: https://courses.engr.illinois.edu/ece391/references/ds12887.pdf
 */
#ifndef _REC_H
#define _REC_H

#include "pcb.h"

#define RET_SUCCESS 0 
#define RET_ERROR -1



// Ports to write to
#define RW_RTC_ADDR 0x70
#define RW_RTC_DATA 0x71

// RTC Registers
#define RTC_SEC 0x00
#define RTC_SEC_A 0x01
#define RTC_MIN 0x02
#define RTC_MIN_A 0x03
#define RTC_HOUR 0x04
#define RTC_HOUR_A 0x05
#define RTC_WEEK_DAY 0x06
#define RTC_MONTH_DAY 0x07
#define RTC_MONTH 0x08
#define RTC_YEAR 0x09
#define RTC_REG_A 0x0A
#define RTC_REG_B 0x0B
#define RTC_REG_C 0x0C // Read only
#define RTC_REG_D 0x0D // Read only




// initial RTC Register Values (use bitwise OR)
		/* Reg A */
// set DV => turn on oscillator & allow RTC to keep time
#define SET_OSCILLATOR 0x20
// set initial rate selection bits to 500ms(2HZ)
#define SET_INIT_RATE 0x0F
		/* Reg B */
// turn off clock update
#define OFF_CLK 0x80 //DO NOT USE during init
// use periodic interrupt
#define SET_PIE 0x40 
// turn on alarm
#define SET_ALARM 0x20 //DO NOT USE during init
// set Update-Ended after each update cycle
#define SET_UIE 0x10 
// use square wave
#define SET_SQW 0x08 
// use binary
#define SET_binary 0x04 //DO NOT USE during init
// use 24 hour mode
#define SET_24 0x02 
// use daylight savings
#define SET_DAYLIGHT 0x01 



/*Constants*/
#define SET 1
#define RESET 0
#define FREQUENCY_MIN 0
#define FREQUENCY_WRITE 1024
#define POWER_MIN 1
#define POWER_MAX 10
#define RTC_FREQ_SELECT 16
#define SET_FREQ_MASK 0xF0
#define FREQ_LOW4_MASK 0x0F

// initializes the RTC
extern void rtc_init(void);
extern int32_t rtc_open();
extern int32_t rtc_close(uint32_t fd);
extern int32_t rtc_read(pcb_t* mypcb, uint32_t fd, const uint32_t* buf, uint32_t nbytes);
extern int32_t rtc_write(uint32_t fd, const uint32_t* buf, uint32_t nbytes);
extern void rtc_cp2_test();

// test RTC handler
void rtc_handler();
#endif


