#ifndef VM_FRAME_H
#define VM_FRAME_H

#include <bitmap.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/malloc.h"
#include "threads/palloc.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "devices/input.h"
#include "devices/shutdown.h"
#include "lib/kernel/hash.h"
#include "userprog/pagedir.h"
#include "userprog/syscall.h"

void frame_init (size_t user_page_limit);
uint8_t *frame_alloc (enum palloc_flags flags);
//void frame_delete ();

#endif /* vm/frame.h */
