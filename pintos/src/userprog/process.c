#include "userprog/process.h"
#include <debug.h>
#include <inttypes.h>
#include <round.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "userprog/gdt.h"
#include "userprog/pagedir.h"
#include "userprog/tss.h"
#include "userprog/syscall.h"
#include "filesys/directory.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "threads/flags.h"
#include "threads/init.h"
#include "threads/interrupt.h"
#include "threads/malloc.h"
#include "threads/palloc.h"
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "threads/synch.h"
#ifdef VM
#include "vm/frame.h"
#endif

#define P_RUNNING 0
#define P_EXITED 1
#define P_LOADING 2

struct proc_args 
  {
    char *file_name_;
    char *relative_dir_path;
    struct relation *rel;
  };

static thread_func start_process NO_RETURN;
//static void process_pass_reldir (char **rel_dir UNUSED);
static bool load (const char *file_name, char *cmdline,
                  void (**eip) (void), void **esp);

const char *ignore_delimiters = " \t\r";

/*static void
process_pass_reldir (char **rel_dir UNUSED)
{
//  process_args.relative_dir_path = (NAME_MAX + 1) * num_dirs;
  return;
}
*/
/* Starts a new thread running a user program loaded from
   FILENAME.  The new thread may be scheduled (and may even exit)
   before process_execute() returns.  Returns the new process's
   thread id, or TID_ERROR if the thread cannot be created. */
tid_t
process_execute (const char *file_name) 
{
  char *fn_copy;
  struct proc_args process_args;
  tid_t tid;

  /* Make a copy of FILE_NAME.
     Otherwise there's a race between the caller and load(). */
  fn_copy = palloc_get_page (0);
  if (fn_copy == NULL)
    return TID_ERROR;
  strlcpy (fn_copy, file_name, PGSIZE);

  struct relation *rel = malloc (sizeof (struct relation));
  list_push_back (&thread_current ()->children_in_r, &rel->elem);
  rel->status_lock = &thread_current ()->children_lock;
  rel->p_status = P_RUNNING;
  rel->c_status = P_LOADING;
  rel->w_status = -1;

  process_args.file_name_ = fn_copy;
  process_args.rel = rel;

  /* Create a new thread to execute FILE_NAME. */
  tid = thread_create (file_name, PRI_DEFAULT, start_process, &process_args);
  rel->c_id = tid;
  lock_acquire (rel->status_lock);
  while (rel->c_status == P_LOADING)
    {
      lock_release (rel->status_lock);
      thread_yield ();
      lock_acquire (rel->status_lock);
    }
  if (rel->c_status == P_EXITED)
    tid = rel->c_id;
  lock_release (rel->status_lock);
  return tid;
}

/* A thread function that loads a user process and starts it
   running. */
static void
start_process (void *process_args)
{
  struct thread *t = thread_current ();
  char *cmdline;
  char *file_name = strtok_r (((struct proc_args *) process_args)->file_name_,
                              ignore_delimiters,
                              &cmdline);
  t->parent_in_r = ((struct proc_args *) process_args)->rel;

/*  char root = '\0';
  if (strcmp(((struct proc_args *) process_args)->relative_dir_path, &root)!=
      0)
    {
      mkdir (((struct proc_args *) process_args)->relative_dir_path);
      t->curr_dirnum = open (((struct proc_args *) 
                             process_args)->relative_dir_path);
    }
*/
  struct intr_frame if_;
  bool success;

  /* Initialize interrupt frame and load executable. */
  memset (&if_, 0, sizeof if_);
  if_.gs = if_.fs = if_.es = if_.ds = if_.ss = SEL_UDSEG;
  if_.cs = SEL_UCSEG;
  if_.eflags = FLAG_IF | FLAG_MBS;

  lock_acquire (&fs_lock);
  success = load (file_name, cmdline, &if_.eip, &if_.esp);
  lock_release (&fs_lock);

  palloc_free_page (file_name);

  /* If load failed, quit. */
  lock_acquire (t->parent_in_r->status_lock);
  if (!success)
    {
      t->parent_in_r->c_status = P_EXITED;
      t->parent_in_r->c_id = TID_ERROR;
      lock_release (t->parent_in_r->status_lock);
      thread_exit ();
    }
  t->parent_in_r->c_status = P_RUNNING;
  lock_release (t->parent_in_r->status_lock); 

  /* Start the user process by simulating a return from an
     interrupt, implemented by intr_exit (in
     threads/intr-stubs.S).  Because intr_exit takes all of its
     arguments on the stack in the form of a `struct intr_frame',
     we just point the stack pointer (%esp) to our stack frame
     and jump to it. */
  asm volatile ("movl %0, %%esp; jmp intr_exit" : : "g" (&if_) : "memory");
  NOT_REACHED ();
}

