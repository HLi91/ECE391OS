#ifndef _SYSCALL_WRAP_H
#define _SYSCALL_WRAP_H
#include "syscall.h"
#include "sig.h"

extern void system_call();
extern void myexe(uint8_t * avg);
extern void test_fun();
extern void halt();

#endif /* _SYSCALL_WRAP_H */

