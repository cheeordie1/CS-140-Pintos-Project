
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

void frame_table_init (size_t user_page_limit);
bool check_use (size_t index);
void set_use (size_t index, bool value); 
void update_frame_entry (void *addr);
void insert_page_addr (size_t index, void *addr);
void delete_entry (void *addr);



  