/* Waits for thread TID to die and returns its exit status.  If
   it was terminated by the kernel (i.e. killed due to an
   exception), returns -1.  If TID is invalid or if it was not a
   child of the calling process, or if process_wait() has already
   been successfully called for the given TID, returns -1
   immediately, without waiting.

   This function will be implemented in problem 2-2.  For now, it
   does nothing. */
int
process_wait (tid_t child_tid) 
{
  bool success = false;
  struct thread *t = thread_current ();
  struct relation *rel;
  struct list_elem *rel_iter;
  /* Find the relation with the tid. */ 
  for (rel_iter = list_begin (&t->children_in_r);
       rel_iter != list_end (&t->children_in_r);
       rel_iter = list_next (rel_iter))
    {
      rel = list_entry (rel_iter, struct relation, elem);
      if (rel->c_id == child_tid)
        {
          success = true;
          break;
        }
    }
  /* Exit because that's not our child! */
  if (!success)
    return -1;

  /* Wait for the child to exit. */
  lock_acquire (rel->status_lock);
  while (rel->c_status == P_RUNNING)
    {
      lock_release (rel->status_lock);
      thread_yield ();
      lock_acquire (rel->status_lock);
    }
  int status = rel->w_status;
  list_remove (&rel->elem);
  lock_release (rel->status_lock);
  free (rel);
  return status;
}

/* Free the current process's resources. */
void
process_exit (void)
{
  struct thread *cur = thread_current ();
  uint32_t *pd;

  printf ("%s: exit(%d)\n", cur->exec_name, cur->parent_in_r->w_status);

  if (cur->exec)
    {
      lock_acquire (&fs_lock);
      file_close (cur->exec);
      lock_release (&fs_lock);
    }
  if (cur->exec_name)
    free (cur->exec_name);

  /* Cut ties with parent. */
  if (cur->parent_in_r != NULL)
    {
      lock_acquire (cur->parent_in_r->status_lock);
      cur->parent_in_r->c_status = P_EXITED;
      if (cur->parent_in_r->p_status == P_EXITED)
        {
          lock_release (cur->parent_in_r->status_lock);
          free (cur->parent_in_r);
        }
      else
        lock_release (cur->parent_in_r->status_lock);
    }

  /* Orphan all children. */
  struct list_elem *rel_iter;
  while (!list_empty (&cur->children_in_r))
    {
      rel_iter = list_begin (&cur->children_in_r);
      struct relation *rel = list_entry (rel_iter, struct relation, elem);
      lock_acquire (rel->status_lock);
      rel->p_status = P_EXITED;
      list_remove (&rel->elem);
      if (rel->c_status == P_EXITED)
        {
          lock_release (rel->status_lock);
          free (rel);
        }
      else
        lock_release (rel->status_lock);
    }
 
  /* Close all fds in fdt. */
  lock_acquire (&fs_lock);
  hash_destroy (&cur->fd_hash, fdt_close);
  lock_release (&fs_lock);

  /* Release all locks. */
  struct list_elem *del_entry;
  while (!list_empty (&cur->acquired_locks))
    {
      del_entry = list_begin (&cur->acquired_locks);
      struct lock *del_lock = list_entry (del_entry,
                                          struct lock,
                                          lock_list_elem);
      lock_release (del_lock);
    }
 
  /* Destroy the current process's page directory and switch back
     to the kernel-only page directory. */
  pd = cur->pagedir;
  if (pd != NULL) 
    {
      /* Correct ordering here is crucial.  We must set
         cur->pagedir to NULL before switching page directories,
         so that a timer interrupt can't switch back to the
         process page directory.  We must activate the base page
         directory before destroying the process's page
         directory, or our active page directory will be one
         that's been freed (and cleared). */
      cur->pagedir = NULL;
      pagedir_activate (NULL);
      #ifdef VM
        page_supp_destroy (cur);
      #endif
      pagedir_destroy (pd);
    }
}

