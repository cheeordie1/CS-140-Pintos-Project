#ifndef THREADS_READY_LIST_H
#define THREADS_READY_LIST_H

#include <list.h>
#include "threads/thread.h"

/* A priority_list is an array of lists. Each element in the
   array is a bucket containing a list of elements. The elements
   in each bucket in a list are completely arbitrary, and this is
   an abstraction from the list object. Thus, each bucket is controlled
   via the same method of control used to create lists. Because the plist
   builds off of list functionality, a data structure can only be in as
   many plists as it has list_elem members at a given time. */
struct priority_list
  {
    int size;
    struct list pl_buckets[PRI_MAX + 1];
  };

bool plist_empty (struct priority_list *pl);
void plist_init (struct priority_list *pl);
struct list_elem *plist_pop_front (struct priority_list *pl);
void plist_push_back (struct priority_list *pl, struct list_elem *e,
                      int priority);
void plist_remove (struct priority_list *pl, struct list_elem *e);
int plist_size (struct priority_list *pl);
int plist_top_priority (struct priority_list *pl);
void plist_update_elem (struct priority_list *pl, struct list_elem *e,
                        int priority);

#endif /* threads/ready-list.h */

