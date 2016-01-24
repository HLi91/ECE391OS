/* This ECE391OS project is an in class project written by
*Hongchuan Li, Meng Gao, Junqing Deng, Han Jiang in UIUC (spring 2014)
*The current file is wriiten by Meng Gao
*Students who are taking ECE391 should AVOID copying this file
*/

#include "i8259.h"
#include "lib.h"
#include "x86_desc.h"
#include "idt.h"
#include "rtc.h"
#include "idt.h"
#include "types.h"
#include "sig.h"
#define EPENDSIG -8

static const int BUF_VALUE_2 = 2;
uint8_t reg;
static volatile uint32_t RTC_INT_FLAG;

/* rtc_init
 * Description: Initializes the rtc 
 * INPUTS:	None
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: set the rtc to with default value
 */
void rtc_init(void){

	/* find default RegA value */
	reg = SET_OSCILLATOR | SET_INIT_RATE;
	/* set Reg A */
	outb(RTC_REG_A, RW_RTC_ADDR);
	outb(reg, RW_RTC_DATA);
	
	/* find default Reg B value */
	reg = SET_PIE | SET_SQW | SET_24 | SET_DAYLIGHT;	
	/* Set Reg B */
	outb(RTC_REG_B, RW_RTC_ADDR);
	outb(reg, RW_RTC_DATA);
	
	/* install handler */
	irq_install_handler(IDT_OFFSET + IRQ_RTC, rtc_handler);
	
	/* unmask irq */
	enable_irq(IRQ_RTC);
}

/* rtc_handler
 * Description: rtc test handler/driver
 * INPUTS:	None
 * OUTPUTS:	prints notification of RTC interrupt on screen
 * RETURN: None
 * SIDE EFFECTS: None
 */
void rtc_handler(void)
{
	// test message
	// printf("RTC Interrupt! !!!!!!!!!!!!!!!!!        \n");
	
	// RTC test program
	// test_interrupts();
	
	// notify RTC interrupt to internal state
	RTC_INT_FLAG = SET;
	
	/* clear RTC Flags by reading Reg C */
	outb(RTC_REG_C, RW_RTC_ADDR);
	inb(RW_RTC_DATA);
	
}

/*
 * rtc_open
 * DESCRITPION: Sets the RTC to default 2Hz, and open RTC
 * INPUT: none
 * OUTPUT: none
 * RETURN: none
 * SIDE EFFECT: none
 */
int32_t rtc_open(){
	uint32_t flags;
	
	cli_and_save(flags);
	
	// get old register A value
	outb(RTC_REG_A , RW_RTC_ADDR);
	reg = inb(RW_RTC_DATA);

	// set RTC to 2 Hz
	outb(RTC_REG_A, RW_RTC_ADDR);
	outb(reg | SET_INIT_RATE, RW_RTC_DATA);
	
	restore_flags(flags);
	
	// return 0 on success
	return RET_SUCCESS;
}

/*
 * rtc_close
 * DESCRITPION: none
 * INPUT: fd: not used
 * OUTPUT: none
 * RETURN: none
 * SIDE EFFECT: none
 */
int32_t rtc_close(uint32_t fd){
	return RET_SUCCESS;
}

/*
 * rtc_read
 * DESCRITPION: waits for an interrupt to occur
 * INPUT: none
 * OUTPUT: none
 * RETURN: 0 on success
 * SIDE EFFECT: wait until an interrupt
 */
int32_t rtc_read(pcb_t* mypcb, uint32_t fd, const uint32_t* buf, uint32_t nbytes){
	
	
	// reset RTC flag
	RTC_INT_FLAG = RESET;
	
	// printf("RTC Interrupt!                             \n");
	
	// wait for next interrupt
	while (RTC_INT_FLAG != SET){
		if (check_signal((uint32_t)mypcb)){
			return EPENDSIG;
		}
	}
	
	// printf("----------------RTC Interrupt---------------\n");
	
	// return on success
	return RET_SUCCESS;
}


 /*
 * rtc_write
 * DESCRITPION: Writes a given frequency to RTC. RTC is able to generate
 *				interrupt at rate of power of 2, driver limits the
 *				frequency up to 1024. 
 * INPUT: none
 * OUTPUT: none
 * RETURN: 0 on success, -1 on failure
 * SIDE EFFECT: none
 */
int32_t rtc_write(uint32_t fd, const uint32_t* buf, uint32_t nbytes){
	
	int frequency;
	int power;
	int input_error;
	int temp;
	uint32_t flags;
	
	// get frequency in Hz
	frequency = *buf;
	
	/*
	// return if input outside range
	if (frequency < FREQUENCY_MIN || frequency > FREQUENCY_MAX){
		return RET_ERROR;
	}
	*/
	
	// check if it is a power of 2 within range
	if (frequency == 0){
		input_error = RESET;
	}
	else{
	for (power = POWER_MIN; power <= POWER_MAX; power++){
		if ((1<<power)==frequency){
			input_error = RESET;
			temp = power;
			break;
		}
		else{
			input_error = SET;
		}
	}
	}
	
	
	
	// return if input is invalid
	if (input_error == SET){
		printf("-----------------INVALID INPUT---------------------    \n");
		return RET_ERROR;
	}
	// else write to register A to change rate
	else{
		cli_and_save(flags);
		
		// get current register A
		outb(RTC_REG_A, RW_RTC_ADDR);
		// set frequency
		reg = (inb(RW_RTC_DATA) & SET_FREQ_MASK) | ((RTC_FREQ_SELECT - temp)& FREQ_LOW4_MASK);
		// write to register A
		outb(RTC_REG_A, RW_RTC_ADDR);
		outb(reg, RW_RTC_DATA);
		
		restore_flags(flags);
		
		//printf("RTC Interrupt Rate Changed!                             \n");
		return RET_SUCCESS;
	}
	
	printf("-----------------UNKNOWN FAILURE-----------------------    \n");
	return RET_ERROR;
}

	
	
	

/*
 * rtc_cp2_test
 * DESCRITPION: test code for checkpoint 2 only, serves no other purpose
 * INPUT: none
 * OUTPUT: none
 * RETURN: none
 * SIDE EFFECT: none
 */
void rtc_cp2_test(){
	//testing code removed
}


	
	
	
	
