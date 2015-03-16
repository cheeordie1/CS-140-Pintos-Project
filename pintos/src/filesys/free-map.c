#include "filesys/free-map.h"
#include <bitmap.h>
#include <debug.h>
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "filesys/inode.h"

#define PERCENT_INODES 1 / 100

/* Initializes the free map. */
void
free_map_init (struct free_map *free_map, size_t size, size_t reserved_inode) 
{
  free_map->free_sectors = bitmap_create (size);
  if (free_map->free_sectors == NULL)
    PANIC ("bitmap creation failed-- map is too large");
  free_map->free_map_file = NULL;
  free_map->reserved_inode = reserved_inode;
  lock_init (&free_map->bitmap_lock);
}

/* Allocates CNT consecutive sectors from the free map and stores
   the first into *SECTORP.
   Returns true if successful, false if not enough consecutive
   sectors were available or if the free_map file could not be
   written. */
bool
free_map_allocate (struct free_map *free_map, size_t start,
                   size_t cnt, block_sector_t *sectorp)
{
  lock_acquire (&free_map->bitmap_lock);
  block_sector_t sector = bitmap_scan_and_flip (free_map->free_sectors, start, 
                                                cnt, false);
  lock_release (&free_map->bitmap_lock);
  if (sector != BITMAP_ERROR
      && free_map->free_map_file != NULL
      && !bitmap_write (free_map->free_sectors, free_map->free_map_file))
    {
      lock_acquire (&free_map->bitmap_lock);
      bitmap_set_multiple (free_map->free_sectors, sector, cnt, false); 
      sector = BITMAP_ERROR;
      lock_release (&free_map->bitmap_lock);
    }
  if (sector != BITMAP_ERROR)
    *sectorp = sector;
  return sector != BITMAP_ERROR;
}

/* Marks CNT consecutive sectors as true in the free map.
   Returns true if successful, false if could not write to file. */
void
free_map_set_multiple (struct free_map *free_map, size_t start, size_t cnt)
{
  lock_acquire (&free_map->bitmap_lock);
  bitmap_set_multiple (free_map->free_sectors, start, cnt, true);
  lock_release (&free_map->bitmap_lock);
  ASSERT (free_map->free_map_file != NULL);
  ASSERT (bitmap_write (free_map->free_sectors, free_map->free_map_file));
}

/* Makes CNT sectors starting at SECTOR available for use. */
void
free_map_release (struct free_map *free_map, block_sector_t sector, size_t cnt)
{
  lock_acquire (&free_map->bitmap_lock);
  ASSERT (bitmap_all (free_map->free_sectors, sector, cnt));
  bitmap_set_multiple (free_map->free_sectors, sector, cnt, false);
  lock_release (&free_map->bitmap_lock);
  bitmap_write (free_map->free_sectors, free_map->free_map_file);
}

/* Opens the free map file and reads it from disk. */
void
free_map_open (struct free_map *free_map)
{
  free_map->free_map_file = file_open (inode_open (free_map->reserved_inode));
  if (free_map->free_map_file == NULL)
    PANIC ("can't open free map");
  if (!bitmap_read (free_map->free_sectors, free_map->free_map_file))
    PANIC ("can't read free map");
}

/* Writes the free map to disk and closes the free map file. */
void
free_map_close (struct free_map *free_map) 
{
  file_close (free_map->free_map_file);
}

/* Creates a new free map file on disk and writes the free map to
   it. */
void
free_map_create (struct free_map *free_map) 
{
  /* Create inode. */
  if (!inode_create (free_map->reserved_inode, bitmap_size (free_map->free_sectors),
                     false))
    PANIC ("free map creation failed");

  /* Write bitmap to file. */
  free_map->free_map_file = file_open (inode_open (free_map->reserved_inode));
  if (free_map->free_map_file == NULL)
    PANIC ("can't open free map");
  if (!bitmap_write (free_map->free_sectors, free_map->free_map_file))
    PANIC ("can't write free map");
}
