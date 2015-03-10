#include "filesys/cache.h"
#include <bitmap.h>
#include <hash.h>
#include <string.h>

#define CACHE_MAX 64
#define CACHE_BLOCK_SZ 512

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
    char dir_name[NAME_MAX + 1];
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
  return (block_a->inumber < block_b->inumber) || 
         (strcmp (block_a->name, block_b->name) < 0);
}

struct inode *
cache_find_inode (block_sector_t inode_sector, char *name)
{
  struct cache_block singleton;
  singleton.inode_sector = inode_sector;
  if (name != NULL)
    strlcpy (singleton.dir_name, name, NAME_MAX);
  else
    singleton.dir_name[0] = '\0';
  struct hash_elem *cached_inode = hash_find (&cache.cache_segment, 
                                              &singleton.elem);
  if (hash_elem == NULL)
    return NULL;
  else
    return &hash_entry (cached_inode, struct cache_block, elem)->data;
}

/* Insert an inode into the cache based on inumber. 
   If out of room, evict. */
void 
cache_insert (struct cache_block *cache_block)
{
  
}


