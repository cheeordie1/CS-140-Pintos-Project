#include <random.h>//just for now :')



#include "vm/frame.h"
#include <bitmap.h>
#include <debug.h>
#include <string.h>
#include "userprog/pagedir.h"
#include "vm/swap.h" 

struct frame_table
  {
    struct lock ft_lock;        /* Lock the frame table before access. */
    struct ft_entry *ft;        /* Pointer to the static array of ft entries. */
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
  /* Free memory starts at 1 MB and runs to the end of RAM. */
  uint8_t *free_start = ptov (1024 * 1024);
  uint8_t *free_end = ptov (init_ram_pages * PGSIZE);
  size_t free_pages = (free_end - free_start) / PGSIZE;
  size_t user_pages = free_pages / 2; 
  table.ft = (struct ft_entry *) calloc (user_pages, 
                                         sizeof (struct ft_entry));
  table.used_map = bitmap_create (user_pages);
}

/* Obtain a frame for the page referred to by spe.
   Fetch the page if located somewhere. */
bool
frame_obtain (struct sp_entry *spe)
{
  bool success = true;
  // TODO acquire shared allocator lock
  lock_acquire (&table.ft_lock);
  spe->idx = bitmap_scan_and_flip (table.used_map, 0, 1, 0);
  if (spe->idx == BITMAP_ERROR)
    {
      //  PROBLEM 1 : how do we get the spe of the evicted frame to set shit up?
      // TODO release shared allocator lock
      // TODO acquire exclusive evictor lock
      spe->idx = frame_evict ();
      // TODO release exclusive evictor lock
      // acquire shared allocator lock :')
    }
  struct ft_entry *fte = frame_get (spe);
  if (!frame_fetch (spe))
      success = false;
  else if (!frame_install_page (spe))
    {
      palloc_free_page ((uint8_t *) frame_get (spe)->p_addr);
      success = false;
    }
  if (!success)
    {
      bitmap_reset (table.used_map, spe->idx); 
      spe->idx = BITMAP_ERROR;
    }
  else
    {
      fte->user = spe;
    } 
  lock_release (&table.ft_lock);
  // TODO release shared allocator lock
  return success; 
}

/* Delete a page at index from the frame table. */
void
frame_delete (struct sp_entry *spe)
{
  // TODO fix frame delete
  if (spe->idx == BITMAP_ERROR)
    return;
  // TODO acquire shared allocator lock
  ASSERT (bitmap_test (table.used_map, spe->idx));
  pagedir_clear_page (spe->pagedir, spe->upage);
  struct ft_entry *fte = frame_get (spe);
  uint8_t *pg = (uint8_t *) fte->p_addr;
  palloc_free_page (pg);
  bitmap_reset (table.used_map, spe->idx);
  // TODO release shared allocator lock
}

/* Retrieve a frame given an index. */
struct ft_entry *
frame_get (struct sp_entry *spe)
{
  struct ft_entry *ret_frame;
  ASSERT (spe->idx < bitmap_size (table.used_map));
  ASSERT (!bitmap_test (table.used_map, spe->idx) || 
          spe->idx != BITMAP_ERROR);
  ret_frame = &table.ft[spe->idx]; 
  return ret_frame;
}

/* Evict a frame from the table. */
static size_t
frame_evict ()
{
  // TODO implement eviction
  /* stack information is read to swap slot*/

  /*
    currently, we just remove the first mother fucker in there
    
    if unmapped
        goes to swap
    
    if it's filesys and hasn't been changed, just fuck it

    if it's swappable
        goes to swap
  */
  int money_money_money_mooooney = (int) random_ulong ();
  struct ft_entry fte = table.ft[money_money_money_mooooney];
  switch (fte.user->location) 
    {
      case UNMAPPED:
        fte.user->location = SWAPPED;
      case SWAPPED:
        swap_write (fte.user);
        break;
      case FILESYSTEM:
        if(pagedir_is_dirty (fte.user->pagedir, fte.user->upage))
          {
            file_write_at (fte.user->fp, (uint8_t *) fte.p_addr,
                           fte.user->read_bytes, fte.user->ofs);
          }
        break;
    }
   



  /* executables and code segments are read to disk*/
  return money_money_money_mooooney; // :')
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
        swap_delete (spe);
        break;
      case FILESYSTEM:
        if (spe->zero_bytes != PGSIZE) 
          {
            kpage = palloc_get_page (PAL_USER);
            if (kpage == NULL) 
              return false;
            if ((size_t) file_read_at (spe->fp, kpage,
                spe->read_bytes, spe->ofs) < spe->read_bytes)
              thread_exit ();
            memset (kpage + spe->read_bytes, 0, spe->zero_bytes);
          } 
        else
          {
            kpage = palloc_get_page (PAL_ZERO);
            if (kpage == NULL) 
              return false;
          }
        break;
      default: NOT_REACHED ();
    }
  table.ft[spe->idx].p_addr = (uint32_t) kpage;
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
  return (pagedir_get_page (spe->pagedir, spe->upage) == NULL
          && pagedir_set_page (spe->pagedir, spe->upage,
                              (uint8_t *) table.ft[spe->idx].p_addr,
                              spe->writable));
}
