/* lib.c - Some basic library functions (printf, strlen, etc.)
 * vim:ts=4 noexpandtab
 */

#include "lib.h"
#include "lbuffer.h"
#include "x86_desc.h"
#include "pid.h"
#include "modex.h"

#define VIDEO_SIZE 0x1000

#define NUM_COLS 80
#define NUM_ROWS 25
#define ATTRIB 0x1

#define SETLOW 0x0F
#define SETHIGH 0x0E
#define CURSORPORT 0x03D4
#define MASK_ONE 0xFF
#define ONE_BYTE 8
static const int VERTICAL_POS_200 = 200;
static const int RADIX_10 = 10;
static const int CONV_BUF_SIZE_36 = 36;
static const int CONV_BUF_SIZE_8 = 8;
static const int RADIX_16 = 16;
static const int CONV_BUF_SIZE = 64;
static const int HORIZ_WIDTH = 400;
static const int MX_WIDTH = 320;
static int screen_x[3];
static int screen_y[3];
static int buff_x[3];
static int buff_y[3];//need to init somewhere
static char* video_mem = (char *)VIDEO;


/*
 * set_screen
 * DESCRITPION: set screen_x and screen_y
 * INPUT: int x -- x coordinate
          int y -- y coordinate
 * OUTPUT: none
 * RETURN: none
 * SIDE EFFECT: none
 */
void set_screen(int x, int y)
{
	screen_x[0] = 0;
	screen_y[0] = 0;

	screen_x[1] = 0;
	screen_y[1] = 0;

	screen_x[2] = 0;
	screen_y[2] = 0;
}

/*
 * get_screen_x
 * DESCRITPION: get screen_x value
 * INPUT: none
 * OUTPUT: none
 * RETURN: screen_x
 * SIDE EFFECT: none
 */
int get_screen_x()
{
	uint32_t process_ter = kernel_pid[get_cur_pid()].terminal_num;
	return screen_x[process_ter];
}

/*
 * get_screen_y
 * DESCRITPION: get screen_y value
 * INPUT: none
 * OUTPUT: none
 * RETURN: screen_y
 * SIDE EFFECT: none
 */
int get_screen_y()
{
	uint32_t process_ter = kernel_pid[get_cur_pid()].terminal_num;
	return screen_y[process_ter];
}


/*
 * set_cursor
 * DESCRITPION: set the cursor to a designated position. 
 * INPUT: int row -- row position
          int col -- column position
 * OUTPUT: none
 * RETURN: none
 * SIDE EFFECT: none
 */
void set_cursor(int row, int col)
 {
	 /*uint32_t process_ter = kernel_pid[get_cur_pid()].terminal_num;
	 if (process_ter != cur_terminal)
		 return;*/

    unsigned short position=(row*NUM_COLS) + col;

    // cursor LOW port to vga INDEX register
    outb(SETLOW, CURSORPORT);
    outb((unsigned char)(position&MASK_ONE),CURSORPORT+1);
    // cursor HIGH port to vga INDEX register
    outb(SETHIGH, CURSORPORT);
    outb((unsigned char )((position>>ONE_BYTE)&MASK_ONE),CURSORPORT+1);
 }

 
 /*
 * empty_screen
 * DESCRITPION: clear the whole screen. 
                
 * INPUT: none
 * OUTPUT: none
 * RETURN: none
 * SIDE EFFECT: set the cursor to up left. 
 */
 void empty_screen()
 {
	set_cursor(0,0);
	screen_x[cur_terminal] = 0;
	screen_y[cur_terminal] = 0;
	clear();
	
 }

 


/*
* void clear(void);
    DESCRITPION: Clears video memory
*   Inputs: void
*   Return Value: none
*	Function:
*/

