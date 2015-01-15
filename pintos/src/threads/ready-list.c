#include "threads/ready-list.h"
#include "threads/thread.h"

/* New ready list based on priority. */
static struct list priority_ready_list;

void 
ready_list_init (void)
{
  list_init (&priority_ready_list);
  int i = 0;
  for (i = 0; i < PRI_MAX; i++)
  {
    struct list priority_list; // do I need to malloc?
    list_init (&priority_list);
    list_push_back (&priority_ready_list, &(&priority_list)->head);
  }
}

/* Adds a thread to the ready list. Uses the thread's priority to place the
   element in the correct list. */
void
ready_list_push_back (struct list_elem *e, int priority)
{
  // TODO(elizabeth): Throw errors if < PRI_MIN or > PRI_MAX
  if (priority < PRI_MIN || priority > PRI_MAX) return;
  int i = 0;
  int priority_index = PRI_MAX - priority;
  for (e = list_begin (&priority_ready_list); 
       e != list_end (&priority_ready_list); e = list_next(e))
  {
    if (i == priority_index)
    {
      struct list *priority_list = list_entry (e, struct list, head);
      list_push_back(priority_list, &thread_current()->allelem);
      return;
    }
    i++;
  }
}

/* Removes a thread from the ready list. Uses the thread's priority to remove
   the element from the correct list. */
void
ready_list_remove (struct list_elem *e, int priority)
{
  list_remove (&thread_current()->allelem);
}

/* Pops a thread from the the highest priority list that contains a thread. */
struct thread *
ready_list_pop_front (void)
{
  struct list_elem *e;
   for (e = list_begin (&priority_ready_list); 
        e != list_end (&priority_ready_list); e = list_next(e))
   {
       struct list *priority_list = list_entry (e, struct list, head);
       if (!list_empty (priority_list))
       {
         return list_entry (list_pop_front (priority_list), 
           struct thread, elem);
       }

   }
   return NULL;
   // TODO(elizabeth): throw an error 
}

/* Returns a boolean detailing whether the ready list is empty. Iterates
    through the priority ready lists of ready lists to check whether each list
    is empty. If so returns false, else returns true. */
bool
ready_list_empty (void)
{
  struct list_elem *e;
  for (e = list_begin (&priority_ready_list); 
       e != list_end (&priority_ready_list); e = list_next(e))
  {
      struct list *priority_list = list_entry (e, struct list, head);
      if (!list_empty (priority_list))
        return false;
  }
  return true;
}

int
ready_list_top_priority (void)
{
  int i = 0;
  struct list_elem *e;
  for (e = list_begin (&priority_ready_list); 
       e != list_end (&priority_ready_list); e = list_next(e))
  {
    struct list *priority_list = list_entry (e, struct list, head);
    if (!list_empty (priority_list))
      return (PRI_MAX - i);
    i++;
  }
  return PRI_MIN;
}