#include "filesys/cache.h"
#include <bitmap.h>

#define CACHE_BLOCKS 64
#define CACHE_BLOCK_SIZE 512

struct buffer_cache 
  {
    struct bitmap *used_blocks;
    void *cache_segment;
  };

static struct buffer_cache cache;

void
cache_init () 
{
  lock_init (&cache_lock);
  cache.used_blocks = bitmap_create (CACHE_BLOCKS);
  cache_segment = calloc (CACHE_BLOCKS, CACHE_BLOCK_SIZE); 
}