void
clear(void)
{
	//uint32_t pid = get_cur_pid();

	switch (cur_terminal)
	{
		case 0:
			video_mem = (char *)VIDEO_T0;
			break;
		case 1:
			video_mem = (char *)VIDEO_T1;
			break;

		case 2:
			video_mem = (char *)VIDEO_T2;
			break;
		default:
			video_mem = (char *)VIDEO;	//this should not happened, just for error handling	

	}
    int32_t i;
    for(i=0; i<NUM_ROWS*NUM_COLS; i++) {
        *(uint8_t *)(video_mem + (i << 1)) = ' ';
        *(uint8_t *)(video_mem + (i << 1) + 1) = ATTRIB;
		}
	screen_x[cur_terminal] = 0;
	screen_y[cur_terminal] = 0;
	buff_x[cur_terminal] = 0;
	buff_y[cur_terminal] = 0;

	display_video(cur_terminal);

	if (modeX_enabled == 1)
	{
		int8_t buf[MX_WIDTH];
		int i;
		memset((void *) buf, '\0', MX_WIDTH);
		for (i = 0; i < VERTICAL_POS_200; i++)
		{
		
			bmp_horiz_line(0, i, (unsigned char *) buf, 1, HORIZ_WIDTH);
		
		}
		show_screen();
		for (i = 0; i < VERTICAL_POS_200; i++)
		{
		
			bmp_horiz_line(0, i, (unsigned char *) buf, 1, HORIZ_WIDTH);
		
		}
		show_screen();
		modex_line = 0;
	
	
	}

}

/* Standard printf().
 * Only supports the following format strings:
 * %%  - print a literal '%' character
 * %x  - print a number in hexadecimal
 * %u  - print a number as an unsigned integer
 * %d  - print a number as a signed integer
 * %c  - print a character
 * %s  - print a string
 * %#x - print a number in 32-bit aligned hexadecimal, i.e.
 *       print 8 hexadecimal digits, zero-padded on the left.
 *       For example, the hex number "E" would be printed as
 *       "0000000E".
 *       Note: This is slightly different than the libc specification
 *       for the "#" modifier (this implementation doesn't add a "0x" at
 *       the beginning), but I think it's more flexible this way.
 *       Also note: %x is the only conversion specifier that can use
 *       the "#" modifier to alter output.
 * */
int32_t
printf(int8_t *format, ...)
{
	uint32_t pid = get_cur_pid();

	switch (kernel_pid[pid].terminal_num)
	{
		case 0:
			video_mem = (char *)VIDEO_T0;
			break;
		case 1:
			video_mem = (char *)VIDEO_T1;
			break;

		case 2:
			video_mem = (char *)VIDEO_T2;
			break;
		default:
			video_mem = (char *)VIDEO;	//this should not happened, just for error handling	

	}
	if (cur_terminal == kernel_pid[pid].terminal_num)
	clear_buffer();
	/* Pointer to the format string */
	int8_t* buf = format;

	/* Stack pointer for the other parameters */
	int32_t* esp = (void *)&format;
	esp++;

	/*if (screen_y == NUM_ROWS)
	{
	
		putc('\0');
		screen_x = 0;
		screen_y = NUM_ROWS-1;
		buff_x =screen_x;
		buff_y = screen_y;
	
	}*/

	while(*buf != '\0') {
		switch(*buf) {
			case '%':
				{
					int32_t alternate = 0;
					buf++;

format_char_switch:
					/* Conversion specifiers */
					switch(*buf) {
						/* Print a literal '%' character */
						case '%':
							putc('%');
							break;

						/* Use alternate formatting */
						case '#':
							alternate = 1;
							buf++;
							/* Yes, I know gotos are bad.  This is the
							 * most elegant and general way to do this,
							 * IMHO. */
							goto format_char_switch;

						/* Print a number in hexadecimal form */
						case 'x':
							{
				                int8_t conv_buf[CONV_BUF_SIZE];
								if(alternate == 0) {
					            itoa(*((uint32_t *) esp), conv_buf, RADIX_16);
									puts(conv_buf);
								} else {
									int32_t starting_index;
									int32_t i;
					                itoa(*((uint32_t *) esp), &conv_buf[CONV_BUF_SIZE_8],
							            RADIX_16);
					                i = starting_index = strlen(&conv_buf[CONV_BUF_SIZE_8]);
									while(i < ONE_BYTE) {
										conv_buf[i] = '0';
										i++;
									}
									puts(&conv_buf[starting_index]);
								}
								esp++;
							}
							break;

						/* Print a number in unsigned int form */
						case 'u':
							{
				int8_t conv_buf[CONV_BUF_SIZE_36];
				itoa(*((uint32_t *) esp), conv_buf, RADIX_10);
								puts(conv_buf);
								esp++;
							}
							break;

						/* Print a number in signed int form */
						case 'd':
							{
				int8_t conv_buf[CONV_BUF_SIZE_36];
								int32_t value = *((int32_t *)esp);
								if(value < 0) {
									conv_buf[0] = '-';
					itoa(-value, &conv_buf[1], RADIX_10);
								} else {
					itoa(value, conv_buf, RADIX_10);
								}
								puts(conv_buf);
								esp++;
							}
							break;

						/* Print a single character */
						case 'c':
							putc( (uint8_t) *((int32_t *)esp) );
							esp++;
							break;

						/* Print a NULL-terminated string */
						case 's':
							puts( *((int8_t **)esp) );
							esp++;
							break;

						default:
							break;
					}

				}
				break;

			default:
				putc(*buf);
				break;
		}
		buf++;
	}
	
	uint32_t process_ter = kernel_pid[get_cur_pid()].terminal_num;
	if (cur_terminal == process_ter)
	set_cursor(screen_y[process_ter], screen_x[process_ter]);
	return (buf - format);
}

