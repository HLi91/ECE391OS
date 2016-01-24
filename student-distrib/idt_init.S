# This ECE391OS project is an in class project written by
# Hongchuan Li, Meng Gao, Junqing Deng, Han Jiang in UIUC (spring 2014)
# The current file is wriiten by Hongchuan Li
# Students who are taking ECE391 should AVOID copying this file


.globl _isr0
.globl _isr1
.globl _isr2
.globl _isr3
.globl _isr4
.globl _isr5
.globl _isr6
.globl _isr7
.globl _isr8
.globl _isr9
.globl _isr10
.globl _isr11
.globl _isr12
.globl _isr13
.globl _isr14
.globl _isr15
.globl _isr16
.globl _isr17
.globl _isr18
.globl _isr19
.globl _isr20
.globl _isr21
.globl _isr22
.globl _isr23
.globl _isr24
.globl _isr25
.globl _isr26
.globl _isr27
.globl _isr28
.globl _isr29
.globl _isr30
.globl _isr31
.globl _isr32
.globl _isr33
.globl _isr34
.globl _isr35
.globl _isr36
.globl _isr37
.globl _isr38
.globl _isr39
.globl _isr40
.globl _isr41
.globl _isr42
.globl _isr43
.globl _isr44
.globl _isr45
.globl _isr46
.globl _isr47
.globl _isr48
.globl _isr49
.globl _isr50

.globl isr0
.globl isr1
.globl isr2
.globl isr3
.globl isr4
.globl isr5
.globl isr6
.globl isr7
.globl isr8
.globl isr9
.globl isr10
.globl isr11
.globl isr12
.globl isr13
.globl isr14
.globl isr15
.globl isr16
.globl isr17
.globl isr18
.globl isr19
.globl isr20
.globl isr21
.globl isr22
.globl isr23
.globl isr24
.globl isr25
.globl isr26
.globl isr27
.globl isr28
.globl isr29
.globl isr30
.globl isr31
.globl isr32
.globl isr33
.globl isr34
.globl isr35
.globl isr36
.globl isr37
.globl isr38
.globl isr39
.globl isr40
.globl isr41
.globl isr42
.globl isr43
.globl isr44
.globl isr45
.globl isr46
.globl isr47
.globl isr48
.globl isr49
.globl isr50


#important update
#for the isr that does have error code
#i dont need to push 0 

isr0:
_isr0:
    cli
    pushl $0    
                   
    pushl $0
    jmp isr_common_stub
_isr1:
isr1:
    cli
    pushl $0    
                   
    pushl $1
    jmp isr_common_stub
_isr2:
isr2:
    cli
    pushl $0    
                   
    pushl $2
    jmp isr_common_stub
_isr3:
isr3:
    cli
    pushl $0    
                   
    pushl $3
    jmp isr_common_stub
_isr4:
isr4:
    cli
    pushl $0    
                   
    pushl $4
    jmp isr_common_stub
	

_isr5:
isr5:
    cli
    pushl $0    
                   
    pushl $5
    jmp isr_common_stub
	

_isr6:
isr6:
    cli
    pushl $0    
                   
    pushl $6
    jmp isr_common_stub
	

_isr7:
isr7:
    cli
    pushl $0    
                   
    pushl $7
    jmp isr_common_stub
	

_isr8:
isr8:
    cli
    #pushl $0    
                   
    pushl $8
    jmp isr_common_stub
	

_isr9:
isr9:
    cli
    pushl $0    
                   
    pushl $9
    jmp isr_common_stub
	

_isr10:
isr10:
    cli
    pushl $0    
                   
    pushl $10
    jmp isr_common_stub
	
_isr11:
isr11:
    cli
    pushl $0    
                   
    pushl $11
    jmp isr_common_stub
	
_isr12:
isr12:
    cli
    pushl $0    
                   
    pushl $12
    jmp isr_common_stub
	
_isr13:
isr13:
    cli
    #pushl $0    
                   
    pushl $13
    jmp isr_common_stub
	
_isr14:
isr14:
    cli
    #pushl $0    
                   
    pushl $14
    jmp isr_common_stub
	
_isr15:
isr15:
    cli
    pushl $0    
                   
    pushl $15
    jmp isr_common_stub
	
_isr16:
isr16:
    cli
    pushl $0    
                   
    pushl $16
    jmp isr_common_stub
	
