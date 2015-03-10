#ifndef CACHE_H
#define CACHE_H

#include "threads/synch.h"

struct lock *cache_lock;

void cache_init (void);

#endif /* filesys/cache.h */
