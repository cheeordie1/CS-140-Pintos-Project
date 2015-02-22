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

/* Returns point to Kernel allocated frame table */
void frame_table_init (struct frame_table *table, size_t user_page_limit)
{
  table->ft = (uint32_t*) calloc (user_page_limit, sizeof (uint32_t));
  table->usage_map = bitmap_create (user_page_limit);
  table->cursor = 0;
}

void insert_page (struct frame_table *table, void *addr)
{
  /* check not the final page */

  //memset (table + (idx * sizeof (uint32_t)), addr, sizeof (uint32_t));
  set_use (table, table->cursor, true); 
  table->cursor++;
}

void update_frame_entry (struct frame_table *table, void *addr)
{
  addr = addr << PAGEADDR;
  size_t index = table->cursor;
  memset (table + index, addr, sizeof (uint32_t));//how to just set higher 20
}

void set_use (struct frame_table *table, size_t index, bool value)
{
  if (value)
    table->ft[index] |= USEDMASK;
  else
    table->ft[index] &= NOTUSEDMASK;
}

#endif

