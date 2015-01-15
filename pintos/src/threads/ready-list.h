#ifndef THREADS_READY_LIST_H
#define THREADS_READY_LIST_H

#include "threads/thread.h"

void ready_list_init (void);
void ready_list_push_back (struct list_elem *e, int priority);
void ready_list_remove (struct list_elem *, int priority);
struct thread *ready_list_pop_front (void);
int ready_list_top_priority (void);
bool ready_list_empty (void);

#endif /* threads/ready-list.h */

