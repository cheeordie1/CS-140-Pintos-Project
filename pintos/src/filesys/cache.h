#ifndef CACHE_H
#define CACHE_H
#include "threads/synch.h"
#include "devices/block.h"

enum sector_type
  {
    FILE_DATA = 1,          /* Cached fileblock represents file data. */
    INODE_DATA = 2,         /* Cached fileblock represents inode. */
    INODE_METADATA = 3      /* Cached fileblock represents inode metadata. */
  };

struct lock *cache_lock;

int cache_init (void);
struct inode *cache_find_inode (block_sector_t inode_sector);
struct cache_block *cache_insert (struct cache_block *cache_block);
void cache_delete (struct cache_block *cache_block);

#endif /* filesys/cache.h */
