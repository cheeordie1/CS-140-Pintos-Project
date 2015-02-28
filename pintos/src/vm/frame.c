#include "vm/frame.h"
#include <bitmap.h>
#include <debug.h>
#include <string.h>
#include "vm/swap.h" 

struct frame_table
  {
    struct lock ft_lock;        /* Lock the frame table before access. */
    uint32_t *ft;               /* Pointer to the static array of ft entries. */
    struct bitmap *used_map;    /* Keep track of free slots for allocation. */
  };

static struct frame_table table;

static size_t frame_evict (void);
static bool frame_fetch (struct sp_entry *spe);
static bool frame_install_page (struct sp_entry *spe);

/* Allocates frame table memory. */
void 
frame_init (size_t user_page_limit)
{
  lock_init (&eviction_lock);
  lock_init (&table.ft_lock);
  table.ft = (uint32_t *) calloc (user_page_limit, sizeof (uint8_t *));
  table.used_map = bitmap_create (user_page_limit);
}

/* Obtain a frame for the page referred to by spe.
   Fetch the page if located somewhere. */
bool
frame_obtain (struct sp_entry *spe)
{
  bool success = true;
  // TODO Acquire allocation lock
  lock_acquire (&table.ft_lock);
  spe->idx = bitmap_scan_and_flip (table.used_map, 0, 1, 0);
  if (spe->idx == BITMAP_ERROR)
    {
      //  PROBLEM 1 : how do we get the spe of the evicted frame to set shit up?
      spe->idx = frame_evict ();
      PANIC ("\n\nRan out of memory to give you :*(\n\n");
    }
  if (!frame_install_page (spe))
      success = false;
  else if (!frame_fetch (spe))
    success = false;
  if (!success)
    {
      bitmap_reset (table.used_map, spe->idx); 
      spe->idx = BITMAP_ERROR;
    }
  else
    spe->location = FRAMED;
  lock_release (&table.ft_lock);
  // release allocation lock
  return success; 
}

/* Delete a page at index from the frame table. */
void
frame_delete (struct sp_entry *spe)
{
  // TODO fix frame delete
  if (spe->idx == BITMAP_ERROR)
    return;
  ASSERT (bitmap_test (table.used_map, spe->idx));
  pagedir_clear_page (spe->t->pagedir, spe->upage);
  uint8_t *pg = (uint8_t *) table.ft[spe->idx];
  palloc_free_page (pg);
  bitmap_reset (table.used_map, spe->idx);
}

/* Retrieve a frame given an index. */
uint8_t *
frame_get (size_t index)
{
  uint8_t *ret_frame;
  if (!bitmap_test (table.used_map, index) || 
      index == BITMAP_ERROR)
    return NULL;
  ret_frame = (uint8_t *) table.ft[index]; 
  return ret_frame;
}

/* Evict a frame from the table. */
static size_t
frame_evict ()
{
  // TODO implement eviction
  return SIZE_MAX;
}

/* Fetches the page referred to by spe into kpage. */
static bool
frame_fetch (struct sp_entry *spe)
{
  uint8_t *kpage;
  switch (spe->location)
    {
      case UNMAPPED:
        kpage = palloc_get_page (PAL_USER | PAL_ZERO);
        if (kpage == NULL)
          return false;
        break;
      case SWAPPED:
        kpage = palloc_get_page (PAL_USER);
        swap_read (spe);
        break;
      case FILESYSTEM:
        file_seek (spe->fp, spe->ofs * PGSIZE);
        if (spe->read_bytes == PGSIZE) 
          {
            kpage = palloc_get_page (PAL_USER);
            if (kpage == NULL) 
              return false;
            if ((size_t) file_read (spe->fp, kpage, spe->read_bytes) < spe->read_bytes)
              thread_exit ();
          } 
        else if (spe->zero_bytes == PGSIZE)
          {
            kpage = palloc_get_page (PAL_ZERO);
            if (kpage == NULL) 
              return false;
          }
        else 
          {
            kpage = palloc_get_page (PAL_USER);
            if (kpage == NULL) 
              return false;
            if ((size_t) file_read (spe->fp, kpage, spe->read_bytes) < spe->read_bytes)
              thread_exit ();
            memset (kpage + spe->read_bytes, 0, spe->zero_bytes);
          }
        break;
      default: NOT_REACHED ();
    }
  table.ft[spe->idx] = (uint32_t) kpage;
  spe->location = FRAMED;
  return true;
}

/* adds a mapping from user virtual address upage to kernel
   virtual address kpage to the page table.
   if writable is true, the user process may modify the page;
   otherwise, it is read-only.
   upage must not already be mapped.
   returns true on success, false if upage is already mapped or
   if memory allocation fails. */
static bool
frame_install_page (struct sp_entry *spe)
{
  /* verify that there's not already a page at that virtual
     address, then map our page there. */
  return (pagedir_get_page (spe->t->pagedir, spe->upage) == NULL &&
          pagedir_set_page (spe->t->pagedir, spe->upage,
                            (uint8_t *) table.ft[spe->idx],
                            spe->writable));
}
