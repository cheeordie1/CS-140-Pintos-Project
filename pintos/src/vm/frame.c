#include "vm/frame.h"

static struct frame_table
  {
    uint32_t *ft;               /* Pointer to the static array of ft entries. */
    size_t cursor;              /* Points to the current index in the frame table. */
    struct lock ft_lock;        /* Lock the frame table before access. */
  } table;

static struct frame_table table;

/* Allocates frame table memory. */
void 
frame_init (size_t user_page_limit)
{
  table.ft = (uint32_t *) calloc (user_page_limit, sizeof (uint32_t));
  table.cursor = 0;
}

/* update table entry to addr */
uint8_t *
frame_alloc (enum palloc_flags flags)
{
  void *kpage = palloc_get_page (flags);
  if (kpage == NULL)
    {
      // eviction because frame table is filled 
      return NULL;
    }
  table.ft[table.cursor] = (uint32_t) kpage;
  // TODO: check for bugs from one past end
  table.cursor++;
  return kpage;
}