_isr17:
isr17:
    cli
    pushl $0    
                   
    pushl $17
    jmp isr_common_stub
	
_isr18:
isr18:
    cli
    pushl $0    
                   
    pushl $18
    jmp isr_common_stub
	
_isr19:
isr19:
    cli
    pushl $0    
                   
    pushl $19
    jmp isr_common_stub
	
_isr20:
isr20:
    cli
    pushl $0    
                   
    pushl $20
    jmp isr_common_stub
	
_isr21:
isr21:
    cli
    pushl $0    
                   
    pushl $21
    jmp isr_common_stub
	
_isr22:
isr22:
    cli
    pushl $0    
                   
    pushl $22
    jmp isr_common_stub
	
_isr23:
isr23:
    cli
    pushl $0    
                   
    pushl $23
    jmp isr_common_stub
	
_isr24:
isr24:
    cli
    pushl $0    
                   
    pushl $24
    jmp isr_common_stub
	
_isr25:
isr25:
    cli
    pushl $0    
                   
    pushl $25
    jmp isr_common_stub
	
_isr26:
isr26:
    cli
    pushl $0    
                   
    pushl $26
    jmp isr_common_stub
	
_isr27:
isr27:
    cli
    pushl $0    
                   
    pushl $27
    jmp isr_common_stub
	
_isr28:
isr28:
    cli
    pushl $0    
                   
    pushl $28
    jmp isr_common_stub
	
_isr29:
isr29:
    cli
    pushl $0    
                   
    pushl $29
    jmp isr_common_stub
	
_isr30:
isr30:
    cli
    pushl $0    
                   
    pushl $30
    jmp isr_common_stub	
_isr31:
isr31:
    cli
    pushl $0    
                   
    pushl $31
    jmp isr_common_stub	
	
	
_isr32:
isr32:
    cli
    pushl $0    
                   
    pushl $32
    jmp isr_common_stub
		
_isr33:
isr33:
    cli
    pushl $0    
                   
    pushl $33
    jmp isr_common_stub
		
_isr34:
isr34:
    cli
    pushl $0    
                   
    pushl $34
    jmp isr_common_stub
		
_isr35:
isr35:
    cli
    pushl $0    
                   
    pushl $35
    jmp isr_common_stub
		
_isr36:
isr36:
    cli
    pushl $0    
                   
    pushl $36
    jmp isr_common_stub
		
_isr37:
isr37:
    cli
    pushl $0    
                   
    pushl $37
    jmp isr_common_stub
		
_isr38:
isr38:
    cli
    pushl $0    
                   
    pushl $38
    jmp isr_common_stub
		
_isr39:
isr39:
    cli
    pushl $0    
                   
    pushl $39
    jmp isr_common_stub
		
_isr40:
isr40:
    cli
    pushl $0    
                   
    pushl $40
    jmp isr_common_stub
		
_isr41:
isr41:
    cli
    pushl $0    
                   
    pushl $41
    jmp isr_common_stub
		
_isr42:
isr42:
    cli
    pushl $0    
                   
    pushl $42
    jmp isr_common_stub
		
_isr43:
isr43:
    cli
    pushl $0    
                   
    pushl $43
    jmp isr_common_stub
		
_isr44:
isr44:
    cli
    pushl $0    
                   
    pushl $44
    jmp isr_common_stub
		
_isr45:
isr45:
    cli
    pushl $0    
                   
    pushl $45
    jmp isr_common_stub
		
_isr46:
isr46:
    cli
    pushl $0    
                   
    pushl $46
    jmp isr_common_stub
		
_isr47:
isr47:
    cli
    pushl $0    
                   
    pushl $47
    jmp isr_common_stub
		
_isr48:
isr48:
    cli
    pushl $0    
                   
    pushl $48
    jmp isr_common_stub
		
_isr49:
isr49:
    cli
    pushl $0    
                   
    pushl $49
    jmp isr_common_stub
		
_isr50:
isr50:
    cli
    pushl $0    
                   
    pushl $50
    jmp isr_common_stub
	


isr_common_stub:
#save flag registers
#revise the segment registers
    pusha
	
    pushl %ds
    pushl %es
    pushl %fs
    pushl %gs
    movw $0x18, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs

    call do_isr      

    popl %gs
    popl %fs
    popl %es
    popl %ds
	
    popa
	
    addl $8, %esp  
	#sti
    iret


