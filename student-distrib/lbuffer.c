/* This ECE391OS project is an in class project written by
*Hongchuan Li, Meng Gao, Junqing Deng, Han Jiang in UIUC (spring 2014)
*The current file is wriiten by Junqing Deng Han Jiang
*Students who are taking ECE391 should AVOID copying this file
*/

#include "lbuffer.h"
#include "types.h"
#include "lib.h"
#include "modex.h"
#include "x86_desc.h"

#define NUM_LINE_9   9
#define INDEX_5   5
#define SIZE_3   3 
#define SIZE_SIX   6 
//static int first_cmd_index = 0;
static int current_hist_index[SIZE_3];
static int max_hist[SIZE_3];
lbuffer_t temp_buffer;
lbuffer_t history[SIZE_3][SIZE_SIX];
//lbuffer_t curent_cmd;
//static int prev;
static int left[SIZE_3];
static int x[SIZE_3], y[SIZE_3];
volatile static int pw_enable = 0;

int display_cmd = 0;
int display_handshake = 0;

int terminal_index;
lbuffer_t buffer[SIZE_3], buffer2[SIZE_3], c_buffer[SIZE_3];

 /*
 * buff_init
 * DESCRITPION: initialise the buffer for screen characters
 * INPUT: none
 * OUTPUT: none
 * RETURN: none
 * SIDE EFFECT: none
 */	
void buff_init()
{
	int i, j;
	// initialize history
	for (terminal_index = 0; terminal_index < SIZE_3; terminal_index++) {
		for (i = 0; i < SIZE_SIX; i++) {
		history[terminal_index][i].size = 0;
		for (j = 0; j<BUFFER_SIZE; j++){
			history[terminal_index][i].data[j] = '\0';
		}
	}
	
	// initialize buffer & current buffer
	for(i = 0; i < BUFFER_SIZE; i++)
	{
		buffer[terminal_index].size = 0;
		c_buffer[terminal_index].size = 0;
		c_buffer[terminal_index].data[i] = '\0';
		buffer[terminal_index].data[i] = '\0';
	}
	}
	terminal_index = 0;
}


 /*
 * buff_clone
 * DESCRITPION: press the arrow key to lookup previous commend
 * INPUT: none
 * OUTPUT: none
 * RETURN: none
 * SIDE EFFECT: none
 */
void buff_clone()
{
	int index;

	if (buffer[terminal_index].size > 1)
	{
		// push back history one slot
		history[terminal_index][INDEX_5] = history[terminal_index][4];
		history[terminal_index][4] = history[terminal_index][SIZE_3];	
		history[terminal_index][SIZE_3] = history[terminal_index][2];
		history[terminal_index][2] = history[terminal_index][1];
		history[terminal_index][1] = buffer[terminal_index];
		// save current into history
		index =history[terminal_index][1].size-1;
		if(buffer[terminal_index].data[buffer[terminal_index].size-1] == '\n')
		{
			history[terminal_index][1].data[index] = '\0';
			history[terminal_index][1].size = buffer[terminal_index].size -1;
		}
		current_hist_index[terminal_index] = -1; //current point to buffer
		max_hist[terminal_index]++;
	}
}

/*
 * clear_buffer
 * DESCRITPION: clear the buffer
 * INPUT: none
 * OUTPUT: none
 * RETURN: none
 * SIDE EFFECT: set all buffer entry to '\0'
 */
void clear_buffer()
{
	buff_clone();

	int i = 0;
	// reset buffer size
	buffer[terminal_index].size = 0;
	// erase all data in buffer
	for(i = 0; i < BUFFER_SIZE; i++)
	{
		buffer[terminal_index].data[i] = '\0';
	}
}

/*
 * clear_c_buffer
 * DESCRITPION: clear the command buffer
 * INPUT: none
 * OUTPUT: none
 * RETURN: none
 * SIDE EFFECT: set all buffer entry to '\0'
 */
 void clear_c_buffer()
{
	int i = 0;
	// clear all data in command buffer
	for(i = 0; i < BUFFER_SIZE; i++)
	{
		c_buffer[terminal_index].data[i] = '\0';
	}
	// reset buffer size
	c_buffer[terminal_index].size = 0;
	
}
 
/*
 * show_prev_cmd
 * DESCRITPION: show previous from buffer line. 
 * INPUT: none
 * OUTPUT: none
 * RETURN: none
 * SIDE EFFECT: none
 */
