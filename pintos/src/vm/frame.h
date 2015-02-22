
#include "userprog/syscall.h"
#include <stdio.h>
#include <string.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/malloc.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "devices/input.h"
#include "devices/shutdown.h"
#include "lib/kernel/hash.h"
#include "userprog/pagedir.h"
#include <bitmap.h>

/*
+---------------------+
| Page ADDR |use bit  | top 20 bits hold pointer
+---------------------+
|           |         | lower 12 holds other info
+---------------------+
|           |         |
+---------------------+
|           |         |
+---------------------+
|           |         |
+---------------------+
|           |         |
+---------------------+
*/
struct frame_table
  {
    uint32_t *ft;           /* pointer to the static array of ft entries. */
    //int num_entries;    /* keeps the index of the highest used array entry. */
    size_t cursor;      /* points to the current index in the frame table. */
    struct bitmap *usage_map; 
    
    /* Possible useful information: process id with current page */
  };

void frame_table_init (struct frame_table *table,size_t user_page_limit);
bool check_use (struct frame_table *table, size_t index);
void set_use (struct frame_table *table, size_t index, bool value); 
void update_frame_entry (struct frame_table *table, void *addr);
void insert_page (struct frame_table *table, void *addr);



  
