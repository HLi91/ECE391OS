# This ECE391OS project is an in class project written by
# Hongchuan Li, Meng Gao, Junqing Deng, Han Jiang in UIUC (spring 2014)
# The current file is wriiten by Hongchuan Li and modified by others
# Students who are taking ECE391 should AVOID copying this file


#define CUR_DESCRIPTOR_MSK 0xffffe000
#define KERNEL_CHK_MSK 0x7fe000
#define EPENDSIG -8


.globl _system_call
.globl system_call
.globl _ret_from_sys_call
.globl ret_from_sys_call
.globl _myexe
.globl myexe
.globl test_fun
.globl _test_fun
.globl RET_EIP
.globl halt, _halt


_system_call:	
system_call:

	# for signal restore
	pushl $0
	pushl $0x80
	pushl %esp
	pushl %gs
	pushl %fs

	# save original value of eax for future use
	# this is NOT used in code anymore
	pushl %eax
	
	# save all
	#pushl %fs
	#pushl %gs
    pushl %es
    pushl %ds	# save user DS
    pushl %eax
    pushl %ebp
    pushl %edi
    pushl %esi
	
	# arguments to syscall
    pushl %edx
    pushl %ecx
    pushl %ebx
	
    movl $0x18, %edi
    movl %edi,%ds	# change to kenel_DS
    movl %edi,%es

	#######page directory switch#######
	movl $kpde, %esi
	movl %esi, %cr3
	###################################
	
	movl %esp, %esi
	andl $CUR_DESCRIPTOR_MSK, %esi	# apply bitmask to get pcb location

	


	# check validity of system call (from 1 to 10)
	# index 0 defaults to error handler, no need to check
	cmpl $12, %eax
	ja INVALID_CALL
	
	# jump table to system call handlers
	jmp *sys_call_table(, %eax,4)
	

_ret_from_sys_call:
ret_from_sys_call:
	########################################
	# replace fake_eax by return value to recover eax after popa
	# this code is NOT in use any more, but eax is still on stack
	# DO NOT reactivate without recalculating offset
	#
	# movl %eax, 24(%esp)
	########################################

	# check unhandled pending signal
	cmpl $EPENDSIG, %eax
	jne NO_SIG
	
	# handle signal
	# user eip is decremented, and eax is restored in handler
	# re-issues syscall
	call do_signal_syscall
	
	# return error
	# movl $-1, %eax
	
NO_SIG:
	#######page directory switch#######
	movl %esp, %esi
	andl $CUR_DESCRIPTOR_MSK, %esi
	movl 28(%esi), %ebx
	movl %ebx, %cr3
	###########################
	

	
	
	# restore all & return
    popl %ebx
    popl %ecx
    popl %edx
	
    popl %esi
    popl %edi
    popl %ebp
    #popl %eax		#dont pop eax
	addl $4, %esp
    popl %ds	# restore DS
    popl %es
	
    addl $4, %esp	#skip fake_eax
	popl %fs
	popl %gs
	addl $12, %esp	#skip signal restore info (temporary)
    iret


INVALID_CALL:
	# call error handler sys_call_table[0]
	jmp *sys_call_table

error:
    call trap_error
	jmp ret_from_sys_call

_halt:	
halt:

	
	#pop useless data
	#addl $40, %esp		
	#addl $0x14, %esp	
	#addl $4, %esp	
	movl %esp, %eax
	pushl %eax	
    call trap_halt
	jmp RET_EIP


execute:

	cli
	pushl RET_EIP
	movl %esp, %eax
	pushl %eax
	pushl %ebx
	
	
    call trap_execute
	
BACK_FROM_C:
	#cli
	# clean up stack
	addl $12, %esp
	
	#when return val is -1, i need to return back to caller
	cmpl $-1, %eax
	je ret_from_sys_call

	cmpl $0, %eax
	je ret_from_sys_call

	#save the current ss into pcb->ssSaved
	movl %ss, %ebx
	movl %ebx, 36(%eax)
	

	#code for iret

	#the seg reg must be set to 0x2b
	movl 12(%eax), %ebx
	movw %bx, %ds
    movw %bx, %es
    movw %bx, %fs
    movw %bx, %gs

	movl 28(%eax), %ebx
	movl %ebx, %cr3
	
	#sti need sti

	#pushl PCBSS(%eax)
	pushl 12(%eax)
	#pushl PCBESP(%eax)
	#pushl 8(%eax)
	pushl $0x83ffffc
	pushf
	
	popl %ebx 
	orl $0x200 , %ebx 
	pushl %ebx 

	#pushl PCBCS(%eax)
	pushl 16(%eax)
	#pushl PCBFILE(%eax)
	pushl 24(%eax)

	
	iret

