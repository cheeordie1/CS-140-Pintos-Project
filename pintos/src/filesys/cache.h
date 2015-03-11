#ifndef CACHE_H
#define CACHE_H

#include "threads/synch.h"
#include "devices/block.h"

struct lock *cache_lock;

void cache_init (void);
struct inode *cache_find_sector (block_sector_t inode_sector);
void cache_insert (struct cache_block *cache_block);

#endif /* filesys/cache.h */
