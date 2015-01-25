/* This file is derived from source code for the Nachos
   instructional operating system.  The Nachos copyright notice
   is reproduced in full below. */

/* Copyright (c) 1992-1996 The Regents of the University of California.
   All rights reserved.

   Permission to use, copy, modify, and distribute this software
   and its documentation for any purpose, without fee, and
   without written agreement is hereby granted, provided that the
   above copyright notice and the following two paragraphs appear
   in all copies of this software.

   IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO
   ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR
   CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE
   AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF CALIFORNIA
   HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
   PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS ON AN "AS IS"
   BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
   PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
   MODIFICATIONS.
*/

#include "threads/synch.h"
#include <stdio.h>
#include <string.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

static void give_lock (struct lock *lock, struct thread *t);
static bool cond_cmp (const struct list_elem *a, 
                      const struct list_elem *b, void *aux UNUSED);
static int next_potential_donation (struct thread *t);

void cond_sort_priority (struct condition *cond);

/* Initializes semaphore SEMA to VALUE.  A semaphore is a
   nonnegative integer along with two atomic operators for
   manipulating it:

   - down or "P": wait for the value to become positive, then
     decrement it.

   - up or "V": increment the value (and wake up one waiting
     thread, if any). */
void
sema_init (struct semaphore *sema, unsigned value) 
{
  ASSERT (sema != NULL);

  sema->value = value;
  list_init (&sema->waiters);
}

/* Down or "P" operation on a semaphore.  Waits for SEMA's value
   to become positive and then atomically decrements it.

   This function may sleep, so it must not be called within an
   interrupt handler.  This function may be called with
   interrupts disabled, but if it sleeps then the next scheduled
   thread will probably turn interrupts back on. */
void
sema_down (struct semaphore *sema) 
{
  enum intr_level old_level;

  ASSERT (sema != NULL);
  ASSERT (!intr_context ());

  old_level = intr_disable ();
  while (sema->value == 0) 
    {
      list_insert_ordered (&sema->waiters, &thread_current ()->elem,
                           thread_cmp, NULL);
      thread_current ()->thread_pl = &sema->waiters;
      thread_block ();
    }
  sema->value--;
  intr_set_level (old_level);
}

/* Down or "P" operation on a semaphore, but only if the
   semaphore is not already 0.  Returns true if the semaphore is
   decremented, false otherwise.

   This function may be called from an interrupt handler. */
bool
sema_try_down (struct semaphore *sema) 
{
  enum intr_level old_level;
  bool success;

  ASSERT (sema != NULL);

  old_level = intr_disable ();
  if (sema->value > 0) 
    {
      sema->value--;
      success = true; 
    }
  else
    success = false;
  intr_set_level (old_level);

  return success;
}

/* Up or "V" operation on a semaphore.  Increments SEMA's value
   and wakes up one thread of those waiting for SEMA, if any.

   This function may be called from an interrupt handler. */
void
sema_up (struct semaphore *sema) 
{
  enum intr_level old_level;

  ASSERT (sema != NULL);

  old_level = intr_disable ();
  sema->value++;
  if (!list_empty (&sema->waiters))
    { 
      intr_set_level (old_level);
      thread_unblock (list_entry (list_pop_front (&sema->waiters),
                      struct thread, elem));
    }
  intr_set_level (old_level);
}

static void sema_test_helper (void *sema_);

/* Self-test for semaphores that makes control "ping-pong"
   between a pair of threads.  Insert calls to printf() to see
   what's going on. */
void
sema_self_test (void) 
{
  struct semaphore sema[2];
  int i;

  printf ("Testing semaphores...");
  sema_init (&sema[0], 0);
  sema_init (&sema[1], 0);
  thread_create ("sema-test", PRI_DEFAULT, sema_test_helper, &sema);
  for (i = 0; i < 10; i++) 
    {
      sema_up (&sema[0]);
      sema_down (&sema[1]);
    }
  printf ("done.\n");
}

/* Thread function used by sema_self_test(). */
static void
sema_test_helper (void *sema_) 
{
  struct semaphore *sema = sema_;
  int i;

  for (i = 0; i < 10; i++) 
    {
      sema_down (&sema[0]);
      sema_up (&sema[1]);
    }
}