void show_prev_cmd(){
	int i;
	// put the cursor location at the end of buffer
	left[terminal_index] = 0;
	
	// do nothing if outside buffer range
	if ((current_hist_index[terminal_index] >= INDEX_5)
			|| (current_hist_index[terminal_index] >= max_hist[terminal_index]))
	{
		return;
	}
	
	// skip hist[0], reserved for current entry
	if(current_hist_index[terminal_index] == -1)
	{
		current_hist_index[terminal_index] = 1;
	}
	// else move up history
	else{
		current_hist_index[terminal_index]++;
	}
	
	// clear line
	for(i = 0; i < buffer[terminal_index].size; i++)
	{
		buffer[terminal_index].data[i] = '\0';
	}
	
	// show previous command
	buffer_to_screen(left[terminal_index]);
	buffer[terminal_index] = history[terminal_index][current_hist_index[terminal_index]];
	buffer_to_screen(left[terminal_index]);
	
}

/*
 * show_next_cmd
 * DESCRITPION: show next line from buffer on screen 
 * INPUT: none
 * OUTPUT: none
 * RETURN: none
 * SIDE EFFECT: none
 */
void show_next_cmd(){
	int i;
	// put the cursor at end of buffer
	left[terminal_index] = 0;
	
	// do nothing if outside buffer range
	if(current_hist_index[terminal_index] <= 0)
	{
		return;
	}
	
	// else move down history
	else{
		current_hist_index[terminal_index]--;
	}
	
	// clear line
	for(i = 0; i < buffer[terminal_index].size; i++)
	{
		buffer[terminal_index].data[i] = '\0';
	}
	buffer_to_screen(left[terminal_index]);
	
	// show next command
	buffer[terminal_index] = history[terminal_index][current_hist_index[terminal_index]];
	buffer_to_screen(left[terminal_index]);	
}

/*
 * mod_left
 * DESCRITPION: move the cursor left by one position
 * INPUT: none
 * OUTPUT: none
 * RETURN: none
 * SIDE EFFECT: none
 */
void mod_left(){
	// do nothing if already at beginning of buffer
	if (left[terminal_index]>=buffer[terminal_index].size)
		return;
	
	// else move left
	left[terminal_index]++;
	
	// set cursor
	x[terminal_index] = get_screen_x();
	y[terminal_index] = get_screen_y();

	x[terminal_index]-=left[terminal_index];

	set_cursor(y[terminal_index], x[terminal_index]);
	
}

/*
 * mod_right
 * DESCRITPION: move the cursor right by one position
 * INPUT: none
 * OUTPUT: none
 * RETURN: none
 * SIDE EFFECT: none
 */
void mod_right(){
	// do nothing if already at end of buffer
	if (left[terminal_index] <= 0)
		return;
	// else move right
	left[terminal_index]--;
	
	
	// set cursor, but do not adjust x at end of buffer
	x[terminal_index] = get_screen_x();
	y[terminal_index] = get_screen_y();
	
	if (left[terminal_index] != 0){
		x[terminal_index]-=left[terminal_index];
		x[terminal_index]++;
	}
	set_cursor(y[terminal_index], x[terminal_index]);
}

/*
 * handle_key_input
 * DESCRITPION: adds character to line buffer, or performs special function
 *              depending on the keyin (ex. backspace, EOS)
 * INPUT: uint8_t in -- char read from keyboard
 * OUTPUT: none
 * RETURN: none
 * SIDE EFFECT: modifies the line buffer
 */
extern void handle_key_input(uint8_t in)
{
	if(!pw_enable){
		switch (in){
		case TAB:
			return;
		case BACK:
			//prev = 0;
			//current_hist_index = first_cmd_index;
			de_buffer();
			history[terminal_index][0] = buffer[terminal_index];
			return;
		case UP: show_prev_cmd(); return;
		case DOWN: show_next_cmd(); return;
		case LEFT: mod_left(); return;
		case RIGHT: mod_right(); return;
		// the following cases will NOT return
		case '\n':
		c_buffer[terminal_index] = buffer[terminal_index];
		c_buffer[terminal_index].data[buffer[terminal_index].size] = '\n';
		c_buffer[terminal_index].size++;
		left[terminal_index] = 0; 
		break;
		}
		
		//current_hist_index = first_cmd_index;
		//prev = 0;
		
		add_to_buffer(in);
		
		// backup current buffer
		history[terminal_index][0] = buffer[terminal_index];
	}
	
	else{
		switch (in){
		case TAB:
			return;
		case BACK:
			de_c_buffer();
			return;
		// the following cases will NOT return
		case '\n':
		c_buffer[terminal_index].data[c_buffer[terminal_index].size] = '\n';
		c_buffer[terminal_index].size++;
		break;
		}
		
		//current_hist_index = first_cmd_index;
		//prev = 0;
		
		add_to_c_buffer(in);
	}
}


