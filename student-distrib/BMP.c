/* This ECE391OS project is an in class project written by
 *Hongchuan Li, Meng Gao, Junqing Deng, Han Jiang in UIUC (spring 2014)
 *The current file is wriiten by Hongchuan Li
 *Students who are taking ECE391 should AVOID copying this file
 */

#include "BMP.h"
#include "modex.h"
#include "kmalloc.h"
#include "lib.h"
#include "lbuffer.h"
#include "sched.h"
#define MX_HEIGHT 182
#define MX_WIDTH 320
#define STEP_LENGTH  3
static const int NUM_CHARS = 400;
static const int LETTER_WIDTH = 4;
static const int SCALE_2 = 2;

/*extern int enter_modex_u()
{

	disable_sched();
	set_mode_X();
	update_statusBar(0,0,0);
	//display_pic = 1;

}


extern int quit_modex_u()
{

	clear_mode_X();
	init_sched();
	clear_buffer();
	clear();

	//display_pic = 0;
	//r_kfree(buf);
	restore_palette();

}
*/

/*
 * handle_bmp
 * Description: open a bmp picture. 
 * INPUTS:
        BMP256_t * bmp -- the bmp picture address.   
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: None
 */
extern int handle_bmp(BMP256_t * bmp)
{
	static int quit = 0;
	

	int ret = update_palette(bmp);
	if (ret == -1)
	{
		restore_palette();
		return ret;
	
	}

	disable_sched();
	set_mode_X();
	update_statusBar(0,0,0);
	display_pic = 1;

	//int size = bmp->InfoHeader.size;
	int width = bmp->InfoHeader.width;
	int height = bmp->InfoHeader.height;
	int line_size = 0;
	if (width % LETTER_WIDTH == 0)
	{
	
		line_size = width;
	
	}else
	{
	
		line_size = ((width / LETTER_WIDTH) + 1) * LETTER_WIDTH;
	
	}
	int start = (int)bmp + (int)(bmp->BMP_header.DataOffset);
	//unsigned char * buf = (unsigned char *)r_kmalloc(sizeof(unsigned char)*width);
	int i;
	int x = 0;
	int y = 0;
	int pos;
	int scale = 1;
	while(!quit)
	{
		
		pos = start -  (y)*line_size;
		for (i = height-1; i >=0; i--)
		{
			
			bmp_horiz_line(x, i, (unsigned char *)pos, scale, width);
			pos += line_size;

		}

	
		show_screen();
		//x++;
		cli();
		if (display_handshake == 1)
		{
		
			display_handshake = 0;
			switch (display_cmd)
			{
				case 'w':
					
					y -= STEP_LENGTH;
					if (y<=0)
						y = 0;
					
					break;
				case 's':
					

					y += STEP_LENGTH;
					if (y >= height - MX_HEIGHT)
						y = height - MX_HEIGHT;
					break;
				case 'a':
					
					x -= STEP_LENGTH;
					if (x <= 0)
						x = 0;
					break;
				case 'd':
					
					x += STEP_LENGTH;
					if (x >= width - MX_WIDTH)
						x =  width - MX_WIDTH;
					break;
				default:
					quit = 1;
					break;
			}
		
		}
		sti();

	}
	clear_mode_X();
	init_sched();
	clear_buffer();
	clear();

	display_pic = 0;
	//r_kfree(buf);
	restore_palette();
	return 0;

}



/*
 * handle_bmpK
 * Description: open a bmp picture. 
 * INPUTS:
        BMP256_t * bmp -- the bmp picture address.   
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: None
 */
extern int handle_bmpK(BMP256_t * bmp)
{
	int quit = 0;
	

	int ret = update_palette(bmp);
	if (ret == -1)
	{
		restore_palette();
		return ret;
	
	}

	display_pic = 1;
	

	dis_status_bar();




	//int size = bmp->InfoHeader.size;
	int width = bmp->InfoHeader.width;
	int height = bmp->InfoHeader.height;
	int line_size = 0;
	if (width % LETTER_WIDTH == 0)
	{
	
		line_size = width;
	
	}else
	{
	
		line_size = ((width / LETTER_WIDTH) + 1) * LETTER_WIDTH;
	
	}
	int start = (int)bmp + (int)(bmp->BMP_header.DataOffset);
	//unsigned char * buf = (unsigned char *)r_kmalloc(sizeof(unsigned char)*width);
	int i;
	int x = 0;
	int y = 0;
	int scale = SCALE_2;
	unsigned char blank[NUM_CHARS];
	memset((void *) blank, 0, NUM_CHARS);
	int pos;
	update_statusBar3(x, y, scale);
	while(!quit)
	{
		
		pos = start -  (y*scale)*line_size;
		


		for (i = height/scale-1; i >=0; i--)
		{
			
			bmp_horiz_line(x*scale, i, (unsigned char *)pos, scale, line_size);
			pos += line_size*scale;

		}
		
		

	
		show_screen();
		//x++;
		cli();
		if (display_handshake == 1)
		{
		
			display_handshake = 0;
			switch (display_cmd)
			{
				case 'w':
					
					y -= STEP_LENGTH;
					if (y<=0)
						y = 0;
					update_statusBar3(x, y, scale);
					break;
				case 's':
					

					y += STEP_LENGTH;
					if (y >= height/scale - MX_HEIGHT)
						y = height/scale - MX_HEIGHT;
					if (y<=0)
						y = 0;
					update_statusBar3(x, y, scale);
					break;
				case 'a':
					
					x -= STEP_LENGTH;
					if (x <= 0)
						x = 0;
					update_statusBar3(x, y, scale);
					break;
				case 'd':
					
					x += STEP_LENGTH;
					if (x >= width/scale - MX_WIDTH)
						x =  width/scale - MX_WIDTH;
					if (x<=0)
						x = 0;
					update_statusBar3(x, y, scale);
					break;

				case '.':
				scale /= SCALE_2;
					if (scale <= 1)
						scale = 1;
					x = 0;
					y = 0;
					
					update_statusBar3(x, y, scale);

					break;
				case ',':
				scale *= SCALE_2;
					if (scale >= 8)
						scale = 8;
					
					x = 0;
					y = 0;
					
					for (i = MX_HEIGHT; i >=0; i--)
					{
			
						bmp_horiz_line(0, i, blank, 1, MX_WIDTH);
						

					}
					update_statusBar3(x, y, scale);

					break;
				default:
					quit = 1;
					break;
			}
		
		}
		sti();

	}
	
	update_statusBar(0, 0, 0);
	
	display_pic = 0;
	clear();
	restore_palette();
	en_status_bar();
	return 0;

}

/*
 * start_modex
 * Description: start a modex process 
 * INPUTS:
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: None
 */
extern void start_modex()
{

	disable_sched();
	set_mode_X();
	update_statusBar(0,0,0);
	//display_pic = 1;


}

/*
 * end_modex
 * Description: terminate the modex process
 * INPUTS:
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: None
 */
extern void end_modex()
{

	
	init_sched();
	clear_buffer();
	clear();
	clear_mode_X();
	restore_palette();
	//display_pic = 0;
	//r_kfree(buf);
	


}
