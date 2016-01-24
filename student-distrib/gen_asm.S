# This ECE391OS project is an in class project written by
# Hongchuan Li, Meng Gao, Junqing Deng, Han Jiang in UIUC (spring 2014)
# The current file is wriiten by Hongchuan Li
# Students who are taking ECE391 should AVOID copying this file


.globl read_eip, _read_eip
.globl sr_paging, _sr_paging
.globl graphic_mode, _graphic_mode
.globl sig_kill_helper, _sig_kill_helper
.globl sig_ignore_helper, _sig_ignore_helper

_read_eip:
read_eip:
popl %eax
jmp *%eax



_sr_paging:
sr_paging:

	movl %cr0, %eax
	xorl CR0, %eax
	ret


_graphic_mode:
graphic_mode:
	xorl %eax, %eax
	movw $0x13, %ax
	int $0x10


	
CR0:
	.long 0x80000001

	
	
.align 4096
_sig_kill_helper:
sig_kill_helper:
	###################
	# syscall halt:
	# This takes in return value in ebx, decimal 256 indicates die by exception
	###################
	movl $256, %ebx
	movl $1, %eax
	int $0x80
	ret


.align 4096
_sig_ignore_helper:
sig_ignore_helper:
	##################
	# ignores the signal, does nothing
	##################
	ret

.align 4096
_fake_func:
fake_func:
	##################
	# This handler does nothing and is never used
	# It prevents other kernel code from getting accessed
	##################
	addl $1, %eax
	addl $-1, %eax
	ret


