#include "filesys/cache.h"
#include <bitmap.h>
#include <hash.h>
#include <string.h>

#define CACHE_MAX 64
#define CACHE_BLOCK_SZ 512
#define EVICT_START 0
#define CURSOR_START 17


struct inode_disk;

struct buffer_cache
  {
    struct bitmap *used_blocks;
    struct hash cache_segment;
    struct cache_block *used_sectors[CACHE_MAX];
    int cursor;                                  /* Keep track of clock hand */
    int evicting_hand; 
  };

struct cache_block
  {
    struct hash_elem elem;
    block_sector_t sector;
    enum sector_type type;
    char data[BLOCK_SECTOR_SIZE];
    char accessed; /* For reads or writes */
    bool in_use;
    bool dirty; /* For writes only */
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
  return (block_a->sector < block_b->sector);
}

/* Find inode cached in buffer_cache. 
   Return null if not present. */
struct inode *
cache_find_inode (block_sector_t sector)
{
  struct cache_block singleton;
  singleton.sector = sector;
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
struct cache_block * 
cache_insert (enum sector_type type, block_sector_t sector)
{
  struct cache_block *cache_block;
  cache_block = calloc (1, sizeof (struct cache_block));
  cache_block->type = type;
  cache_block->accessed = type;
  cache_block->sector = sector;
  cache_block->in_use = true;
  block_read (fs_device, cache_block->sector, cache_block->data);
  int pos = hash_size (&cache.cache_segment);
  if (pos == CACHE_MAX)
    pos = cache_evict ();
  hash_insert (&cache.cache_segment, &cache_block->elem);
  cache.used_sectors[pos] = cache_block;

  return cache_block;
}

/* Removes a cached block from the cache table*/
void
cache_delete (struct cache_block *cache_block) 
{
  hash_delete (&cache.cache_segment, cache_block->elem);
  if (cache_block->dirty)
      block_write (fs_device, cache_block->sector, cache_block->data);
  free (cache_block);
}

void
cache_write_all (void)
{
  // loop through array checking dirty bits
  //should be called by some thread
}

/* Use a two hand clock algorithm to evict from the cache. */
int
cache_evict ()
{
  while (true)
    {
      struct cache_block *cache_block = cache.used_sectors[cache.cursor];
      cache_block->accessed--;
       
      cache_block = cache.used_sectors[cache.evicting_hand];
      if (!cache_block->in_use || cache_block->accessed == 0)
        {
          cache_delete (cache_block);
          return cache.evicting_hand;
        }
      cache.cursor = (++cache.cursor) % CACHE_MAX;
      cache.evicting_hand = (++cache.evicting_hand) % CACHE_MAX;
    }
  // TODO implement clock eviction. Possibly store a second list
  // element in inodes to put them in modular list

  return;
}
