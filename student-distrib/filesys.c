/* This ECE391OS project is an in class project written by
*Hongchuan Li, Meng Gao, Junqing Deng, Han Jiang in UIUC (spring 2014)
*The current file is wriiten by Hongchuan Li and modified by others
*Students who are taking ECE391 should AVOID copying this file
*/

#include "filesys.h"
#include "x86_desc.h"
#include "types.h"
#include "lib.h"




/* read_dentry_by_name
 * Description: read directory entry by given name, save the entry at dentry. 
 * INPUTS:	const uint8_t* fname -- the file's name. 
            dentry_t* dentry -- the address where the found entry is going to be saved to.
 * OUTPUTS:	None
 * RETURN: -1 -- failure, non-existent file
           0 -- success
 * SIDE EFFECTS: pass the file name, file type, inode number to input "dentry"
 */
 

extern int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry){
	
	
	int i;
	//mods_addrs is the file module's starting address
	//which is also the starting address of the boot block
	boot_block_t* bb = (boot_block_t*)mods_addrs;  
	if (fname[0] == '\0')
		return RET_ERROR;

	for (i = 0; i<MAX_DIR_NUM; i++)
	{
	    //compare each directory entry name with the fname passed in. 
		if (strncmp((int8_t *)fname, bb->dir_entry[i].file_name, MAX_FILE_NAME_LENGTH) == 0)
		{
			//check if the search reaches the largest number of nodes.
			if (bb->dir_entry[i].index_node > bb->num_inode)
				return RET_ERROR;
			
			//if found, copy the name, type, and inode number to the dentry and return. 
			strncpy(dentry->file_name, bb->dir_entry[i].file_name, MAX_FILE_NAME_LENGTH);
			dentry->file_type = bb->dir_entry[i].file_type;
			dentry->index_node = bb->dir_entry[i].index_node;
			return 0;
		
		}
	
	}
	return RET_ERROR;
}



/* read_dentry_by_index
 * Description: read directory entry by given index, save the entry at dentry. 
 * INPUTS:  uint32_t index -- the file's index that we are finding. 
           dentry_t* dentry -- the address where the found entry is going to be saved to.
 * OUTPUTS:	None
 * RETURN: -1 - failure, non-existent file or invalid index
 *          0 - success
 * SIDE EFFECTS: pass the file name, file type, inode number to DENTRY
 */
 

extern int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry)
{
	//mods_addrs is the file module's starting address
	//which is also the starting address of the boot block	
	boot_block_t* bb = (boot_block_t*)mods_addrs;
	
	//check the validity of the index
	if (index > MAX_DIR_NUM || index <0)
		return RET_ERROR;
		
	//check if the entry is valid 	
	if (bb->dir_entry[index].index_node > bb->num_inode)
		return RET_ERROR;
	
	//if found, copy the name, type, and inode number to the dentry and return. 
	strncpy(dentry->file_name, bb->dir_entry[index].file_name, MAX_FILE_NAME_LENGTH);
	dentry->file_type = bb->dir_entry[index].file_type;
	dentry->index_node = bb->dir_entry[index].index_node;
	return 0;
}

/* read_data
 * Description: read up to LENGTH byte starting from OFFSET in the file with
 *              inode number INODE
 * INPUTS:	
	 uint32_t inode -- the index node that contains the data. 
	 uint32_t offset -- the offset in byte of from the beginning of the first block 
	 uint8_t* buf -- the address that the data will be saved to
	 uint32_t length-- the number of bytes that will be read from the file. 
 * OUTPUTS:	None
 * RETURN: -1, RET_ERROR - failure, invalid inode # or bad data block number is found
 *          0, E0F - end of file reached
 *          other - return the number of bytes read on success
 * SIDE EFFECTS: none
 */
 