RET_EIP:
	########################
	# Returned from HALT, assumes that ebx contains a 8bit return value
	########################
	#restore tss from the pcb.old pointer 
	movl %esp, %esi
	andl $CUR_DESCRIPTOR_MSK, %esi	# apply bitmask to get pcb location
	
	pushl %esi
	call restore_tss
							#restore esp, ss
	movl 32(%esi), %esp
	movl 36(%esi), %eax
	movl %eax, %ss
							#restore cr3
	movl %esp, %esi
	andl $CUR_DESCRIPTOR_MSK, %esi	#old pcb
	movl 28(%esi), %eax
	movl %eax, %cr3

	addl $4, %esp

	movl $0x18, %ecx
	movl %ecx, %fs
	movl %ecx, %gs
	
	#restore all registers
	
	addl $4, %esp		#ebx need to be reserved
    popl %ecx
    popl %edx
    popl %esi
    popl %edi
    popl %ebp
    #popl %eax
	addl $4, %esp
    popl %ds	# restore DS
    popl %es
	
    addl $4, %esp		# fake_eax value
	popl %fs
	popl %gs
	addl $12, %esp	#skip signal restore info (temporary)
	
	cmpl $256, %ebx
	je EXCEPTION
	
	andl $0x000000FF, %ebx	#zero extend ebx to get return value
	movl %ebx, %eax

	movl $0x2b, %ebx
	movl %ebx, %fs
	movl %ebx, %gs
	

	iret

EXCEPTION:
	movl $256, %eax

	movl $0x2b, %ebx
	movl %ebx, %fs
	movl %ebx, %gs
	

	iret

read:
	#######page directory switch#######
	movl %esp, %esi
	andl $CUR_DESCRIPTOR_MSK, %esi
	cmpl $KERNEL_CHK_MSK, %esi
	je Kread
	movl 28(%esi), %ebx
	movl %ebx, %cr3
	###########################
Kread:
	pushl %esi
    call trap_read	
	addl $4, %esp			
	jmp ret_from_sys_call
	

write:
	#######page directory switch#######
	movl %esp, %esi
	andl $CUR_DESCRIPTOR_MSK, %esi
	cmpl $KERNEL_CHK_MSK, %esi
	je Kwrite
	movl 28(%esi), %ebx
	movl %ebx, %cr3
	###########################
Kwrite:
	pushl %esi
    call 	trap_write			
	addl $4, %esp		
	jmp  	ret_from_sys_call

	
open:
	#######page directory switch#######
	movl %esp, %esi
	andl $CUR_DESCRIPTOR_MSK, %esi
	cmpl $KERNEL_CHK_MSK, %esi
	je Kopen
	movl 28(%esi), %ebx
	movl %ebx, %cr3
	###########################
Kopen:
	pushl %esi
	#pushl %ebx
	
    call 	trap_open
	addl $4, %esp
	jmp  	ret_from_sys_call

	
close:
	pushl %esi
	#pushl %ebx
	
    call 	trap_close
	addl $4, %esp
	jmp  	ret_from_sys_call


getargs:
	#######page directory switch#######
	movl %esp, %esi
	andl $CUR_DESCRIPTOR_MSK, %esi
	cmpl $KERNEL_CHK_MSK, %esi
	je Kgetarg
	movl 28(%esi), %ebx
	movl %ebx, %cr3
	###########################
Kgetarg:
	pushl %esi
    call 	trap_getargs
	addl	$4, %esp
	jmp  	ret_from_sys_call


vidmap:
	pushl	%esi
    call 	trap_vidmap
	addl	$4, %esp
	jmp  	ret_from_sys_call	

	
set_handler:
	# set_handler does not access user space
	# it changes pcb info in kernel
	movl %esp, %esi
	andl $CUR_DESCRIPTOR_MSK, %esi
	pushl %esi
	
    call 	trap_set_handler
	
	addl	$4, %esp
	jmp  	ret_from_sys_call


sigreturn:
	#######page directory switch#######
	movl %esp, %esi
	andl $CUR_DESCRIPTOR_MSK, %esi
	cmpl $KERNEL_CHK_MSK, %esi
	je Ksigreturn
	movl 28(%esi), %ebx
	movl %ebx, %cr3
Ksigreturn:
	###########################
    call 	trap_sigreturn
	jmp  	ret_from_sys_call

malloc:
	call	trap_malloc
	jmp		ret_from_sys_call

modeX:
	#######page directory switch#######
	movl %esp, %esi
	andl $CUR_DESCRIPTOR_MSK, %esi
	cmpl $KERNEL_CHK_MSK, %esi
	je Kmodex
	movl 28(%esi), %ebx
	movl %ebx, %cr3
	###########################
Kmodex:
	call	trap_modeX
	jmp		ret_from_sys_call




	
	
	
	
sys_call_table:
.long error, halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn, malloc, modeX







# code for calling execute from kernel
# creates pcb, pushes desired values, and calls execute
_myexe:
myexe:
	pushl %ebp
	movl %esp, %ebp
	pushl %ebx
	pushl %edi
	pushl %esi


								#construct the kernel pcb
								
	movl %esp, %ebx
	andl $CUR_DESCRIPTOR_MSK, %ebx
	movl %esp, 32(%ebx)
	movl $0x18, 36(%ebx)
	pushf
	popl %eax
	movl %eax, 40(%ebx)
	movl $BACK_K, 44(%ebx)
	movl $0x10, 48(%ebx)
	movl $kpde, 52(%ebx)
	movl $kpde, 28(%ebx)



	movl 8(%ebp), %ebx
	movl $2, %eax
	#jmp system_call





	int $0x80

BACK_K:
	popl %esi
	popl %edi
	popl %ebx
	leave
	ret


exe_error:
	movl $-1, %eax
	jmp ret_from_sys_call




# ONLY for testing purpose
_test_fun:
test_fun:
	movl $1, %eax
	int $0x80
	