/* Initializes LOCK.  A lock can be held by at most a single
   thread at any given time.  Our locks are not "recursive", that
   is, it is an error for the thread currently holding a lock to
   try to acquire that lock.

   A lock is a specialization of a semaphore with an initial
   value of 1.  The difference between a lock and such a
   semaphore is twofold.  First, a semaphore can have a value
   greater than 1, but a lock can only be owned by a single
   thread at a time.  Second, a semaphore does not have an owner,
   meaning that one thread can "down" the semaphore and then
   another one "up" it, but with a lock the same thread must both
   acquire and release it.  When these restrictions prove
   onerous, it's a good sign that a semaphore should be used,
   instead of a lock. */
void
lock_init (struct lock *lock)
{
  ASSERT (lock != NULL);

  lock->holder = NULL;
  sema_init (&lock->semaphore, 1);
}

void
lock_donate (struct lock *lock, int donate_pri)
{
  struct thread *cur;
  int count = 0;
  for (cur = lock->holder; cur != NULL && count < 9;
       cur = cur->waiting_for_tlock) 
    {
      if (cur->priority >= donate_pri)  
        break;
      cur->priority = donate_pri;
      cur->donated = true;
      list_update_elem (cur->thread_pl,
                        &cur->elem,
                        thread_cmp);
      count++;       
    }
}

/* Helper function to give locks to a thread. */
void 
give_lock (struct lock *lock, struct thread *t)
{
  lock->holder = t; 
  t->waiting_for_tlock = NULL;
  list_push_back (&t->acquired_locks, &lock->lock_list_elem);
}

/* Acquires LOCK, sleeping until it becomes available if
   necessary.  The lock must not already be held by the current
   thread.

   This function may sleep, so it must not be called within an
   interrupt handler.  This function may be called with
   interrupts disabled, but interrupts will be turned back on if
   we need to sleep.

   If a thread with a lower priority has this lock,
   donate this priority to that set this next to 
   thread with the lock set thread with the lock's
   previous list_elem to this lock, thus negating 
   prior previous waiters */
void
lock_acquire (struct lock *lock)
{
  enum intr_level old_level; 

  ASSERT (lock != NULL);
  ASSERT (!intr_context ());
  ASSERT (!lock_held_by_current_thread (lock));

  old_level = intr_disable ();
  struct thread *t = thread_current ();
  int current_priority = t->priority;
  if (lock_try_acquire (lock))
    intr_set_level (old_level);
  else
    {
      t->waiting_for_tlock = lock->holder;
      if (current_priority > lock->holder->priority && !thread_mlfqs)
        lock_donate (lock, current_priority);
      intr_set_level (old_level);
      sema_down (&lock->semaphore);
      give_lock (lock, t);
    }
}

/* Tries to acquires LOCK and returns true if successful or false
   on failure.  The lock must not already be held by the current
   thread.

   This function will not sleep, so it may be called within an
   interrupt handler. */
bool
lock_try_acquire (struct lock *lock)
{
  bool success;
  enum intr_level old_level;

  ASSERT (lock != NULL);
  ASSERT (!lock_held_by_current_thread (lock));

  old_level = intr_disable ();
  success = sema_try_down (&lock->semaphore);
  if (success)
      give_lock (lock, thread_current ());
  intr_set_level (old_level);
  return success;
}

/* Find the next possible donation from all of the treads
   waiting on your locks. */
int
next_potential_donation (struct thread *t)
{
  int max_donatable_pri = 0;
  struct list_elem *cur_lock;
  for (cur_lock = list_begin (&t->acquired_locks);
       cur_lock != list_end (&t->acquired_locks);
       cur_lock = list_next (cur_lock))
    {
      struct lock *acquired_lock = list_entry (cur_lock, struct lock, lock_list_elem);
      int highest_pri_waiter = thread_list_top_pri (&acquired_lock->semaphore.waiters);
      if (highest_pri_waiter > max_donatable_pri)
        max_donatable_pri = highest_pri_waiter;
    }
  return max_donatable_pri;
}

/* Releases LOCK, which must be owned by the current thread.

   An interrupt handler cannot acquire a lock, so it does not
   make sense to try to release a lock within an interrupt
   handler. */
void
lock_release (struct lock *lock) 
{
  enum intr_level old_level;

  ASSERT (lock != NULL);
  ASSERT (lock_held_by_current_thread (lock));

  old_level = intr_disable ();
  struct thread *t = thread_current ();
  list_remove (&lock->lock_list_elem);
  if (!thread_mlfqs)
    {
      if (list_empty (&t->acquired_locks))
        {
          if (t->donated)
            {
              t->priority = t->original_priority;
              t->donated = false;
            }
        }
      else
        {
          int max_donatable_pri = next_potential_donation (t);
          if (t->original_priority < max_donatable_pri)
            t->priority = max_donatable_pri;
          else
            { 
              t->priority = t->original_priority;
              t->donated = false;
            }
        }
    }
  lock->holder = NULL;
  intr_set_level (old_level);
  sema_up (&lock->semaphore);
}

