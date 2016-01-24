/* This ECE391OS project is an in class project written by
*Hongchuan Li, Meng Gao, Junqing Deng, Han Jiang in UIUC (spring 2014)
*The current file is wriiten by Hongchuan Li and modified by others
*Students who are taking ECE391 should AVOID copying this file
*/

#include "types.h"
#include "filesys.h"
#include "rtc.h"
#include "lbuffer.h"
#include "lib.h"
#include "pcb.h"
#include "kmalloc.h"
#include "x86_desc.h"
#include "sig.h"
#define EPENDSIG -8
static const int FIVE_BITS = 31;

/*
 * stdin_read
 * Description: standard read 
 * INPUTS:	
		 int32_t fd -- file descriptor
		 void* buf -- buffer address
		 int32_t nbytes -- number of bytes
		 pcb_t* mypcb -- Process control block
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: None
 */
extern int32_t stdin_read(int32_t fd, void* buf, int32_t nbytes, pcb_t* mypcb)			
{

	if (fd == 0)
	{

		int i;
		while(1)
		{
		if (check_signal((uint32_t)mypcb)){
			return EPENDSIG;
		}
			if (cur_terminal == kernel_pid[get_cur_pid()].terminal_num)
			{
				for (i=0; i<nbytes; i++)
				{
					((int8_t*)buf)[i] = c_buffer[terminal_index].data[i];
					if (c_buffer[terminal_index].data[i] == '\0')
						break;
					else if (c_buffer[terminal_index].data[i] == '\n')
					{
						clear_c_buffer();
						return i;
					}
		
				}
			}
		
		}
	
	}
	return -1;

}

/*
 * stdin_write
 * Description: standard write 
 * INPUTS:	
		 int32_t fd -- file descriptor
		 void* buf -- buffer address
		 int32_t nbytes -- number of bytes
		 pcb_t* mypcb -- Process control block
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: None
 */		
extern int32_t stdin_write(int32_t fd, const void* buf, int32_t nbytes, pcb_t* mypcb)
{return -1;}

/*
 * stdin_open
 * Description: open standard input
 * INPUTS:	
		 const uint8_t* filename -- 
		 pcb_t* mypcb -- 
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: None
 */
extern int32_t stdin_open(const uint8_t* filename, pcb_t* mypcb)							//i believe we dont actually need the open function here
{return 0;}

/*
 * stdin_close
 * Description: close standard input
 * INPUTS:	
		 int32_t fd -- file descriptor
		 pcb_t* mypcb -- Process control block
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: None
 */
extern int32_t stdin_close(int32_t fd, pcb_t* mypcb)										//this function do nothing
{return -1;}


/*
 * stdout_read
 * Description: read standard output
 * INPUTS:	
		 int32_t fd -- file descriptor
		 void* buf -- buffer address
		 int32_t nbytes -- number of bytes
		 pcb_t* mypcb -- Process control block
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: None
 */
extern int32_t stdout_read(int32_t fd, void* buf, int32_t nbytes, pcb_t* mypcb)
{return -1;}

/*
 * stdout_write
 * Description: write standard output
 * INPUTS:	
		 int32_t fd -- file descriptor
		 void* buf -- buffer address
		 int32_t nbytes -- number of bytes
		 pcb_t* mypcb -- Process control block
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: None
 */
extern int32_t stdout_write(int32_t fd, const void* buf, int32_t nbytes, pcb_t* mypcb)	//this function do nothing
{
	if (fd == 1)
	{
		int i;
		uint32_t flag;
		//int8_t local[2000];
		int8_t * local = (int8_t *)r_kmalloc(nbytes+1);
		//strcpy(local, (int8_t *)buf);
		memcpy(local, buf, nbytes);
		local[nbytes] = '\0';
		//((int8_t *)buf)[nbytes-1] = '\0';

		cli_and_save(flag);

		//printf("%s", local);
		if (modeX_enabled == 0)
		{
		if (check_signal((uint32_t)mypcb)){
			return EPENDSIG;
		}
			for (i=0;i<nbytes;i++){
				//if (local[i]!='\0')
					putc(local[i]);
			}
		}else
		{
		
			puts(local);
		
		}

		restore_flags(flag);
		mypcb->file_array[fd].file_position = nbytes;
		r_kfree((uint32_t)local);
		
		return nbytes;
	
	}
	return -1;

}
/*
 * stdout_open
 * Description: open standard output
 * INPUTS:	
         const uint8_t* filename -- file name
		 pcb_t* mypcb -- Process control block
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: None
 */
extern int32_t stdout_open(const uint8_t* filename, pcb_t* mypcb)							//this function do nothing
{return 0;}


/*
 * stdout_close
 * Description: close standard output
 * INPUTS:	
		 int32_t fd -- file descriptor
		 pcb_t* mypcb -- Process control block
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: None
 */
extern int32_t stdout_close(int32_t fd, pcb_t* mypcb)										//this function do nothing
{return -1;}


/*
 * filesys_read
 * Description: read file system
 * INPUTS:	
		 int32_t fd -- file descriptor
		 void* buf -- buffer address
		 int32_t nbytes -- number of bytes
		 pcb_t* mypcb -- Process control block 
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: None
 */
