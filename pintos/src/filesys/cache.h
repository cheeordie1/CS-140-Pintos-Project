#ifndef CACHE_H
#define CACHE_H

#include "threads/synch.h"
#include "devices/block.h"

enum sector_type
  {
    FILE_DATA = 1,       /* Cached fileblock represents file data. */
    INODE_DATA = 2,      /* Cached fileblock represents inode metadata. */
    INODE_METADATA = 3   /* Cached fileblock represents directory metadata. */
  };

struct cache_block
  {
    struct hash_elem elem;          /* Element in a hash by sector. */
    block_sector_t sector;          /* Sector number to hash with. */
    enum sector_type type;          /* Type of fileblock for eviction. */
    char data[BLOCK_SECTOR_SIZE];   /* Fileblock data. */
  };

struct lock *cache_lock;

void cache_init (void);
struct inode *cache_find_sector (block_sector_t inode_sector);
void cache_insert (struct cache_block *cache_block);

#endif /* filesys/cache.h */
