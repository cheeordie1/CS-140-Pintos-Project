#ifndef CACHE_H
#define CACHE_H
#include "threads/synch.h"
#include "devices/block.h"

enum sector_type
  {
    FILE_DATA = 1,       /* Cached fileblock represents file data. */
    INODE_DATA = 2,      /* Cached fileblock represents inode data. */
    INODE_METADATA = 3   /* Cached fileblock represents inode metadata. */
  };

struct cache_block
  {
    struct shared_lock pin_lock;    /* Lock for pinning a slot. */
    struct shared_lock rw_lock;     /* Lock for sharing read access. */
    struct hash_elem elem;          /* Element in a hash by sector. */
    block_sector_t sector;          /* Sector number to hash with. */
    char accessed;                  /* For reads or writes */
    bool removed;                   /* Allocated or not. */
    bool dirty;                     /* Recent write access. */
    enum sector_type type;          /* Type of fileblock for eviction. */
    char data[BLOCK_SECTOR_SIZE];   /* Fileblock data. */
  };

struct shared_lock general_eviction_lock;
// TODO PLEASE REMEMBER TO TAKE THIS OUT
struct lock GENGAR;

void cache_init (void);
struct cache_block *cache_find_sector (block_sector_t);
struct cache_block *cache_fetch (block_sector_t, enum sector_type);
void cache_delete (struct cache_block *);
void cache_flush (void);

#endif /* filesys/cache.h */
