#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include <stdint.h>
#include "userprog/process.h"

typedef int pid_t;
typedef int mapid_t;

void syscall_init (void);

#endif /* userprog/syscall.h */
