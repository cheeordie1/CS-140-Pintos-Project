#include "filesys/cache.h"
#include <bitmap.h>
#include <string.h>
#include "devices/timer.h"
#include "threads/thread.h"

#define CACHE_MAX 64
#define CACHE_BLOCK_SZ 512
#define EVICT_START 0
#define CURSOR_START 17

struct buffer_cache
  {
    struct bitmap *used_blocks;                 /* Free cache blocks. */
    struct hash cache_segment;                  /* Hash that holds data. */
    struct lock hash_lock;                      /* Hash lock. */
    struct cache_block cache_frames[CACHE_MAX]; /* Blocks of cached data. */
    size_t cursor;                              /* Keep track of clock hand */
    size_t evicting_hand;                       /* Hand for eviction. */
  };

static unsigned cache_hash (const struct hash_elem *e, void *aux UNUSED);
static bool cache_cmp (const struct hash_elem *a,
                       const struct hash_elem *b,
                       void *aux UNUSED);
static size_t cache_evict (void);

static struct buffer_cache cache;

/* Flush all data from cache to filesystem, deleting cache. */
void
cache_flush (void *aux UNUSED)
{
  for (;;)
    {
      barrier ();
      while (!hash_empty (&cache.cache_segment))
        {
          barrier ();
          struct hash_iterator i;
          hash_first (&i, &cache.cache_segment);
          struct hash_elem *hashed_block = hash_next (&i);
          struct cache_block *delete_block = 
	  		hash_entry (hashed_block, struct cache_block, elem);
          cache_delete (delete_block);
        }
      timer_msleep (30000);  	
    }
}

/* Initialize the cache to flush every 30 seconds. */
void
cache_init ()
{
  lock_init (&cache.hash_lock);
  cache.used_blocks = bitmap_create (CACHE_MAX);
  hash_init (&cache.cache_segment, cache_hash, cache_cmp, NULL);
  cache.cursor = CURSOR_START;
  cache.evicting_hand = EVICT_START;

  size_t curr_block;
  for (curr_block = 0; curr_block < CACHE_MAX; curr_block++)
    {
      struct cache_block *cache_data;
      cache_data = &cache.cache_frames[curr_block];
      slock_init (&cache_data->spot_lock);
      slock_init (&cache_data->rw_lock);
    }
  thread_create ("flush_thread", PRI_MAX, cache_flush, NULL);
}

/* Hash the cache block by inumber. */
static unsigned
cache_hash (const struct hash_elem *e, void *aux UNUSED)
{
  return hash_int (hash_entry (e, struct cache_block, elem)->sector);
}

/* Compare cache blocks by inumber. */
static bool
cache_cmp (const struct hash_elem *a,
           const struct hash_elem *b,
           void *aux UNUSED)
{
  struct cache_block *block_a = hash_entry (a, struct cache_block, elem);
  struct cache_block *block_b = hash_entry (b, struct cache_block, elem);
  return (block_a->sector < block_b->sector);
}

/* Find file sector cached in buffer_cache. 
   Return null if not present. */
struct cache_block *
cache_find_sector (block_sector_t sector)
{
  struct cache_block singleton;
  singleton.sector = sector;
  lock_acquire (&cache.hash_lock);
  struct hash_elem *cached_inode = hash_find (&cache.cache_segment, 
                                              &singleton.elem);
  lock_release (&cache.hash_lock);
  if (cached_inode == NULL)
    return NULL;
  else
    {
      struct cache_block *cached_data = hash_entry (cached_inode, 
                                        struct cache_block, elem);
      lock_acquire_shared (&cached_data->spot_lock);
      if (cached_data->sector != sector)
        {      
          lock_release_shared (&cached_data->spot_lock);
	  return NULL;
	}
      cached_data->accessed = cached_data->type;
      return cached_data;
    }
}


struct cache_block *
cache_pin (block_sector_t sector, enum sector_type type)
{
  size_t pos;
  struct cache_block *cached_data; 
  while (true) 
    {
      barrier ();
      /* The sector wanted is in the cache. */
      if ((cached_data = cache_find_sector (sector)) != NULL)
	    return cached_data;
      /* The sector isn't, so we must search for cache openings or fetch. */
      else
        {
	      while ((pos = bitmap_scan (cache.used_blocks, 0, 1, false))
		         != BITMAP_ERROR)
	        {
	          /* continue trying until exclusive access is gained. */
	          if (!lock_try_acquire_exclusive (&cache.cache_frames[pos]))
                goto TM69;	
            }
	    }
    /* The cache is full, so something must be evicted  */
	pos = cache_evict ();
	cached_data = &cache.cache_frames[pos];

    TM69:
      bitmap_set (cache.used_blocks, pos, true);
	  cached_data = cache_fetch (pos, sector, type);
  	  lock_release_exclusive (&cached_data->spot_lock);
	  lock_acquire_shared (&cached_data->spot_lock);
	  if (cached_data->sector != sector)
	    {
	      lock_release_shared (&cached_data->spot_lock);
          continue;
        }
      else 
        return cached_data;
    }
}

void
cache_unpin (struct cache_block *cached_data)
{
  lock_release_shared (&cached_data->spot_lock);
}

/* Insert an inode into the cache based on inumber. 
   If out of room, evict. cache_block argument must be
   an already malloced pointer. */
struct cache_block * 
cache_fetch (size_t pos, block_sector_t sector, enum sector_type type)
{ 
  struct cache_block *cache_block;
  cache_block = &cache.cache_frames[pos];
  cache_block->type = type;
  cache_block->accessed = type;
  cache_block->sector = sector;
  cache_block->removed = false;
  cache_block->dirty = false;
  lock_acquire (&cache.hash_lock);
  hash_insert (&cache.cache_segment, &cache_block->elem);
  lock_release (&cache.hash_lock);
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
  lock_acquire (&cache.hash_lock);
  hash_delete (&cache.cache_segment, &cache_block->elem);
  lock_release (&cache.hash_lock);
  if (cache_block->dirty)
    block_write (fs_device, cache_block->sector, cache_block->data);
}

/* Writes to a cached sector, setting its dirty bool. */
void
cache_write (struct cache_block *cached_sector, const void *buf,
	     off_t start, size_t cnt)
{
  cached_sector->dirty = true;
  memcpy (&cached_sector->data[start], buf, cnt);
}

/* Use a two hand clock algorithm to evict from the cache. */
static size_t
cache_evict (void)
{
  while (true)
    {
      struct cache_block *cache_block = &cache.cache_frames[cache.cursor];
      if (cache_block->accessed > 0)
        cache_block->accessed--;
       
      cache_block = &cache.cache_frames[cache.evicting_hand];
      if (cache_block->removed || cache_block->accessed == 0)
        {
          cache_delete (cache_block);
          return cache.evicting_hand;
        }
      cache.cursor = (cache.cursor + 1) % CACHE_MAX;
      cache.evicting_hand = (cache.evicting_hand + 1) % CACHE_MAX;
    }
}
