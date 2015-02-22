#ifndef VM_FRAME_H
#define VM_FRAME_H
/*
vm_SRC = vm/frame.c
vm_SRC += vm/swap.c
vm_SRC += vm/page.c
*/

#include "vm/frame.h"

#define PAGEADDR 12
#define USEDMASK 1
#define NOTUSEDMASK 0

static struct frame_table
  {
    uint32_t *ft;           /* pointer to the static array of ft entries. */
    size_t cursor;      /* points to the current index in the frame table. */
    struct bitmap *usage_map; 
  } table;

static struct frame_table table;

/* Returns point to Kernel allocated frame table */
void frame_table_init (size_t user_page_limit)
{
  table.ft = (uint32_t*) calloc (user_page_limit, sizeof (uint32_t));
  table.usage_map = bitmap_create (user_page_limit);
  table.cursor = 0;
}

void update_frame_entry (void *addr)
{
  int index = table.cursor;
  insert_page_addr (index, addr);
  set_use (index, true);
  table.cursor++;
}

/* update table entry to addr */
void insert_page_addr (size_t index, void *addr)
{
  table.ft[index] = (uint32_t) addr;
}

void set_use (size_t index, bool value)
{ 
  bitmap_set (table.usage_map, index, value);
}

#endif

