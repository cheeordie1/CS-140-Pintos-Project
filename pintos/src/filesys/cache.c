#include "filesys/cache.h"
#include <bitmap.h>
#include <hash.h>
#include <string.h>

#define CACHE_MAX 64
#define CACHE_BLOCK_SZ 512

enum sector_type
  {
    INODE = 0100,         /* Cached fileblock represents inode. */
    DIRECTORY = 0200,     /* Cached fileblock represents dir_entries. */
    FILE_DATA = 0400      /* Cached fileblock represents file data. */
  };

struct inode_disk;

struct buffer_cache
  {
    struct bitmap *used_blocks;
    struct hash cache_segment;
  };

struct cache_block
  {
    struct hash_elem elem;
    block_sector_t inode_sector;
    enum sector_type type;
    struct inode_disk data;
  };

static unsigned cache_hash (const struct hash_elem *e, void *aux);
static bool cache_cmp (const struct hash_elem *a,
                       const struct hash_elem *b,
                       void *aux);

static struct buffer_cache cache;

void
cache_init ()
{
  lock_init (&cache_lock);
  cache.used_blocks = bitmap_create (CACHE_BLOCKS);
  cache.cache_segment = hash_init (&cache_segment, cache_hash, 
                                   cache_cmp, NULL);
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
struct void *
cache_find_inode (block_sector_t sector)
{
  struct cache_block singleton;
  singleton.inode_sector = sector;
  struct hash_elem *cached_inode = hash_find (&cache.cache_segment, 
                                              &singleton.elem);
  if (hash_elem == NULL)
    return NULL;
  else
    return &hash_entry (cached_inode, struct cache_block, elem)->data;
}

/* Insert an inode into the cache based on inumber. 
   If out of room, evict. cache_block argument must be
   an already malloced pointer. */
void 
cache_insert (struct cache_block *cache_block)
{
  if (hash_size (&cache.cache_segment) == CACHE_MAX)
    cache_evict ();
  hash_insert (&cache.cache_segment, &cache_block->elem);
}

/* Use a clock algorithm to evict from the cache. */
void
cache_evict ()
{
  // TODO implement clock eviction. Possibly store a second list
  // element in inodes to put them in modular list
  return;
}
