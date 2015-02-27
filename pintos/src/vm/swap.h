#ifndef VM_SWAP_H
#define VM_SWAP_H

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

#define SECTORS_IN_PAGE 8

void swap_init (void); 
void swap_obtain (struct sp_entry *spe);
void swap_write (struct sp_entry *spe);
void swap_read (struct sp_entry *spe);
void swap_delete (struct sp_entry *spe);

#endif /* vm/swap.h */
