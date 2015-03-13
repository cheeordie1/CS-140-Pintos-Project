#include "filesys/cache.h"
#include <bitmap.h>
#include <hash.h>
#include <string.h>

#define CACHE_MAX 64
#define CACHE_BLOCK_SZ 512
#define EVICT_START 0
#define CURSOR_START 17

struct buffer_cache
  {
    struct bitmap *used_blocks;                  /* Free cache blocks. */
    struct hash cache_segment;                   /* Hash that holds data. */
    struct cache_block cache_frames[CACHE_MAX];  /* Blocks of cached data. */
    size_t cursor;                                  /* Keep track of clock hand */
    size_t evicting_hand;                           /* Hand for eviction. */
  };

static unsigned cache_hash (const struct hash_elem *e, void *aux);
static bool cache_cmp (const struct hash_elem *a,
                       const struct hash_elem *b,
                       void *aux);
static size_t cache_evict (void);

static struct buffer_cache cache;

void
cache_init ()
{
  slock_init (&general_eviction_lock);
  lock_init (&GENGAR);
  cache.used_blocks = bitmap_create (CACHE_BLOCKS);
  cache.cache_segment = hash_init (&cache_segment, cache_hash, 
                                   cache_cmp, NULL);
  cache.cursor = CURSOR_START;
  cache.evicting_hand = EVICT_START;
}

/* Hash the cache block by inumber. */
static unsigned
cache_hash (const struct hash_elem *e, void *aux)
{
  return hash_int (hash_entry (e, struct cache_block, elem)->inumber);
}

/* Compare cache blocks by inumber. */
static bool
cache_cmp (const struct hash_elem *a,
           const struct hash_elem *b,
           void *aux)
{
  struct cache_block *block_a = hash_entry (a, struct cache_block, elem);
  struct cache_block *block_b = hash_entry (b, struct cache_block, elem);
  return (block_a->inode_sector < block_b->inode_sector);
}

/* Find file sector cached in buffer_cache. 
   Return null if not present. */
struct cache_block *
cache_find_sector (block_sector_t sector)
{
  struct cache_block singleton;
  singleton.inode_sector = sector;
  struct hash_elem *cached_inode = hash_find (&cache.cache_segment, 
                                              &singleton.elem);
  if (hash_elem == NULL)
    return NULL;
  else
    {
      struct cache_block *cached_data = hash_entry (cached_inode, 
                                        struct cache_block, elem);
      cached_data->accessed = cached_data->type;
      return cached_data;
    }
}

/* Insert an inode into the cache based on inumber. 
   If out of room, evict. cache_block argument must be
   an already malloced pointer. */
struct cache_block * 
cache_fetch (block_sector_t sector, enum sector_type type)
{ 
  struct cache_block *cache_block;
  size_t pos = bitmap_scan_and_flip (cache.used_blocks, 0, 1, false);
  if (pos == BITMAP_ERROR)
    pos = cache_evict ();
  cache_block = &cache.cache_frames[pos];
  cache_block->type = type;
  cache_block->accessed = type;
  cache_block->sector = sector;
  cache_block->removed = false;
  cache_block->dirty = false;
  hash_insert (&cache.cache_segment, &cache_block->elem);
  block_read (fs_device, cache_block->sector, cache_block->data);
  return cache_block; 
}

/* Removes a cached block from the cache table*/
void
cache_delete (struct cache_block *cache_block) 
{
  if (cache_block->removed)
    return;
  cache_block->removed = true;
  hash_delete (&cache.cache_segment, cache_block->elem);
  if (cache_block->dirty)
    block_write (fs_device, cache_block->sector, cache_block->data);
}

/* Flush all data from cache to filesystem, deleting cache. */
void
cache_flush (void)
{
  while (!hash_empty (&cache.cache_segment))
    {
      struct hash_elem *hashed_block = hash_first (&cache.cache_segment);
      struct cache_block *delete_block = hash_entry (hashed_block, 
                                                     struct cache_block, elem);
      cache_delete (delete_block);
    }
}

/* Use a two hand clock algorithm to evict from the cache. */
static size_t
cache_evict (void)
{
  while (true)
    {
      struct cache_block *cache_block = cache.cache_frames[cache.cursor];
      if (cache_block->accessed > 0)
        cache_block->accessed--;
       
      cache_block = cache.cache_frames[cache.evicting_hand];
      if (cache_block->removed || cache_block->accessed == 0)
        {
          cache_delete (cache_block);
          return cache.evicting_hand;
        }
      cache.cursor = (++(cache.cursor)) % CACHE_MAX;
      cache.evicting_hand = (++(cache.evicting_hand)) % CACHE_MAX;
    }
}