/*
* int32_t puts(int8_t* s);
*   Inputs: int_8* s = pointer to a string of characters
*   Return Value: Number of bytes written
*	Function: Output a string to the console 
*/

int32_t
puts(int8_t* s)
{
	if (modeX_enabled == 1)
	{
	
		modex_to_screen(modex_line, s);
		modex_line++;
		modex_line%=9;

	
	
	}
	register int32_t index = 0;
	while(s[index] != '\0') {
		putc(s[index]);
		index++;
	}

	//set_cursor(get_screen_y(), get_screen_x());
	return index;
}

/*
* void putc(uint8_t c);
*   Inputs: uint_8* c = character to print
*   Return Value: void
*	Function: Output a character to the console 
*/

void
putc(uint8_t c)
{
	uint32_t flag;
	cli_and_save(flag);
	uint32_t pid = get_cur_pid();
	uint32_t process_ter = kernel_pid[get_cur_pid()].terminal_num;
	switch (kernel_pid[pid].terminal_num)
	{
		case 0:
			video_mem = (char *)VIDEO_T0;
			break;
		case 1:
			video_mem = (char *)VIDEO_T1;
			break;

		case 2:
			video_mem = (char *)VIDEO_T2;
			break;
		default:
			video_mem = (char *)VIDEO;	//this should not happened, just for error handling	

	}

    if(c == '\n' || c == '\r') {
        screen_y[process_ter]++;
        screen_x[process_ter]=0;
    } else {
        *(uint8_t *)(video_mem + ((NUM_COLS*screen_y[process_ter] + screen_x[process_ter]) << 1)) = c;
        *(uint8_t *)(video_mem + ((NUM_COLS*screen_y[process_ter] + screen_x[process_ter]) << 1) + 1) = ATTRIB;
        screen_x[process_ter]++;
        
        screen_y[process_ter] = (screen_y[process_ter] + (screen_x[process_ter] / NUM_COLS));
		screen_x[process_ter] %= NUM_COLS;
    }
	if (screen_y[process_ter] == NUM_ROWS)
	{
	
		shift_video_mem();
		screen_y[process_ter]--;
	
	}
	

	buff_x[process_ter] = screen_x[process_ter];
	buff_y[process_ter] = screen_y[process_ter];
	
	display_video(kernel_pid[pid].terminal_num);
	restore_flags(flag);
	
}

/*
 * buffer_to_screen
 * DESCRITPION: output buffer onto screen. 
 * INPUT: int left -- left side position. 
 * OUTPUT: none
 * RETURN: none
 * SIDE EFFECT: none
 */