/* Returns true if the current thread holds LOCK, false
   otherwise.  (Note that testing whether some other thread holds
   a lock would be racy.) */
bool
lock_held_by_current_thread (const struct lock *lock) 
{
  ASSERT (lock != NULL);

  return lock->holder == thread_current ();
}

/* One semaphore in a list. */
struct semaphore_elem 
  {
    struct list_elem elem;              /* List element. */
    struct semaphore semaphore;         /* This semaphore. */
  };

/* Initializes condition variable COND.  A condition variable
   allows one piece of code to signal a condition and cooperating
   code to receive the signal and act upon it. */
void
cond_init (struct condition *cond)
{
  ASSERT (cond != NULL);
  
  list_init (&cond->waiters);
}

/* Compare the priority of two threads before inserting into a
   semaphore. */
bool
cond_cmp (const struct list_elem *a, 
          const struct list_elem *b, void *aux UNUSED)
{
  struct semaphore_elem *se_a = list_entry(a, struct semaphore_elem, elem);
  struct semaphore_elem *se_b = list_entry(b, struct semaphore_elem, elem);
  return thread_cmp (list_begin (&se_a->semaphore.waiters),
                     list_begin (&se_b->semaphore.waiters), NULL);
}

/* Atomically releases LOCK and waits for COND to be signaled by
   some other piece of code.  After COND is signaled, LOCK is
   reacquired before returning.  LOCK must be held before calling
   this function.

   The monitor implemented by this function is "Mesa" style, not
   "Hoare" style, that is, sending and receiving a signal are not
   an atomic operation.  Thus, typically the caller must recheck
   the condition after the wait completes and, if necessary, wait
   again.

   A given condition variable is associated with only a single
   lock, but one lock may be associated with any number of
   condition variables.  That is, there is a one-to-many mapping
   from locks to condition variables.

   This function may sleep, so it must not be called within an
   interrupt handler.  This function may be called with
   interrupts disabled, but interrupts will be turned back on if
   we need to sleep. */
void
cond_wait (struct condition *cond, struct lock *lock) 
{
  struct semaphore_elem waiter;

  sema_init (&waiter.semaphore, 0);

  ASSERT (cond != NULL);
  ASSERT (lock != NULL);
  ASSERT (!intr_context ());
  ASSERT (lock_held_by_current_thread (lock));
  
  sema_init (&waiter.semaphore, 0);
  list_insert_ordered (&cond->waiters, &waiter.elem,
                       cond_cmp, NULL);
  lock_release (lock);
  sema_down (&waiter.semaphore);
  lock_acquire (lock);
}

/* If any threads are waiting on COND (protected by LOCK), then
   this function signals one of them to wake up from its wait.
   LOCK must be held before calling this function.

   An interrupt handler cannot acquire a lock, so it does not
   make sense to try to signal a condition variable within an
   interrupt handler. */
void
cond_signal (struct condition *cond, struct lock *lock UNUSED) 
{
  ASSERT (cond != NULL);
  ASSERT (lock != NULL);
  ASSERT (!intr_context ());
  ASSERT (lock_held_by_current_thread (lock));

  cond_sort_priority (cond);
  if (!list_empty (&cond->waiters))  
    sema_up (&list_entry (list_pop_front (&cond->waiters),
                          struct semaphore_elem, elem)->semaphore);
}

/* Wakes up all threads, if any, waiting on COND (protected by
   LOCK).  LOCK must be held before calling this function.

   An interrupt handler cannot acquire a lock, so it does not
   make sense to try to signal a condition variable within an
   interrupt handler. */
void
cond_broadcast (struct condition *cond, struct lock *lock) 
{
  ASSERT (cond != NULL);
  ASSERT (lock != NULL);

  while (!list_empty (&cond->waiters))
    cond_signal (cond, lock);
}

/* Sort all of the semaphores with single threads waiting on
   them in the condition by priority. */
void 
cond_sort_priority (struct condition *cond)
{
  list_sort (&cond->waiters, cond_cmp, NULL);
}