/* Sets up the CPU for running user code in the current
   thread.
   This function is called on every context switch. */
void
process_activate (void)
{
  struct thread *t = thread_current ();

  /* Activate thread's page tables. */
  pagedir_activate (t->pagedir);

  /* Set thread's kernel stack for use in processing
     interrupts. */
  tss_update ();
}

/* We load ELF binaries.  The following definitions are taken
   from the ELF specification, [ELF1], more-or-less verbatim.  */

/* ELF types.  See [ELF1] 1-2. */
typedef uint32_t Elf32_Word, Elf32_Addr, Elf32_Off;
typedef uint16_t Elf32_Half;

/* For use with ELF types in printf(). */
#define PE32Wx PRIx32   /* Print Elf32_Word in hexadecimal. */
#define PE32Ax PRIx32   /* Print Elf32_Addr in hexadecimal. */
#define PE32Ox PRIx32   /* Print Elf32_Off in hexadecimal. */
#define PE32Hx PRIx16   /* Print Elf32_Half in hexadecimal. */

/* Executable header.  See [ELF1] 1-4 to 1-8.
   This appears at the very beginning of an ELF binary. */
struct Elf32_Ehdr
  {
    unsigned char e_ident[16];
    Elf32_Half    e_type;
    Elf32_Half    e_machine;
    Elf32_Word    e_version;
    Elf32_Addr    e_entry;
    Elf32_Off     e_phoff;
    Elf32_Off     e_shoff;
    Elf32_Word    e_flags;
    Elf32_Half    e_ehsize;
    Elf32_Half    e_phentsize;
    Elf32_Half    e_phnum;
    Elf32_Half    e_shentsize;
    Elf32_Half    e_shnum;
    Elf32_Half    e_shstrndx;
  };

/* Program header.  See [ELF1] 2-2 to 2-4.
   There are e_phnum of these, starting at file offset e_phoff
   (see [ELF1] 1-6). */
struct Elf32_Phdr
  {
    Elf32_Word p_type;
    Elf32_Off  p_offset;
    Elf32_Addr p_vaddr;
    Elf32_Addr p_paddr;
    Elf32_Word p_filesz;
    Elf32_Word p_memsz;
    Elf32_Word p_flags;
    Elf32_Word p_align;
  };

/* Values for p_type.  See [ELF1] 2-3. */
#define PT_NULL    0            /* Ignore. */
#define PT_LOAD    1            /* Loadable segment. */
#define PT_DYNAMIC 2            /* Dynamic linking info. */
#define PT_INTERP  3            /* Name of dynamic loader. */
#define PT_NOTE    4            /* Auxiliary info. */
#define PT_SHLIB   5            /* Reserved. */
#define PT_PHDR    6            /* Program header table. */
#define PT_STACK   0x6474e551   /* Stack segment. */

/* Flags for p_flags.  See [ELF3] 2-3 and 2-4. */
#define PF_X 1          /* Executable. */
#define PF_W 2          /* Writable. */
#define PF_R 4          /* Readable. */

