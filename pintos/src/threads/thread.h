#ifndef THREADS_THREAD_H
#define THREADS_THREAD_H

#include <debug.h>
#include <list.h>
#include <stdint.h>
#include "threads/fixed-point.h"
#include "threads/vaddr.h"
#include "filesys/cache.h"
#include "filesys/file.h"

/* States in a thread's life cycle. */
enum thread_status
  {
    THREAD_RUNNING,     /* Running thread. */
    THREAD_READY,       /* Not running but ready to run. */
    THREAD_BLOCKED,     /* Waiting for an event to trigger. */
    THREAD_DYING        /* About to be destroyed. */
  };

/* Thread identifier type.
   You can redefine this to whatever type you like. */
typedef int tid_t;
#define TID_ERROR ((tid_t) -1)          /* Error value for tid_t. */

#ifdef USERPROG

#include <hash.h>
#include "threads/synch.h"

struct relation
  {
    tid_t c_id;                 /* Process ID of child. */
    int p_status;               /* Hold the parent's status. */
    int c_status;               /* Hold the child's status. */
    int w_status;               /* Hold the status of child on exit. */
    struct lock *status_lock;   /* Hold this lock to read/write status. */
    struct list_elem elem;      /* One relationship in a list. */ 
  };

struct lock fs_lock;

struct file_descriptor
  {
    int fd;
    struct file *file_;
    struct hash_elem elem; 
  };

bool fdt_cmp (const struct hash_elem *a,
              const struct hash_elem *b,
              void *aux UNUSED);
void fdt_close (struct hash_elem *fd_entry, void *aux UNUSED);
bool fdt_insert (struct hash *fdt_hash, struct file_descriptor *fdt_entry);
unsigned fdt_hash (const struct hash_elem *e, void *aux UNUSED);
struct hash_elem *fdt_search (struct hash *fdt_hash, int fd);

#endif

/* Thread priorities. */
#define PRI_MIN 0                       /* Lowest priority. */
#define PRI_DEFAULT 31                  /* Default priority. */
#define PRI_MAX 63                      /* Highest priority. */

/* A kernel thread or user process.

   Each thread structure is stored in its own 4 kB page.  The
   thread structure itself sits at the very bottom of the page
   (at offset 0).  The rest of the page is reserved for the
   thread's kernel stack, which grows downward from the top of
   the page (at offset 4 kB).  Here's an illustration:

        4 kB +---------------------------------+
             |          kernel stack           |
             |                |                |
             |                |                |
             |                V                |
             |         grows downward          |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             |                                 |
             +---------------------------------+
             |              magic              |
             |                :                |
             |                :                |
             |               name              |
             |              status             |
        0 kB +---------------------------------+

   The upshot of this is twofold:

      1. First, `struct thread' must not be allowed to grow too
         big.  If it does, then there will not be enough room for
         the kernel stack.  Our base `struct thread' is only a
         few bytes in size.  It probably should stay well under 1
         kB.

      2. Second, kernel stacks must not be allowed to grow too
         large.  If a stack overflows, it will corrupt the thread
         state.  Thus, kernel functions should not allocate large
         structures or arrays as non-static local variables.  Use
         dynamic allocation with malloc() or palloc_get_page()
         instead.

   The first symptom of either of these problems will probably be
   an assertion failure in thread_current(), which checks that
   the `magic' member of the running thread's `struct thread' is
   set to THREAD_MAGIC.  Stack overflow will normally change this
   value, triggering the assertion. */
struct thread
  {
    /* Owned by thread.c. */
    tid_t tid;                          /* Thread identifier. */
    enum thread_status status;          /* Thread state. */
    char name[16];                      /* Name (for debugging purposes). */
    uint8_t *stack;                     /* Saved stack pointer. */
    int priority;                       /* Priority that doesn't change from
                                           donations. */
    int original_priority;              /* Priority of the thread before 
                                           donations. */
    struct list_elem allelem;           /* List element for all threads list. */
    int nice;                           /* How nice a thread is to let other
                                           threads schedule. */
    fp recent_cpu;                      /* Recently used CPU by thread. */
    int64_t start;                      /* Beginning of when thread went to
                                           sleep last. */
    int64_t sleep;                      /* Number of ticks we are waiting for. */
    bool recently_up;                   /* Boolean to let thread mlfqs know 
                                           when we update. */
    bool donated;                       /* Boolean to let thread donation know
                                           when we get donated to. */

    struct list *thread_pl;             /* Priority list that currently
                                           contains this thread. */
    struct thread *waiting_for_tlock;   /* Lock that the thread is waiting on. */ 
    struct list acquired_locks;         /* List of locks that this thread has
                                           acquired. */
  
    /* Shared between thread.c and synch.c. */
    struct list_elem elem;              /* List element. */

#ifdef USERPROG

    /* Owned by userprog/process.c. */
    struct file *exec;                  /* File pointer to the process executable file. */
    char *exec_name;                    /* Name of the process executable file. */
    uint32_t *pagedir;                  /* Page directory. */
    struct hash fd_hash;                /* Table of all file descriptors. */

    struct lock children_lock;          /* Lock that every child uses to synchronize. */
    struct list children_in_r;          /* This thread is a parent to this relationship. */
    struct relation *parent_in_r;       /* This thread is a child to this relationship. */

#endif

#ifdef VM
    struct list spe_list;               /* List of spt entries used by this thread. */
    void *saved_esp;                    /* Saved esp for page faults in kernel. */
#endif

#ifdef FILESYS
  
    int curr_dirnum;                    /* Fd of current_working directory. */

#endif

    /* Owned by thread.c. */
    unsigned magic;                     /* Detects stack overflow. */
  };


/* If false (default), use round-robin scheduler.
   If true, use multi-level feedback queue scheduler.
   Controlled by kernel command-line option "-o mlfqs". */
extern bool thread_mlfqs;

void thread_init (void);
void thread_start (void);

void thread_tick (void);
void thread_print_stats (void);

typedef void thread_func (void *aux);
tid_t thread_create (const char *name, int priority, thread_func *, void *);

bool thread_cmp (const struct list_elem *a, 
                 const struct list_elem *b, void *aux UNUSED);
int thread_list_top_pri (struct list *list);

void thread_block (void);
void thread_unblock (struct thread *);
void thread_sleep (void);

struct thread *thread_current (void);
tid_t thread_tid (void);
const char *thread_name (void);

void thread_exit (void) NO_RETURN;
void thread_yield (void);

/* Performs some operation on thread t, given auxiliary data AUX. */
typedef void thread_action_func (struct thread *t, void *aux);
void thread_foreach (thread_action_func *, void *);
void thread_update_timers (void);
int thread_get_priority (void);
void thread_set_priority (int);

int thread_get_nice (void);
void thread_set_nice (int);

int thread_get_recent_cpu (void);

int thread_get_load_avg (void);

#endif /* threads/thread.h */