extern int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length)
{
	uint32_t i;
	boot_block_t* bb = (boot_block_t*)mods_addrs;
	if (bb->num_inode < inode)
		return RET_ERROR;
	inode_t * myNode = findNode(inode);
	uint32_t myLength = myNode->length;
	if (offset > myLength)
		return EOF;
	
	uint32_t stop = offset+length;
	
    //if stop position exceeds the node length, then copy the data from the offset to the end of the file. 
	if (stop >= myNode->length)
	{
     	
		uint32_t sBlockNumInd = offset/KB4;
		uint32_t sBlockOff = offset%KB4;
		uint32_t eBlockNumInd = myNode->length/KB4;
		uint32_t eBlockOff = myNode->length%KB4;
		uint32_t sBlockNum;
		
		uint32_t bufInd = 0;
		
		//check if the starting block index == the ending block index.
		
		if (sBlockNumInd == eBlockNumInd)
		{	
			sBlockNum = myNode->data_block[sBlockNumInd];
			
			//if the starting block index > the number of blocks in the node, or the starting block number < 0,
			// return error. 
			if (sBlockNum >bb->num_data_block || sBlockNum<0)
				return RET_ERROR;
				
			//copy the data of the last block to the buffer.  
			int num_bytes = eBlockOff-sBlockOff;
			copyData(sBlockNum, buf, bufInd, sBlockOff, num_bytes);
			return num_bytes;
		
		}
		
	
		sBlockNum = myNode->data_block[sBlockNumInd];
		
		//if the starting block index > the number of blocks in the node, or the starting block number < 0,
		// return error. 		
		if (sBlockNum >bb->num_data_block || sBlockNum<0)
			return RET_ERROR;
		copyData(sBlockNum, buf, bufInd, sBlockOff, KB4-sBlockOff);
		bufInd+=(KB4-sBlockOff);
		
		//Now, it has multiple blocks. Go through each block, copy data into the buffer.
        int num_full_blocks = (eBlockNumInd-sBlockNumInd-1);	
		for (i = 0; i< num_full_blocks; i++)
		{
			uint32_t curBlockNum = myNode->data_block[sBlockNumInd+i+1];
			if (curBlockNum >bb->num_data_block || curBlockNum<0)
				return RET_ERROR;
			copyData(curBlockNum, buf, bufInd, 0, KB4);
			
			//buffer offset increases by 4kb.  
			bufInd+=KB4;
		
		}
		uint32_t curBlockNum = myNode->data_block[eBlockNumInd];
		
			//if the starting block index > the number of blocks in the node, or the starting block number < 0,
			// return error. 
		if (curBlockNum >bb->num_data_block || curBlockNum<0)
			return RET_ERROR;
		copyData(curBlockNum, buf, bufInd, 0, eBlockOff);
		int num_bytes = bufInd + eBlockOff;
		
		return num_bytes;
	}else
	{  //if stop position is within the node length, then copy the data from the offset to the (offset + the required length). 
		uint32_t sBlockNumInd = offset/KB4;
		uint32_t sBlockOff = offset%KB4;
		uint32_t eBlockNumInd = stop/KB4;
		uint32_t eBlockOff = stop%KB4;
		uint32_t sBlockNum;
		
		uint32_t bufInd = 0;
		
		//check if the starting block index == the ending block index.
		if (sBlockNumInd == eBlockNumInd)
		{	
			sBlockNum = myNode->data_block[sBlockNumInd];		
			
			//if the starting block index > the number of blocks in the node, or the starting block number < 0,
			// return error. 
			if (sBlockNum >bb->num_data_block || sBlockNum<0)
				return RET_ERROR;
				
			//copy the data of from beginning of the last block to the end of the required data to the buffer.  
			copyData(sBlockNum, buf, bufInd, sBlockOff, eBlockOff-sBlockOff);
			return length;
		
		}
		
		
		sBlockNum = myNode->data_block[sBlockNumInd];
		
		//if the starting block index > the number of blocks in the node, or the starting block number < 0,
		// return error.
		if (sBlockNum >bb->num_data_block || sBlockNum<0)
			return RET_ERROR;
		copyData(sBlockNum, buf, bufInd, sBlockOff, KB4-sBlockOff);
		bufInd+=(KB4-sBlockOff);
		
		//copy each block into the buffer;
		for (i = 0; i< (eBlockNumInd-sBlockNumInd-1); i++)
		{
			uint32_t curBlockNum = myNode->data_block[sBlockNumInd+i+1];
			//if the starting block index > the number of blocks in the node, or the starting block number < 0,
			// return error.			
			if (curBlockNum >bb->num_data_block || curBlockNum<0)
				return RET_ERROR;
			copyData(curBlockNum, buf, bufInd, 0, KB4);
			bufInd+=KB4;
		
		}
		
		//copy the data in the last block into the buffer.  
		uint32_t curBlockNum = myNode->data_block[eBlockNumInd];
		//if the starting block index > the number of blocks in the node, or the starting block number < 0,
		// return error.
		if (curBlockNum >bb->num_data_block || curBlockNum<0)
				return RET_ERROR;
		copyData(curBlockNum, buf, bufInd, 0, eBlockOff);
		
		return length;
		
	
	}
	
}

/* copyData
 * Description: copy each byte data into a buffer from some a block according the block index and the size of the data. 
 *              
 * INPUTS:	
	uint32_t blockNum --  the index of the block that contains the data. 
	uint8_t* buf -- the destination buffer. 
	uint32_t bufOff -- the offset destination buffer from the buffer's beginning. 
	uint32_t blockOff -- the offset from the beginning of the block;
	uint32_t size -- the size of the data that is going to be copied into the buffer. the unit of the size is 1 byte; 
 * OUTPUTS:	None
 * RETURN: -1 - failure, invalid inode # or bad data block number is found
 *          0 - end of file reached
 *          other - return the number of bytes read on success
 * SIDE EFFECTS: none
 */
 
void copyData(uint32_t blockNum, uint8_t* buf, uint32_t bufOff, uint32_t blockOff, uint32_t size)
{

	uint32_t i;
	// find block
	uint8_t* curData = findBlock(blockNum);
	//copy each byte into the buffer. 
	for (i=0; i<size; i++)
	{
		
		buf[bufOff+i] = curData[blockOff+i];
	
	}


}


/* findNode
 * Description: find the address of a node according to the index of the node. 
 * INPUTS:	
	 uint32_t inode -- the index node that contains the data. 
 * OUTPUTS:	None
 * RETURN: the address of a node according to the index of the node. 
 * SIDE EFFECTS: none
 */
inode_t * findNode(uint32_t inode)
{
	// start address + 4kb (boot block) + 4kb for each index node
	return (inode_t *)(mods_addrs + KB4 + inode * KB4);

}


/* findBlock
 * Description: find the address of a block 
 *              
 * INPUTS:	
 *	 uint32_t blockNum -- the block index
 * OUTPUTS:	None
 * RETURN: the address of the block found according to its index;
 * SIDE EFFECTS: none
 */
uint8_t* findBlock(uint32_t blockNum)
{
	// start address + 4kb * (index nodes + boot block) + 4kb for each data block
	return (uint8_t*)(mods_addrs + KB4*(((boot_block_t*)mods_addrs)->num_inode+1) + blockNum * KB4);

}