/*
 * add_to_buffer
 * DESCRITPION: display a new char on screen
 * INPUT: none
 * OUTPUT: none
 * RETURN: 0 success, -1 otherwise
 * SIDE EFFECT: increment size of buffer, and write to buffer
 */
int add_to_buffer(uint8_t temp)
{
	//static int modex_line = 0;
	int i;
	if(buffer[terminal_index].size == BUFFER_SIZE-1 && temp != '\n'){
		return -1;
	}
	if(buffer[terminal_index].size<BUFFER_SIZE)
	{
	// move char back one slot if inserting in the middle
		for (i=0;i<left[terminal_index];i++){
			buffer[terminal_index].data[buffer[terminal_index].size-i] = buffer[terminal_index].data[buffer[terminal_index].size-i-1];
		}
	// add char to location
		buffer[terminal_index].data[buffer[terminal_index].size-left[terminal_index]] = temp;
		buffer[terminal_index].size++;
	// display

		if (!pw_enable)
		{
			if (modeX_enabled == 0)
				buffer_to_screen(left[terminal_index]);	
			else
			{
				
				if (temp  == '\n')
				{
					
						statusBar_to_screen(modex_line);
						modex_line++;
					modex_line %= NUM_LINE_9;
						clear_buffer();
					
				}
				update_statusBar(0,0,0);
				
			}

		}
		return 0;
	}
	else
	return -1;
}




/*
 * de_buffer
 * DESCRITPION: backspace to delete character on screen
 * INPUT: none
 * OUTPUT: none
 * RETURN: 0 success, -1 otherwise
 * SIDE EFFECT: decrease size of buffer, & exchange '\0' with last char
 */
int de_buffer()
{
	int i;
	// do nothing if backspace at beginning
	if(buffer[terminal_index].size-left[terminal_index] == 0){
		return -1;
	}
	if(buffer[terminal_index].size > 0)
	{
	// move char forward one slot if deleting in the middle
		for (i=left[terminal_index];i>0;i--){
			buffer[terminal_index].data[buffer[terminal_index].size-i-1] = buffer[terminal_index].data[buffer[terminal_index].size-i];
		}
	// put end of string
		buffer[terminal_index].data[buffer[terminal_index].size -1] = '\0';
		buffer[terminal_index].size--;
	// display
		if (!pw_enable)
		{
			if (modeX_enabled == 0)
				buffer_to_screen(left[terminal_index]);	
			else
				update_statusBar(0,0,0);
		}
		return 0;
	}
	
	return -1;
}

/*
 * add_to_c_buffer
 * DESCRITPION: add a char to c_buffer
 * INPUT: none
 * OUTPUT: none
 * RETURN: 0 success, -1 otherwise
 * SIDE EFFECT: increment size of buffer, and write to buffer
 */
int add_to_c_buffer(uint8_t temp)
{
	if(c_buffer[terminal_index].size == BUFFER_SIZE-1 && temp != '\n'){
		return -1;
	}
	if(c_buffer[terminal_index].size<BUFFER_SIZE)
	{
		c_buffer[terminal_index].data[c_buffer[terminal_index].size] = temp;
		c_buffer[terminal_index].size++;		
		return 0;
	}
	else
	return -1;
}

/*
 * de_c_buffer
 * DESCRITPION: backspace to delete character on c_buffer
 * INPUT: none
 * OUTPUT: none
 * RETURN: 0 success, -1 otherwise
 * SIDE EFFECT: decrease size of buffer, & exchange '\0' with last char
 */
int de_c_buffer()
{
	// do nothing if backspace at beginning
	if(c_buffer[terminal_index].size == 0){
		return -1;
	}
	if(c_buffer[terminal_index].size > 0)
	{
		c_buffer[terminal_index].data[c_buffer[terminal_index].size -1] = '\0';
		c_buffer[terminal_index].size--;
		return 0;
	}
	
	return -1;
}

/*
 * copy_buffer
 * DESCRITPION: read size_src from src to buffer
 * INPUT: 
 *	 uint8_t *src -- source address.
 *	 int size_src -- the size to read.
 * OUTPUT: none
 * RETURN: 0 success, -1 otherwise
 * SIDE EFFECT: write to buffer
 */
int copy_buffer(uint8_t *src, int size_src)
{
	int i = 0;
	// nullity check
	if((size_src > BUFFER_SIZE)||(src == NULL))
	{
		return -1;
	}
	
	clear_buffer();
	
	// copy data
	for(i = 0; i< size_src; i++)
	{
		if(src[i] == '\0')
		break;
		buffer[terminal_index].data[i] = src[i];
	}
	return 0;
	
}



void sc_pw_enable(uint32_t bit)
{

	pw_enable = (int)bit;
	if (bit == 0)
	{
	
			//somehow clear the buffer or don't even store them
	
	}
}


