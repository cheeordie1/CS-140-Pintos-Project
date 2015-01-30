#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include <stdint.h>
#include "userprog/process.h"

void syscall_init (void);

void syscall_halt (void);

void syscall_exit (int status UNUSED);

pid_t syscall_exec (const char *cmd_line UNUSED);

int syscall_wait (pid_t pid UNUSED);

bool syscall_create (const char *file UNUSED, uint32_t initial_size UNUSED);

bool syscall_remove (const char* file UNUSED);

int syscall_open (const char* file UNUSED);

int syscall_filesize (int fd UNUSED);

int syscall_read (int fd UNUSED, void *buf UNUSED, uint32_t size UNUSED);

int syscall_write (int fd UNUSED, const void* buf UNUSED, uint32_t size UNUSED);

void syscall_seek (int fd UNUSED, uint32_t position UNUSED);

uint32_t syscall_tell (int fd UNUSED);

void syscall_close (int fd UNUSED);

#endif /* userprog/syscall.h */