static bool setup_stack (void **esp);
static void push_stack_args (const char *file_name, char *cmdline, void **esp);
static void push (void **esp, void *adr, size_t num_bytes);
static int parse_cmd_ln (char *cmdline, void **esp);
static bool validate_segment (const struct Elf32_Phdr *, struct file *);
static bool load_segment (struct file *file, off_t ofs, uint8_t *upage,
                          uint32_t read_bytes, uint32_t zero_bytes,
                          bool writable);

/* Loads an ELF executable from FILE_NAME into the current thread.
   Stores the executable's entry point into *EIP
   and its initial stack pointer into *ESP.
   Returns true if successful, false otherwise. */
bool
load (const char *file_name, char *cmdline, void (**eip) (void), void **esp) 
{
  struct thread *t = thread_current ();
  struct Elf32_Ehdr ehdr;
  struct file *file = NULL;
  off_t file_ofs;
  bool success = false;
  int i;

  hash_init (&t->fd_hash, fdt_hash, fdt_cmp, NULL);

  /* Allocate and activate page directory. */
  t->pagedir = pagedir_create ();
  if (t->pagedir == NULL) 
    goto done;
  process_activate ();

  /* Copy file name into TCB. */
  t->exec_name = malloc (strnlen (file_name, PGSIZE - 1) + 1);
  strlcpy (t->exec_name, file_name, PGSIZE - 1); 

  /* Open executable file. */
  file = filesys_open (file_name);
  if (file == NULL) 
    {
      printf ("load: %s: open failed\n", file_name);
      goto done; 
    }
  t->exec = file;

  /* Read and verify executable header. */
  if (file_read (file, &ehdr, sizeof ehdr) != sizeof ehdr
      || memcmp (ehdr.e_ident, "\177ELF\1\1\1", 7)
      || ehdr.e_type != 2
      || ehdr.e_machine != 3
      || ehdr.e_version != 1
      || ehdr.e_phentsize != sizeof (struct Elf32_Phdr)
      || ehdr.e_phnum > 1024) 
    {
      printf ("load: %s: error loading executable\n", file_name);
      goto done; 
    }

  /* Set Thread state on current executable running. */
  file_deny_write (file);

  /* Read program headers. */
  file_ofs = ehdr.e_phoff;
  for (i = 0; i < ehdr.e_phnum; i++) 
    {
      struct Elf32_Phdr phdr;

      if (file_ofs < 0 || file_ofs > file_length (file))
        goto done;
      file_seek (file, file_ofs);

      if (file_read (file, &phdr, sizeof phdr) != sizeof phdr)
        goto done;
      file_ofs += sizeof phdr;
      switch (phdr.p_type) 
        {
        case PT_NULL:
        case PT_NOTE:
        case PT_PHDR:
        case PT_STACK:
        default:
          /* Ignore this segment. */
          break;
        case PT_DYNAMIC:
        case PT_INTERP:
        case PT_SHLIB:
          goto done;
        case PT_LOAD:
          if (validate_segment (&phdr, file)) 
            {
              bool writable = (phdr.p_flags & PF_W) != 0;
              uint32_t file_page = phdr.p_offset & ~PGMASK;
              uint32_t mem_page = phdr.p_vaddr & ~PGMASK;
              uint32_t page_offset = phdr.p_vaddr & PGMASK;
              uint32_t read_bytes, zero_bytes;
              if (phdr.p_filesz > 0)
                {
                  /* Normal segment.
                     Read initial part from disk and zero the rest. */
                  read_bytes = page_offset + phdr.p_filesz;
                  zero_bytes = (ROUND_UP (page_offset + phdr.p_memsz, PGSIZE)
                                - read_bytes);
                }
              else 
                {
                  /* Entirely zero.
                     Don't read anything from disk. */
                  read_bytes = 0;
                  zero_bytes = ROUND_UP (page_offset + phdr.p_memsz, PGSIZE);
                }
              if (!load_segment (file, file_page, (void *) mem_page,
                                 read_bytes, zero_bytes, writable))
                goto done;
            }
          else
            goto done;
          break;
        }
    }

  /* Set up stack. */
  if (!setup_stack (esp))
      goto done;

  /* Push stack values */
  push_stack_args (file_name, cmdline, esp);

  /* Start address. */
  *eip = (void (*) (void)) ehdr.e_entry;

  success = true;

 done:
  /* We arrive here whether the load is successful or not. */
  return success;
}

