#ifndef FILESYS_FREE_MAP_H
#define FILESYS_FREE_MAP_H

#include <stdbool.h>
#include <stddef.h>
#include "devices/block.h"

struct free_map
  {
    struct file *free_map_file;            /* Free map file. */
    struct bitmap *free_sectors;           /* Free map, one bit per sector. */
    struct block_sector_t reserved_inode;  /* Reserve a growing file. */
    struct lock bitmap_lock;               /* Lock for doing bitmap ops */
  };

void free_map_init (struct free_map *, size_t, size_t);
void free_map_create (struct free_map *);
void free_map_open (struct free_map *);
void free_map_close (struct free_map *);

bool free_map_allocate (struct free_map *, size_t, size_t, block_sector_t *);
void free_map_set_multiple (struct free_map *, size_t, size_t);
void free_map_release (struct free_map *, block_sector_t, size_t);

#endif /* filesys/free-map.h */
