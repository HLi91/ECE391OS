# This ECE391OS project is an in class project written by
# Hongchuan Li, Meng Gao, Junqing Deng, Han Jiang in UIUC (spring 2014)
# The current file is wriiten by Hongchuan Li
# Students who are taking ECE391 should AVOID copying this file
.globl enable_paging
.globl _enable_paging


/* enable_paging
 * Description: set control registers to enable paing
 * INPUTS:	None
 * OUTPUTS:	None
 * RETURN: None
 * SIDE EFFECTS: writes into control registers
 */
enable_paging:
_enable_paging:
	pushl %ebp
	movl %esp, %ebp
	
# upper 20 bits of CR3 => page directory base register 
	movl $kpde, %eax
	movl %eax, %cr3
	#movl %cr3, %ebx
	
# bit 4: 0 => 4kb, 1 => 4MB page size
	movl %cr4, %eax
	orl CR4, %eax
	movl %eax, %cr4

# enable paging in protected mode
	movl %cr0, %eax
	orl CR0, %eax
	movl %eax, %cr0
	
	movl %ebx, %eax
	leave
	ret


CR0:
	.long 0x80000001

CR4:
	.long 0x00000010
