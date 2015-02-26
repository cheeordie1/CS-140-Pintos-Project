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
#include "userprog/pagedir.h"
#include "vm/page.h"
#include "filesys/file.h"
#include "filesys/filesys.h"

struct lock eviction_lock;

void frame_init (size_t user_page_limit);
bool frame_obtain (struct sp_entry *spe);
void frame_delete (size_t index);
uint32_t *frame_get (size_t index);

#endif /* vm/frame.h */
