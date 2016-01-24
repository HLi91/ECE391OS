# This ECE391OS project is an in class project written by
# Hongchuan Li, Meng Gao, Junqing Deng, Han Jiang in UIUC (spring 2014)
# The current file is wriiten by Hongchuan Li
# Students who are taking ECE391 should AVOID copying this file

.globl initial_pit
.globl _initial_pit


_initial_pit:
initial_pit:

pushl %ebp
movl %esp, %ebp
pushf

cli
movb $0x36, %al
outb %al, $0x43



movw 8(%ebp), %ax
outb %al, $0x40
movb %ah, %al
outb %al, $0x40


popf
leave
ret
