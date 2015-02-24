#include "vm/frame.h"
#include <bitmap.h>
#include <debug.h>

static struct frame_table
  {
    struct lock ft_lock;        /* Lock the frame table before access. */
    uint32_t *ft;               /* Pointer to the static array of ft entries. */
    struct bitmap *used_map;    /* Keep track of free slots for allocation. */
  } table;

static struct frame_table table;

/* Allocates frame table memory. */
void 
frame_init (size_t user_page_limit)
{
  lock_init (&eviction_lock);
  lock_init (&table.ft_lock);
  table.ft = (uint32_t *) calloc (user_page_limit, sizeof (uint32_t));
  used_map = bitmap_create (user_page_limit);
}

/* Get a page frame for a user page. Evict if no pages left.
   This function will be called in process instead of 
   palloc_get_page in order to control the allocation of
   user processes in a caching context. */
size_t
frame_alloc (enum palloc_flags flags)
{
  size_t frame_entry_idx;
  uint8_t *kpage;
  lock_acquire (&table.ft_lock);
  frame_entry_idx = bitmap_scan_and_flip (table.used_map, 0, 1, 0);
  if (frame_entry_idx == BITMAP_ERROR)
    {
      lock_acquire (&eviction_lock); 
      // TODO eviction because frame table full
      PANIC ("\n\nRan out of memory to give you :*(\n\n");
      lock_release (&eviction_lock);
      lock_release (&table.ft_lock);  
      return SIZE_MAX; 
    }
  kpage = palloc_get_page (flags);
  if (kpage == NULL)
    {
      lock_release (&table.ft_lock);
      return BITMAP_ERROR;
    }
  lock_release (&table.ft_lock);
  table.ft[frame_entry_idx] = (uint32_t) kpage;
  return frame_entry_idx;
}

/* Delete a page at index from the frame table. 
   This function will be called in process instead
   of palloc_free_page in order to control the
   freeing of user processes in a caching context. */
void
frame_delete (size_t index)
{
  ASSERT (bitmap_test (table.used_map, index));

  uint32_t *pg = table.ft[index];
  palloc_free_page (pg);
  bitmap_reset (table.used_map, index);
}

/* Retrieve a frame given an index. */
uint8_t *
frame_get (size_t index)
{
  uint8_t *ret_frame;
  lock_acquire (&table.ft_lock);
  if (!bitmap_test (table.used_map, index) || 
      index == BITMAP_ERROR)
    return NULL;
  ret_frame = table.ft[index] 
  lock_release (&table.ft_lock);
  return ret_frame;
}