#ifndef VM
static bool install_page (void *upage, void *kpage, bool writable);
#endif

/* Checks whether PHDR describes a valid, loadable segment in
   FILE and returns true if so, false otherwise. */
static bool
validate_segment (const struct Elf32_Phdr *phdr, struct file *file) 
{
  /* p_offset and p_vaddr must have the same page offset. */
  if ((phdr->p_offset & PGMASK) != (phdr->p_vaddr & PGMASK)) 
    return false; 

  /* p_offset must point within FILE. */
  if (phdr->p_offset > (Elf32_Off) file_length (file)) 
    return false;

  /* p_memsz must be at least as big as p_filesz. */
  if (phdr->p_memsz < phdr->p_filesz) 
    return false; 

  /* The segment must not be empty. */
  if (phdr->p_memsz == 0)
    return false;
  
  /* The virtual memory region must both start and end within the
     user address space range. */
  if (!is_user_vaddr ((void *) phdr->p_vaddr))
    return false;
  if (!is_user_vaddr ((void *) ((char *)phdr->p_vaddr + phdr->p_memsz)))
    return false;

  /* The region cannot "wrap around" across the kernel virtual
     address space. */
  if (phdr->p_vaddr + phdr->p_memsz < phdr->p_vaddr)
    return false;

  /* Disallow mapping page 0.
     Not only is it a bad idea to map page 0, but if we allowed
     it then user code that passed a null pointer to system calls
     could quite likely panic the kernel by way of null pointer
     assertions in memcpy(), etc. */
  if (phdr->p_vaddr < PGSIZE)
    return false;

  /* It's okay. */
  return true;
}

/* Loads a segment starting at offset OFS in FILE at address
   UPAGE.  In total, READ_BYTES + ZERO_BYTES bytes of virtual
   memory are initialized, as follows:

        - READ_BYTES bytes at UPAGE must be read from FILE
          starting at offset OFS.

        - ZERO_BYTES bytes at UPAGE + READ_BYTES must be zeroed.

   The pages initialized by this function must be writable by the
   user process if WRITABLE is true, read-only otherwise.

   Return true if successful, false if a memory allocation error
   or disk read error occurs. */
static bool
load_segment (struct file *file, off_t ofs, uint8_t *upage,
              uint32_t read_bytes, uint32_t zero_bytes, bool writable) 
{
  ASSERT ((read_bytes + zero_bytes) % PGSIZE == 0);
  ASSERT (pg_ofs (upage) == 0);
  ASSERT (ofs % PGSIZE == 0);
  
  #ifdef VM
    struct thread *t = thread_current ();
    size_t page_no = 0;
  #endif
  file_seek (file, ofs);
  while (read_bytes > 0 || zero_bytes > 0) 
    {
      /* Calculate how to fill this page.
         We will read PAGE_READ_BYTES bytes from FILE
         and zero the final PAGE_ZERO_BYTES bytes. */
      size_t page_read_bytes = read_bytes < PGSIZE ? read_bytes : PGSIZE;
      size_t page_zero_bytes = PGSIZE - page_read_bytes;

      /* Get a page of memory. */
      #ifdef VM
        struct sp_entry *spe = page_supp_alloc (t, upage);
        spe->read_bytes = page_read_bytes;
        spe->zero_bytes = page_zero_bytes;
        spe->ofs = ofs + page_no * PGSIZE;
        spe->writable = writable;
        spe->fp = file;
        spe->location = FILESYSTEM;
        spe->upage = upage;
        page_no++;
      #else
        uint8_t *kpage = palloc_get_page (PAL_USER);
        if (kpage == NULL)
          return false;

        /* Load this page. */
        if (file_read (file, kpage, page_read_bytes) != (int) page_read_bytes)
          {
            palloc_free_page (kpage);
            return false; 
          }
        memset (kpage + page_read_bytes, 0, page_zero_bytes);

        /* Add the page to the process's address space. */
        if (!install_page (upage, kpage, writable)) 
          {
            palloc_free_page (kpage);
            return false; 
          }
      #endif
      /* Advance. */
      read_bytes -= page_read_bytes;
      zero_bytes -= page_zero_bytes;
      upage += PGSIZE;
    }
  return true;
}