void buffer_to_screen(int left)
{
	//uint32_t pid = get_cur_pid();

	switch (cur_terminal)
	{
		case 0:
			video_mem = (char *)VIDEO_T0;
			break;
		case 1:
			video_mem = (char *)VIDEO_T1;
			break;

		case 2:
			video_mem = (char *)VIDEO_T2;
			break;
		default:
			video_mem = (char *)VIDEO;	

	}

	int i, temp_x;
	screen_x[cur_terminal] = buff_x[cur_terminal];
	screen_y[cur_terminal] = buff_y[cur_terminal];
	for (i=0; i<buffer[terminal_index].size+1; i++)
	{
		if (i==BUFFER_SIZE){
			
			break;
		}
		uint8_t c = (uint8_t)buffer[terminal_index].data[i];
		if(c == '\n' || c == '\r') {
			//screen_y++;
			//screen_x=0;
			if (screen_y[cur_terminal] == NUM_ROWS)
			{
			
				shift_video_mem();
				
				
			
			}
			update_buffer_y();
			clear_buffer();
			screen_y[cur_terminal]  =screen_y[cur_terminal] +1 - screen_y[cur_terminal]/(NUM_ROWS-1);
			screen_x[cur_terminal] =0;
			set_cursor(screen_y[cur_terminal], screen_x[cur_terminal]);
			display_video(-1);
			return;
		} else {
			*(uint8_t *)(video_mem + ((NUM_COLS*screen_y[cur_terminal] + screen_x[cur_terminal]) << 1)) = c;
			*(uint8_t *)(video_mem + ((NUM_COLS*screen_y[cur_terminal] + screen_x[cur_terminal]) << 1) + 1) = ATTRIB;
			screen_x[cur_terminal]++;
			screen_y[cur_terminal] = (screen_y[cur_terminal] + (screen_x[cur_terminal] / NUM_COLS));
			screen_x[cur_terminal]%=NUM_COLS;
			if (screen_y[cur_terminal] == NUM_ROWS)
			{
				shift_video_mem();
				
				screen_y[cur_terminal]--;
				buff_y[cur_terminal]--;
				//update_buffer_y();
				//clear_buffer();
			}
		}
	}
	if (screen_x[cur_terminal]!=0){
	
	temp_x = screen_x[cur_terminal] - left - 1;
	screen_x[cur_terminal]--;
	}
	set_cursor(screen_y[cur_terminal], temp_x);
	display_video(-1);
	return	;
	
}


/*
 * shift_video_mem
 * DESCRITPION: shift video memory.
 * INPUT: none
 * OUTPUT: none
 * RETURN: none
 * SIDE EFFECT: none
 */
void shift_video_mem()
{
	//uint32_t pid = get_cur_pid();

	//switch (kernel_pid[pid].terminal_num)
	//{
	//	case 0:
	//		video_mem = (char *)VIDEO_T0;
	//		break;
	//	case 1:
	//		video_mem = (char *)VIDEO_T1;
	//		break;

	//	case 2:
	//		video_mem = (char *)VIDEO_T2;
	//		break;
	//	default:
	//		video_mem = (char *)VIDEO;	//this should not happened, just for error handling	

	//}

	int i,j;
	for (i=0; i<NUM_ROWS-1; i++)	//i is y
	{
	
		for (j = 0; j<NUM_COLS; j++)
		{
		
			*(uint8_t *)(video_mem + ((NUM_COLS*i + j) << 1)) = *(uint8_t *)(video_mem + ((NUM_COLS*(i+1) + j) << 1));
			*(uint8_t *)(video_mem + ((NUM_COLS*i + j) << 1)+1) = *(uint8_t *)(video_mem + ((NUM_COLS*(i+1) + j) << 1)+1);
		
		}
	
	
	}
	for (j = 0; j<NUM_COLS; j++)
		{
		
			*(uint8_t *)(video_mem + ((NUM_COLS*(NUM_ROWS-1) + j) << 1)) = 0;
			*(uint8_t *)(video_mem + ((NUM_COLS*(NUM_ROWS-1) + j) << 1)+1) = ATTRIB;
		
		}

	//display_video(kernel_pid[pid].terminal_num);
	//buff_y--;

}

/*
 * update_buffer_y
 * DESCRITPION: update a row of buffer.
 * INPUT: none
 * OUTPUT: none
 * RETURN: none
 * SIDE EFFECT: none
 */
void update_buffer_y()
{
	buff_x[cur_terminal] = 0;
	if (buff_y[cur_terminal] >= NUM_ROWS-1)
	{
		shift_video_mem();
		buff_y[cur_terminal] = NUM_ROWS-1;
		
		return;
	}
	buff_y[cur_terminal] += buffer[terminal_index].size/NUM_COLS+1;
}


/*
* int8_t* itoa(uint32_t value, int8_t* buf, int32_t radix);
*   Inputs: uint32_t value = number to convert
*			int8_t* buf = allocated buffer to place string in
*			int32_t radix = base system. hex, oct, dec, etc.
*   Return Value: number of bytes written
*	Function: Convert a number to its ASCII representation, with base "radix"
*/

