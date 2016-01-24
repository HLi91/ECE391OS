/* This ECE391OS project is an in class project written by
*Hongchuan Li, Meng Gao, Junqing Deng, Han Jiang in UIUC (spring 2014)
*The current file is wriiten by Junqing Deng Han Jiang
*Students who are taking ECE391 should AVOID copying this file
*/

#include "keyboard.h"
#include "lib.h"

#include "idt.h"
#define DEBUG 0
#include "lbuffer.h"
#include "syscall_wrap.h"
#include "gen_asm.h"

#define CASE_DISTANCE 32
#define KEYBOARD_ARRAY_SIZE 128
static const int INDEX_TWO = 2;
static int func_key = NONE_MAKE;
static int caps_lock = 0;
/* keyboard key map.
 * credit goes to: http://www.osdever.net/bkerndev/Docs/keyboard.htm
 */
unsigned char kbdus_normal[KEYBOARD_ARRAY_SIZE] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
	UP,	/* Up Arrow */
    0,	/* Page Up */
  '-',
  LEFT,	/* Left Arrow */
    0,
	RIGHT,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
	DOWN,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

/*
scancode
29 ctrl
14 backspace
0 esc?



*/

//to accommodate the high characters on keys. 
//if there is only one char on the key, the character will be the same as the normal 
//array above. 
unsigned char kbdus_high[KEYBOARD_ARRAY_SIZE] =
{
    0,  27,  '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
    '\b', /* Backspace */
  '\t',     /* Tab */
  'q', 'w', 'e', 'r', /* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '{', '}', '\n', /* Enter key */
    0,      /* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ':', /* 39 */
 '\"', '~',   0,    /* Left shift */
 '|', 'z', 'x', 'c', 'v', 'b', 'n',      /* 49 */
  'm', '<', '>', '?',   0,        /* Right shift */
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
	UP,  /* Up Arrow */
    0,  /* Page Up */
  '-',
  LEFT,  /* Left Arrow */
    0,
	RIGHT,  /* Right Arrow */
  '+',
    0,  /* 79 - End key*/
	DOWN,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};

/* toggleLetter
 * Description: if the input character is a upper case/lower case letter, 
 * then change it to lower case/upper case letter. 
 * INPUTS:  unsigned char c, an ASCII character. 
 * OUTPUTS: the toggled uppercase/lowercase character. 
 * RETURN: None
 * SIDE EFFECTS: None
 */

unsigned char toggleLetter(unsigned char c){   
    if(c >='A'&& c <='Z')
       return c + CASE_DISTANCE;
    else if(c >='a'&& c <='z')
       return c - CASE_DISTANCE;
    else 
       return c;
}

/* toggleLetter
 * Description: if the input character is a upper case/lower case letter, 
 * then change it to lower case/upper case letter. 
 * INPUTS:  unsigned char c, an ASCII character. 
 * OUTPUTS: the toggled uppercase/lowercase character. 
 * RETURN: None
 * SIDE EFFECTS: None
 */
void check_func_key(unsigned char scancode)
{
  switch (scancode){
  case(LEFT_SHIFT_DN): 
  case(RIGHT_SHIFT_DN): 
    func_key = SHIFT_MAKE; 
    break;

  case(CTRL_DN):
    func_key = CTRL_MAKE;
    break;
  case(ALT_DN):
    func_key = ALT_MAKE;
    break;  
  case(CAPS_LOCK):
    caps_lock = caps_lock ^ 1; // toggle caps lock;
    break;

  case(LEFT_SHIFT_UP):
  case(RIGHT_SHIFT_UP):
  case(CTRL_UP):
  case(ALT_UP):
    func_key = NONE_MAKE; 
    break;

  default:
    break;
  }

}

/* keyboard_handler
 * Description: get keyboard input & print on screen
 * INPUTS:	None
 * OUTPUTS:	key pressed on screen
 * RETURN: None
 * SIDE EFFECTS: get keyboard input & print on screen
 */
void keyboard_handler(regs * r)
{
  unsigned char new_char = '\0'; 
	unsigned char scancode;
	
	/* fetch scancode from keyboard */
	scancode = inb(KEYBOARD_PORT);
	check_func_key(scancode);
	// if a key is just released (top bit = 1) 
	if (!(scancode & KEY_RELEASE))
	{    
		
		if(func_key == SHIFT_MAKE)
		{
			new_char = kbdus_high[scancode]; 
		}else{
			new_char = kbdus_normal[scancode];
		}


		if (display_pic == 1)
		{
		
			display_cmd = new_char;
			display_handshake = 1;

		
			return;
		}

	  
        // check capslock and shift key to decide if the letter case should be toggled.
		if((caps_lock && (func_key != SHIFT_MAKE))||(!caps_lock && (func_key == SHIFT_MAKE)))
		{		  
		  new_char = toggleLetter(new_char);
		}
	    if(func_key == CTRL_MAKE && (new_char == 'C' || new_char == 'c')){
			if (cur_terminal == kernel_pid[get_cur_pid()].terminal_num)
			{
				printf("program terminated by keyboard\n");
				//clear_buffer();
				set_vm_stutus();
				exception_block = 0;
				halt();
				
			}else 
			{
			
				//printf("set pending_halt\n");
				pending_halt = 1;

			
			
			}
			return;
			
		}
		//cli();
		if(func_key == ALT_MAKE && scancode == FUNC_KEY_1){
			//printf("Alt + F1 Pressed\n");
			
			cur_terminal = 0;
			terminal_index = 0;
			display_video(cur_terminal);
			//sti();
		    return;
		}
		if(func_key == ALT_MAKE && scancode == FUNC_KEY_2){
			//printf("Alt + F2 Pressed\n");
			
			cur_terminal = 1;
			terminal_index = 1;
			display_video(cur_terminal);
			//sti();
		    return;
		}
		if(func_key == ALT_MAKE && scancode == FUNC_KEY_3){
			//printf("Alt + F3 Pressed\n");
			
			cur_terminal = INDEX_TWO;
			terminal_index = INDEX_TWO;
			display_video(cur_terminal);
			//sti();
		    return;
		}
		//if(func_key == ALT_MAKE && scancode == FUNC_KEY_4){
		//	//printf("Alt + F3 Pressed\n");
		//	sr_paging();
		//	graphic_mode();
		//	sr_paging();
		//	
		//	//sti();
		//    return;
		//}


		//sti();


        //handle CTRL + L to clear screen. 
		if(func_key == CTRL_MAKE && (new_char == 'L' || new_char == 'l')){
		  clear();
		  clear_buffer();
		  exception_block = 0;
		  set_cursor(0,0);
		  return;
		}
		if(func_key == CTRL_MAKE && (new_char == 'X' || new_char == 'x')){
			if (cur_terminal == kernel_pid[get_cur_pid()].terminal_num)
			{
				//printf("program terminated by keyboard\n");
				//set_vm_stutus();
				//exception_block = 0;
				write_signal(r->esp, INDEX_TWO);
				
			}else 
			{
			
				pending_sigint = 1;

			}
				
		  return;
		}
		if(new_char != '\0' && exception_block ==0)
			handle_key_input((uint8_t)new_char);
	}

}





/* keyboard_install
 * Description: initializes the keyboard.
 * INPUTS:	None
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: enable IR1, and install handler
 */
void keyboard_install()
{

	buff_init();
	irq_install_handler(IDT_OFFSET+IRQ_KEYBOARD, (void *)keyboard_handler);
	enable_irq(IRQ_KEYBOARD);


}

