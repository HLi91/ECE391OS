/* This ECE391OS project is an in class project written by
*Hongchuan Li, Meng Gao, Junqing Deng, Han Jiang in UIUC (spring 2014)
*The current file is wriiten by Junqing Deng Han Jiang
*Students who are taking ECE391 should AVOID copying this file
*/

#ifndef _KEYBOARD_H
#define _KEYBOARD_H
#include "x86_desc.h"
#include "i8259.h"
#include "lib.h"
#include "lbuffer.h"

#define KEY_RELEASE 0x80
#define KEYBOARD_PORT 0x60

#define LEFT_SHIFT_DN	0x2a  //pressed left shift
#define RIGHT_SHIFT_DN	0x36  //pressed right shift
#define LEFT_SHIFT_UP	0xaa  //released left shift
#define RIGHT_SHIFT_UP	0xb6  //released right shift
#define CTRL_DN			0x1d  //pressed crtl
#define CTRL_UP			0x9d  //released shift
#define ALT_DN			0x38  //pressed alt
#define ALT_UP			0xB8  //released alt
#define CAPS_LOCK		0x3a  //pressed capslock
#define UP				128  //pressed up arrow
#define DOWN			129  //pressed down arrow
#define LEFT			130  //pressed left arrow
#define RIGHT			131  //pressed right arrow
#define FUNC_KEY_1_SCANCODE			59 //F1's scan code. 
typedef enum {SHIFT_MAKE, CTRL_MAKE, ALT_MAKE, NONE_MAKE} func_keys_pressed;
typedef enum {FUNC_KEY_1=FUNC_KEY_1_SCANCODE, FUNC_KEY_2, FUNC_KEY_3, FUNC_KEY_4, FUNC_KEY_5, FUNC_KEY_6,  FUNC_KEY_7} enum_function_keys; 
/* the scancode was find in http://www.osdever.net/bkerndev/Docs/keyboard.htm */

// prints the key pressed onto the screen
	void keyboard_handler(regs* r);
	
// adds keyboard handler to irq descriptor table, and enable interrupt on PIC
	void keyboard_install();




#endif

