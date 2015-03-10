#include "filesys/cache.h"
#include <bitmap.h>
#include <hash.h>

#define CACHE_MAX 64
#define CACHE_BLOCK_SZ 512

struct buffer_cache
  {
    struct bitmap *used_blocks;
    struct hash cache_segment;
  };

struct cache_block
  {
    struct hash_elem elem;
    size_t inumber;
    char inode_chunk[CACHE_BLOCK_SZ];
  };

static unsigned cache_hash (const struct hash_elem *e, void *aux);

static struct buffer_cache cache;

void
cache_init ()
{
  lock_init (&cache_lock);
  cache.used_blocks = bitmap_create (CACHE_BLOCKS);
  cache.cache_segment = hash_init (&cache_segment);
}

/* Hash the cache block by inumber. */
static unsigned
cache_hash (const struct hash_elem *e, void *aux)
{
  return hash_int (hash_entry (e, struct cache_block, elem)->inumber);
}

/* Compare cache blocks by inumber. */
bool
cache_cmp (const struct hash_elem *a,
           const struct hash_elem *b,
           void *aux)
{
  struct cache_block *block_a = hash_entry (a, struct cache_block, elem);
  struct cache_block *block_b = hash_entry (b, struct cache_block, elem);
  return block_a->inumber < block_b->inumber;
}

/* Insert an inode into the cache based on inumber. 
   If out of room, evict. */
void cache_insert ()
{
  
}
