
#include "vm/swap.h"
#include "devices/block.h"
#include "vm/frame.h"

struct swap_table 
  {
    struct lock swap_lock;
    struct bitmap *used_map;
    struct block *swap_block;
  };

static struct swap_table swap_table;

/* Initialize swap table. */
void 
swap_init () 
{
  lock_init (&swap_table.swap_lock);
  swap_table.swap_block = block_get_role (BLOCK_SWAP);
  swap_table.used_map = bitmap_create (block_size (swap_table.swap_block) * 
                                       BLOCK_SECTOR_SIZE);
}

/* Obtain swap slot and fetch page data into it. */
void
swap_obtain (struct sp_entry *spe)
{
  lock_acquire (&swap_table.swap_lock);
  spe->sector = bitmap_scan_and_flip (swap_table.used_map, 0, SECTORS_IN_PAGE, 0);
  lock_release (&swap_table.swap_lock);
  if (spe->sector == BITMAP_ERROR) 
    PANIC ("All swap slots are filled.\n");
  swap_write (spe);
}

/* Write frame page data into a swap slot. */
void 
swap_write (struct sp_entry *spe)
{
  uint8_t *kpage = (uint8_t *) frame_get (spe)->p_addr;
  int cur;
  for (cur = 0; cur < SECTORS_IN_PAGE; cur++)
    block_write (swap_table.swap_block, spe->sector + cur, 
                 kpage + cur * BLOCK_SECTOR_SIZE);
}

/* Read swap data to frame table*/
void
swap_read (struct sp_entry *spe)
{
  uint8_t *kpage = (uint8_t *) frame_get (spe)->p_addr;
  int cur;
  for (cur = 0; SECTORS_IN_PAGE; cur++)
    block_read (swap_table.swap_block,  spe->sector + cur,
                kpage + cur * BLOCK_SECTOR_SIZE);
}

/* Delete a swap slot. Free its space in swap. */
void
swap_delete (struct sp_entry *spe) 
{
  lock_acquire (&swap_table.swap_lock);
  bitmap_set_multiple (swap_table.used_map, spe->sector,
                       SECTORS_IN_PAGE, false);
  spe->sector = SIZE_MAX;
  lock_release (&swap_table.swap_lock);
}