int8_t*
itoa(uint32_t value, int8_t* buf, int32_t radix)
{
	static int8_t lookup[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	int8_t *newbuf = buf;
	int32_t i;
	uint32_t newval = value;

	/* Special case for zero */
	if(value == 0) {
		buf[0]='0';
		buf[1]='\0';
		return buf;
	}

	/* Go through the number one place value at a time, and add the
	 * correct digit to "newbuf".  We actually add characters to the
	 * ASCII string from lowest place value to highest, which is the
	 * opposite of how the number should be printed.  We'll reverse the
	 * characters later. */
	while(newval > 0) {
		i = newval % radix;
		*newbuf = lookup[i];
		newbuf++;
		newval /= radix;
	}

	/* Add a terminating NULL */
	*newbuf = '\0';

	/* Reverse the string and return */
	return strrev(buf);
}

/*
* int8_t* strrev(int8_t* s);
*   Inputs: int8_t* s = string to reverse
*   Return Value: reversed string
*	Function: reverses a string s
*/

int8_t*
strrev(int8_t* s)
{
	register int8_t tmp;
	register int32_t beg=0;
	register int32_t end=strlen(s) - 1;

	while(beg < end) {
		tmp = s[end];
		s[end] = s[beg];
		s[beg] = tmp;
		beg++;
		end--;
	}

	return s;
}
/* strcat
 * Description: connect two strings
 * INPUTS:	
 int8_t * dest  -- destination string
 const int8_t * source -- source string
 int size -- size. 
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: none
 */
void strcat(int8_t * dest, const int8_t * source, int size)
{
	int i;
	int j;
	for (i = 0; i<size; i++)
	{
	
		if (dest[i] == '\0')
			break;
	
	
	}
	j = 0;
	for (; i < size; i++, j++)
	{
		dest[i] = source[j];
		if (source[j] == '\0')
			return;
		

	}



}

/*
* uint32_t strlen(const int8_t* s);
*   Inputs: const int8_t* s = string to take length of
*   Return Value: length of string s
*	Function: return length of string s
*/

uint32_t
strlen(const int8_t* s)
{
	register uint32_t len = 0;
	while(s[len] != '\0')
		len++;

	return len;
}

/*
* void* memset(void* s, int32_t c, uint32_t n);
*   Inputs: void* s = pointer to memory
*			int32_t c = value to set memory to
*			uint32_t n = number of bytes to set
*   Return Value: new string
*	Function: set n consecutive bytes of pointer s to value c
*/

void*
memset(void* s, int32_t c, uint32_t n)
{
	c &= MASK_ONE;
	asm volatile("                  \n\
			.memset_top:            \n\
			testl   %%ecx, %%ecx    \n\
			jz      .memset_done    \n\
			testl   $0x3, %%edi     \n\
			jz      .memset_aligned \n\
			movb    %%al, (%%edi)   \n\
			addl    $1, %%edi       \n\
			subl    $1, %%ecx       \n\
			jmp     .memset_top     \n\
			.memset_aligned:        \n\
			movw    %%ds, %%dx      \n\
			movw    %%dx, %%es      \n\
			movl    %%ecx, %%edx    \n\
			shrl    $2, %%ecx       \n\
			andl    $0x3, %%edx     \n\
			cld                     \n\
			rep     stosl           \n\
			.memset_bottom:         \n\
			testl   %%edx, %%edx    \n\
			jz      .memset_done    \n\
			movb    %%al, (%%edi)   \n\
			addl    $1, %%edi       \n\
			subl    $1, %%edx       \n\
			jmp     .memset_bottom  \n\
			.memset_done:           \n\
			"
			:
			: "a"(c << 24 | c << 16 | c << ONE_BYTE | c), "D"(s), "c"(n)
			: "edx", "memory", "cc"
			);

	return s;
}

/*
* void* memset_word(void* s, int32_t c, uint32_t n);
*   Inputs: void* s = pointer to memory
*			int32_t c = value to set memory to
*			uint32_t n = number of bytes to set
*   Return Value: new string
*	Function: set lower 16 bits of n consecutive memory locations of pointer s to value c
*/

/* Optimized memset_word */
void*
memset_word(void* s, int32_t c, uint32_t n)
{
	asm volatile("                  \n\
			movw    %%ds, %%dx      \n\
			movw    %%dx, %%es      \n\
			cld                     \n\
			rep     stosw           \n\
			"
			:
			: "a"(c), "D"(s), "c"(n)
			: "edx", "memory", "cc"
			);

	return s;
}

/*
* void* memset_dword(void* s, int32_t c, uint32_t n);
*   Inputs: void* s = pointer to memory
*			int32_t c = value to set memory to
*			uint32_t n = number of bytes to set
*   Return Value: new string
*	Function: set n consecutive memory locations of pointer s to value c
*/

void*
memset_dword(void* s, int32_t c, uint32_t n)
{
	asm volatile("                  \n\
			movw    %%ds, %%dx      \n\
			movw    %%dx, %%es      \n\
			cld                     \n\
			rep     stosl           \n\
			"
			:
			: "a"(c), "D"(s), "c"(n)
			: "edx", "memory", "cc"
			);

	return s;
}

/*
* void* memcpy(void* dest, const void* src, uint32_t n);
*   Inputs: void* dest = destination of copy
*			const void* src = source of copy
*			uint32_t n = number of byets to copy
*   Return Value: pointer to dest
*	Function: copy n bytes of src to dest
*/

void*
memcpy(void* dest, const void* src, uint32_t n)
{
	asm volatile("                  \n\
			.memcpy_top:            \n\
			testl   %%ecx, %%ecx    \n\
			jz      .memcpy_done    \n\
			testl   $0x3, %%edi     \n\
			jz      .memcpy_aligned \n\
			movb    (%%esi), %%al   \n\
			movb    %%al, (%%edi)   \n\
			addl    $1, %%edi       \n\
			addl    $1, %%esi       \n\
			subl    $1, %%ecx       \n\
			jmp     .memcpy_top     \n\
			.memcpy_aligned:        \n\
			movw    %%ds, %%dx      \n\
			movw    %%dx, %%es      \n\
			movl    %%ecx, %%edx    \n\
			shrl    $2, %%ecx       \n\
			andl    $0x3, %%edx     \n\
			cld                     \n\
			rep     movsl           \n\
			.memcpy_bottom:         \n\
			testl   %%edx, %%edx    \n\
			jz      .memcpy_done    \n\
			movb    (%%esi), %%al   \n\
			movb    %%al, (%%edi)   \n\
			addl    $1, %%edi       \n\
			addl    $1, %%esi       \n\
			subl    $1, %%edx       \n\
			jmp     .memcpy_bottom  \n\
			.memcpy_done:           \n\
			"
			:
			: "S"(src), "D"(dest), "c"(n)
			: "eax", "edx", "memory", "cc"
			);

	return dest;
}

/*
* void* memmove(void* dest, const void* src, uint32_t n);
*   Inputs: void* dest = destination of move
*			const void* src = source of move
*			uint32_t n = number of byets to move
*   Return Value: pointer to dest
*	Function: move n bytes of src to dest
*/

/* Optimized memmove (used for overlapping memory areas) */
void*
memmove(void* dest, const void* src, uint32_t n)
{
	asm volatile("                  \n\
			movw    %%ds, %%dx      \n\
			movw    %%dx, %%es      \n\
			cld                     \n\
			cmp     %%edi, %%esi    \n\
			jae     .memmove_go     \n\
			leal    -1(%%esi, %%ecx), %%esi    \n\
			leal    -1(%%edi, %%ecx), %%edi    \n\
			std                     \n\
			.memmove_go:            \n\
			rep     movsb           \n\
			"
			:
			: "D"(dest), "S"(src), "c"(n)
			: "edx", "memory", "cc"
			);

	return dest;
}

/*
* int32_t strncmp(const int8_t* s1, const int8_t* s2, uint32_t n)
*   Inputs: const int8_t* s1 = first string to compare
*			const int8_t* s2 = second string to compare
*			uint32_t n = number of bytes to compare
*	Return Value: A zero value indicates that the characters compared 
*					in both strings form the same string.
*				A value greater than zero indicates that the first 
*					character that does not match has a greater value 
*					in str1 than in str2; And a value less than zero 
*					indicates the opposite.
*	Function: compares string 1 and string 2 for equality
*/

int32_t
strncmp(const int8_t* s1, const int8_t* s2, uint32_t n)
{
	int32_t i;
	for(i=0; i<n; i++) {
		if( (s1[i] != s2[i]) ||
				(s1[i] == '\0') /* || s2[i] == '\0' */ ) {

			/* The s2[i] == '\0' is unnecessary because of the short-circuit
			 * semantics of 'if' expressions in C.  If the first expression
			 * (s1[i] != s2[i]) evaluates to false, that is, if s1[i] ==
			 * s2[i], then we only need to test either s1[i] or s2[i] for
			 * '\0', since we know they are equal. */

			return s1[i] - s2[i];
		}
	}
	return 0;
}


/* strncmp2
 * Description: compare two strings. 
 * INPUTS:	
	  const int8_t* s1 -- string 1
	  const int8_t* s2 -- string 2
	  uint32_t n  --comparasion length.
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: none
 */
int32_t
strncmp2(const int8_t* s1, const int8_t* s2, uint32_t n)
{
	int32_t i;
	for(i=0; i<n; i++) {
		if ((s1[i] == '\0' && s2[i] == '\n')||(s1[i] == '\n' && s2[i] == '\0'))
			return 0;
		if( (s1[i] != s2[i]) ||
				(s1[i] == '\0') /* || s2[i] == '\0' */ ) {

			/* The s2[i] == '\0' is unnecessary because of the short-circuit
			 * semantics of 'if' expressions in C.  If the first expression
			 * (s1[i] != s2[i]) evaluates to false, that is, if s1[i] ==
			 * s2[i], then we only need to test either s1[i] or s2[i] for
			 * '\0', since we know they are equal. */

			return s1[i] - s2[i];
		}
	}
	return 0;
}

/*
* int8_t* strcpy(int8_t* dest, const int8_t* src)
*   Inputs: int8_t* dest = destination string of copy
*			const int8_t* src = source string of copy
*   Return Value: pointer to dest
*	Function: copy the source string into the destination string
*/

int8_t*
strcpy(int8_t* dest, const int8_t* src)
{
	int32_t i=0;
	while(src[i] != '\0') {
		dest[i] = src[i];
		i++;
	}

	dest[i] = '\0';
	return dest;
}

/*
* int8_t* strcpy(int8_t* dest, const int8_t* src, uint32_t n)
*   Inputs: int8_t* dest = destination string of copy
*			const int8_t* src = source string of copy
*			uint32_t n = number of bytes to copy
*   Return Value: pointer to dest
*	Function: copy n bytes of the source string into the destination string
*/

int8_t*
strncpy(int8_t* dest, const int8_t* src, uint32_t n)
{
	int32_t i=0;
	while(src[i] != '\0' && i < n) {
		dest[i] = src[i];
		i++;
	}

	while(i < n) {
		dest[i] = '\0';
		i++;
	}

	return dest;
}


/*
* void test_interrupts(void)
    Description: test interrupts
*   Inputs: void
*   Return Value: void
*	Function: increments video memory. To be used to test rtc
    side effects: none
*/

void
test_interrupts(void)
{
	
	int32_t i;
	for (i=0; i < NUM_ROWS*NUM_COLS; i++) {
		video_mem[i<<1]++;
	}
}

volatile static int vm_status = 0;

#define FREE 0
#define IN_PROCESS 1
#define PENDING 2



/* display_video
 * Description: display the video according to its terminal number. 
 * INPUTS:	uint32_t terminal_num -- teminal number. 
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: none
 */
void display_video(uint32_t terminal_num)
{
	

	switch (terminal_num)
	{
		case 0:
			video_mem = (char *)VIDEO_T0;
			break;
		case 1:
			video_mem = (char *)VIDEO_T1;
			break;

		case 2:
			video_mem = (char *)VIDEO_T2;
			break;
		case -1:
			break;		//when called by buffer func


		default:
			video_mem = (char *)VIDEO;	//this should not happened, just for error handling	
			test_interrupts();
			break;
			

	}

	if (terminal_num == cur_terminal || terminal_num == -1)
	{

		//this probably contains some race condition but I decided not to solve it right now

		if (vm_status == FREE)
			vm_status = IN_PROCESS;
		else if (vm_status == IN_PROCESS)
		{
			vm_status = PENDING;
			
			return;
		}
		else if (vm_status == PENDING)
			return;
		
		memcpy((void *)VIDEO, video_mem, VIDEO_SIZE);
		
		if (vm_status == IN_PROCESS)
			vm_status = FREE;
		else if (vm_status == PENDING)
		{
		
			vm_status = FREE;
			display_video(cur_terminal);
			return;
		
		}else
		{
		
			//should not happen
		
		}
		
		set_cursor(screen_y[cur_terminal], screen_x[cur_terminal]);
		

	}
	

}
/* set_vm_stutus
 * Description: set virtual memory status to free.  
 * INPUTS:	None
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: none
 */
void set_vm_stutus()
{

	vm_status = FREE;

}
