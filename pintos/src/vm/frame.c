#include "vm/frame.h"
#include <bitmap.h>
#include <debug.h>
#include <string.h>
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
  table.ft = (struct ft_entry *) calloc (user_page_limit, 
                                         sizeof (struct ft_entry));
  table.used_map = bitmap_create (user_page_limit);
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
      // acquire shared allocator lock
      PANIC ("\n\nRan out of memory to give you :*(\n\n");
    }
  struct ft_entry *fte = frame_get (spe);
  if (!frame_install_page (spe))
    success = false;
  else if (!frame_fetch (spe))
    {
      pagedir_clear_page (spe->t->pagedir, spe->upage);
      success = false;
    }
  if (!success)
    {
      bitmap_reset (table.used_map, spe->idx); 
      spe->idx = BITMAP_ERROR;
    }
  else
    {
      spe->location = FRAMED;
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
  pagedir_clear_page (spe->t->pagedir, spe->upage);
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
  /* executables and code segments are read to disk*/
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
        swap_delete (spe);
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
  return (pagedir_get_page (spe->t->pagedir, spe->upage) == NULL
          && pagedir_set_page (spe->t->pagedir, spe->upage,
                              (uint8_t *) table.ft[spe->idx].p_addr,
                              spe->writable));
}
