#ifndef VM_PAGE_H
#define VM_PAGE_H

#include <bitmap.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <syscall-nr.h>
#include <hash.h>
#include "threads/synch.h"
#include "threads/interrupt.h"
#include "threads/malloc.h"
#include "threads/palloc.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "userprog/syscall.h"
#include "filesys/file.h"
#include "filesys/filesys.h"

enum cache_flag
  {
    FRAMED = 001,           /* Faulting page is in frame table. */
    SWAPPED = 002,          /* Faulting page is in swap slot. */
    FILESYSTEM = 004        /* Faulting page is on disc. */
  };

struct spt_entry
  {
    size_t ft_idx;
    enum cache_flag location;
    struct thread *page_owner;
    struct hash_elem elem;
    int fd; 
  };

void page_table_init (void);
void page_table_alloc (size_t index, enum cache_flag flag);



#endif