extern int32_t filesys_read(int32_t fd, void* buf, int32_t nbytes, pcb_t* mypcb)
{
	uint32_t myInode = (uint32_t)(mypcb->file_array[fd].inode_pt);
	uint32_t offset = mypcb->file_array[fd].file_position;
	if (offset == -1)
		return 0;
	uint32_t ret_keep;
	
	ret_keep = read_data(myInode, offset, (uint8_t *)buf, nbytes);
	mypcb->file_array[fd].file_position += ret_keep;
	
	/*
	if (ret_keep == 0)
	{
	
		mypcb->file_array[fd].file_position = -1;
	
	}
	else
		mypcb->file_array[fd].file_position += nbytes;
	*/
	
	return ret_keep;

}

/*
 * filesys_write
 * Description: write to file system
 * INPUTS:	
		 int32_t fd -- file descriptor
		 void* buf -- buffer address
		 int32_t nbytes -- number of bytes
		 pcb_t* mypcb -- Process control block
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: None
 */
extern int32_t filesys_write(int32_t fd, const void* buf, int32_t nbytes, pcb_t* mypcb)
{

	return -1;

}


/*
 * stdin_read
 * Description: read standard input
 * INPUTS:	
		 const uint8_t* filename -- 
		 pcb_t* mypcb -- 
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: None
 */
extern int32_t filesys_open(const uint8_t* filename, pcb_t* mypcb)
{

	return 0;

}

/*
 * filesys_close
 * Description: close file system
 * INPUTS:	
		 int32_t fd -- file descriptor
		 pcb_t* mypcb -- Process control block
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: None
 */
extern int32_t filesys_close(int32_t fd, pcb_t* mypcb)
{
	
	return 0;

}

/*
 * dir_read
 * Description: read directory
 * INPUTS:	
		 int32_t fd -- file descriptor
		 void* buf -- buffer address
		 int32_t nbytes -- number of bytes
		 pcb_t* mypcb -- Process control block
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: None
 */
extern int32_t dir_read(int32_t fd, void* buf, int32_t nbytes, pcb_t* mypcb)
{
	uint32_t offset = mypcb->file_array[fd].file_position;

	boot_block_t* bb = (boot_block_t*)mods_addrs; 

	uint32_t limit = bb->num_dir_entry;

	dentry_t dentry;
	static uint32_t read_dir_falg = 0;

	if (read_dir_falg == 1)
	{
		read_dir_falg = 0;
		return 0;
	
	}

	read_dentry_by_index(offset, &dentry);
	offset++;
	if (offset == limit)
	{
		offset %= limit;
		read_dir_falg = 1;
	}
	mypcb->file_array[fd].file_position = offset;
	uint32_t n = nbytes;
	if (nbytes > FIVE_BITS)
		n = FIVE_BITS;

	// strncpy((int8_t *)buf, dentry.file_name, n);
	// ((int8_t *)buf)[n+1] = '\n';
	strcpy((int8_t *)buf, dentry.file_name);
	
	return strlen((int8_t *)buf);

	return n;

}

/*
 * dir_write
 * Description: write directory 
 * INPUTS:	
		 int32_t fd -- file descriptor
		 void* buf -- buffer address
		 int32_t nbytes -- number of bytes
		 pcb_t* mypcb -- Process control block
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: None
 */
extern int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes, pcb_t* mypcb)
{

	return -1;

}

/*
 * dir_open
 * Description: open directory
 * INPUTS:	
         const uint8_t* filename -- file name
		 pcb_t* mypcb -- Process control block
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: None
 */
extern int32_t dir_open(const uint8_t* filename, pcb_t* mypcb)
{

	return 0;

}

/*
 * dir_close
 * Description: close directory
 * INPUTS:	
		 int32_t fd -- file descriptor
		 pcb_t* mypcb -- Process control block
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: None
 */
extern int32_t dir_close(int32_t fd, pcb_t* mypcb)
{

	return 0;

}

/*
 * sys_rtc_read
 * Description: read system RTC
 * INPUTS:	
		 int32_t fd -- file descriptor
		 void* buf -- buffer address
		 int32_t nbytes -- number of bytes
		 pcb_t* mypcb -- Process control block
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: None
 */
extern int32_t sys_rtc_read(int32_t fd, void* buf, int32_t nbytes, pcb_t* mypcb)
{
	
	return rtc_read(mypcb, fd, buf, nbytes);

}

/*
 * sys_rtc_write
 * Description:  write system RTC
 * INPUTS:	
		 int32_t fd -- file descriptor
		 void* buf -- buffer address
		 int32_t nbytes -- number of bytes
		 pcb_t* mypcb -- Process control block
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: None
 */
extern int32_t sys_rtc_write(int32_t fd, const void* buf, int32_t nbytes, pcb_t* mypcb)
{
	return rtc_write(fd, buf, nbytes);

}

/*
 * sys_rtc_open
 * Description:  open system RTC
 * INPUTS:	
         const uint8_t* filename -- file name
		 pcb_t* mypcb -- Process control block
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: None
 */
extern int32_t sys_rtc_open(const uint8_t* filename, pcb_t* mypcb)
{
	return rtc_open(filename);

}

/*
 * sys_rtc_close
 * Description: close system RTC
 * INPUTS:	
		 int32_t fd -- file descriptor
		 pcb_t* mypcb -- Process control block
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: None
 */
extern int32_t sys_rtc_close(int32_t fd, pcb_t* mypcb)
{
	return rtc_close(fd);
}







