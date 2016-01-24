#ifndef _FILESYS_H
#define _FILESYS_H


#include "filesys.h"
#include "x86_desc.h"
#include "types.h"
#include "lib.h"

#define RET_ERROR -1
#define KB4 4096
#define KB 1024
#define EOF 0

#define MAX_DIR_NUM 63
#define MAX_FILE_NAME_LENGTH 32
#define ONE_KB 1024
#define RESERVE_24 24
#define RESERVE_52 52
#define STR_LEN_4 4

//directory entry in boot block.
typedef struct dentry_t{
	int8_t file_name[MAX_FILE_NAME_LENGTH]; //32 B file name
	uint32_t file_type; // 0 RTC, 1 directory, 2 regular file
	uint32_t index_node; // only meaningful for regular files
	
	uint8_t reserved[RESERVE_24]; //24B reserved
} dentry_t;

//boot block of file system.
typedef struct boot_block {
	uint32_t num_dir_entry;
	uint32_t num_inode;
	uint32_t num_data_block;
	uint8_t reserved[RESERVE_52]; //52B reserved to make up 64B
	
	dentry_t dir_entry[MAX_DIR_NUM];
} boot_block_t;

//index node of file system.
typedef struct inode{
	uint32_t length;
	uint32_t data_block[ONE_KB - 1];
} inode_t;

/*typedef struct data_t{
	
	uint32_t data_block[1024];
} data_t;*/

//module struct 
typedef struct mod_t{
	boot_block_t* mod_start;
	void * mod_end;
	uint8_t c[STR_LEN_4]; // The ‘string’ field provides an arbitrary string to be associated with that particular boot module
	uint8_t reserved;//The ‘reserved’ field must be set to 0 by the boot loader and ignored by the operating system.
} mod_t;

//read directory entry by given name, save the entry at dentry. 
extern int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
//read directory entry by given index, save the entry at dentry. 
extern int32_t read_dentry_by_index(uint32_t index, dentry_t* dentry);
//read directory entry by given offset to a given inode, save the entry found at dentry. 
extern int32_t read_data(uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

//find the address of a node according to the index of the node. 
inode_t * findNode(uint32_t inode);

//find the address of a block according to the index of the block.  
uint8_t* findBlock(uint32_t blockNum);

//copy each byte data into a buffer from some a block according the block index and the size of the data. 
void copyData(uint32_t blockNum, uint8_t* buf, uint32_t bufOff, uint32_t blockOff, uint32_t size);

#endif

