#ifndef VM_FRAME_H
#define VM_FRAME_H

#include <bitmap.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/malloc.h"
#include "threads/palloc.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "filesys/file.h"
#include "filesys/filesys.h"

struct lock eviction_lock;

void frame_init (size_t user_page_limit);
size_t frame_alloc (enum palloc_flags flags);
void frame_delete (size_t index);
uint8_t *frame_get (size_t index);

#endif /* vm/frame.h */
