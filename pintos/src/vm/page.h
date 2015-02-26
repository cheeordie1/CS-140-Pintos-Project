#ifndef VM_PAGE_H
#define VM_PAGE_H

#include <bitmap.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <syscall-nr.h>
#include <hash.h>
#include <list.h>
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
    UNMAPPED = 000,         /* Faulting page has not been stored. */
    FRAMED = 001,           /* Faulting page is in frame table. */
    SWAPPED = 002,          /* Faulting page is in swap slot. */
    FILESYSTEM = 004        /* Faulting page is on disc. */
  };

struct sp_entry
  {
    struct file *fp;           /* Location in file. */ 
    size_t idx;                /* Index into frame table. */
    enum cache_flag location;  /* Location flag FS, Frame, or Swap. */
    size_t read_bytes;         /* Number of bytes to read. */
    size_t zero_bytes;         /* Number of bytes to zero. */
    size_t ofs;                /* Offset into file. */
    bool writable;             /* Whether page is writable. */
    uint8_t *upage;            /* Virtual address of page. */
    struct thread *t;          /* Owner of the page. */
    struct hash_elem h_elem;   /* Element of hash of sp_entries. */
    struct list_elem l_elem;   /* Element of list of sp_entries. */
  };

void page_table_init (void);
struct sp_entry *page_supp_alloc (struct thread *t, uint8_t *upage);
void page_supp_delete (struct sp_entry *spe_temp);

#endif /* vm/page.h */
