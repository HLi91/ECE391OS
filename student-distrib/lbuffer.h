/* This ECE391OS project is an in class project written by
*Hongchuan Li, Meng Gao, Junqing Deng, Han Jiang in UIUC (spring 2014)
*The current file is wriiten by Junqing Deng Han Jiang
*Students who are taking ECE391 should AVOID copying this file
*/

#ifndef _LBUFFER_H
#define _LBUFFER_H
#include "types.h"
#include "lib.h"
#include "x86_desc.h"
//#define BUFFER_SIZE 128

// constants
#define UP				128
#define DOWN			129
#define LEFT			130
#define RIGHT			131
#define INITIAL_HIST	100
#define END_OF_SCREEN_X 80
#define HIST_MAX		6

// keys
#define TAB				'\t'
#define BACK			'\b'
#define ENTER			'\n'
#define EOS				'\0'

typedef struct lbuffer_t {
	unsigned char data[BUFFER_SIZE];
	int size;
} lbuffer_t;


// terminal
extern int terminal_index;

extern void clear_buffer();
extern void clear_c_buffer();
extern lbuffer_t get_buffer();
extern int add_to_buffer(uint8_t temp);
extern int add_to_c_buffer(uint8_t temp);
extern int de_buffer();
extern int de_c_buffer();
extern int copy_buffer(uint8_t *src, int size_src);
extern void handle_key_input(uint8_t in);
extern void buff_init();
extern int get_left();
extern void sc_pw_enable(uint32_t bit);


// line buffers for terminal #1,2,3
extern lbuffer_t buffer[3], buffer2[3], c_buffer[3];
extern int display_cmd, display_handshake;
#endif
