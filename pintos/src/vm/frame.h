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

struct ft_entry 
  {
    uint32_t p_addr;
    struct sp_entry *user;
  };

void frame_init (size_t user_page_limit);
bool frame_obtain (struct sp_entry *spe);
void frame_delete (struct sp_entry *spe);
struct ft_entry *frame_get (struct sp_entry *spe);

#endif /* vm/frame.h */