/* Create a minimal stack by mapping a zeroed page at the top of
   user virtual memory. */
static bool
setup_stack (void **esp) 
{
  bool success = false;

  #ifdef VM
    struct sp_entry *spe;
    spe = page_supp_alloc (thread_current (), (uint8_t *) PHYS_BASE - PGSIZE);
    spe->writable = true;
    spe->location = UNMAPPED;
    success = frame_obtain (spe);
    if (success)
      *esp = PHYS_BASE;
    else
      page_supp_delete (spe);
  #else
    uint8_t *kpage;
    kpage = palloc_get_page (PAL_USER | PAL_ZERO);
    if (kpage != NULL) 
      {
        success = install_page (((uint8_t *) PHYS_BASE) - PGSIZE, kpage, true);
        if (success)
          *esp = PHYS_BASE;
        else
          palloc_free_page (kpage);
      }
  #endif
  return success;
}

/* Push all of the relevant values from the cmd 
   line to the stack. */
void
push_stack_args (const char *file_name, char *cmdline, void **esp)
{
  push (esp, (void *) file_name, strnlen (file_name, PGSIZE) + 1);
  int argc = parse_cmd_ln (cmdline, esp) + 1;
  void *argv_data_ptr = *(void **) esp;
  push (esp, 0, ((uint32_t) *esp) % 4);
  push (esp, 0, sizeof (char*));
  int curr_str;
  for (curr_str = 0; curr_str < argc; curr_str++)
  {
    push (esp, &argv_data_ptr, sizeof (char *));
    argv_data_ptr += strnlen (argv_data_ptr, PGSIZE) + 1;  
  }
  void *argv_ptr_strt = *(void **) esp;
  push (esp, &argv_ptr_strt, sizeof (char **));
  push (esp, &argc, sizeof (int *));
  push (esp, 0, sizeof (void *));
}

/* Push memory onto the stack. */
void
push (void **esp, void *adr, size_t num_bytes)
{
  *esp = (*(char **) esp) - num_bytes;
  if (adr != NULL)
    memcpy (*esp, adr, num_bytes);
  else
    memset (*esp, 0, num_bytes);
}

/* Split the command line into words separated only by '\0'
   null characters. */
int
parse_cmd_ln (char *cmdline, void **esp)
{
  int count = 0;
  char *token, *save_ptr;
  for (token = strtok_r (cmdline, ignore_delimiters, &save_ptr);
       token != NULL;
       token = strtok_r (NULL, ignore_delimiters, &save_ptr))
    {
      push (esp, token, strnlen (token, PGSIZE) + 1);
      count++;
    }
  return count;
}

#ifndef VM
/* adds a mapping from user virtual address upage to kernel
   virtual address kpage to the page table.
   if writable is true, the user process may modify the page;
   otherwise, it is read-only.
   upage must not already be mapped.
   kpage should probably be a page obtained from the user pool
   with palloc_get_page().
   returns true on success, false if upage is already mapped or
   if memory allocation fails. */
static bool
install_page (void *upage, void *kpage, bool writable)
{
  struct thread *t = thread_current ();

  /* verify that there's not already a page at that virtual
     address, then map our page there. */
  return (pagedir_get_page (t->pagedir, upage) == NULL
          && pagedir_set_page (t->pagedir, upage, kpage, writable));
}
#endif
