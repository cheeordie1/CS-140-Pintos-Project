#include "userprog/syscall.h"
#include <stdio.h>
#include <string.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/malloc.h"
#include "threads/synch.h"
#include "threads/thread.h"
#ifdef VM
#include "threads/vaddr.h"
#include "vm/frame.h"
#include "vm/page.h"
#endif
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "devices/input.h"
#include "devices/shutdown.h"
#include "lib/kernel/hash.h"
#include "userprog/pagedir.h"
#include "vm/frame.h"

#define STACK_LIMIT 0x00800000 

static bool is_valid_ptr (const void *ptr, size_t len);
static bool is_valid_mapping (const void *ptr, size_t len);
#ifdef VM
static void validate_esp (void *esp);
#endif
static void syscall_handler (struct intr_frame *);
static void syscall_halt (void);
static void syscall_exit (int status);
static pid_t syscall_exec (const char *cmd_line);
static int syscall_wait (pid_t pid);
static bool syscall_create (const char *file, uint32_t initial_size);
static bool syscall_remove (const char* file);
static int syscall_open (const char* file);
static int syscall_filesize (int fd);
static int syscall_read (int fd, void *buf, uint32_t size);
static int syscall_write (int fd, const void* buf, uint32_t size);
static void syscall_seek (int fd, uint32_t position);
static uint32_t syscall_tell (int fd);
static void syscall_close (int fd);
#ifdef VM
static mapid_t syscall_mmap (int fd, void *addr);
static void syscall_munmap (mapid_t mapid);
#endif
static void *syscall_arg (void *esp, int index);

/* Initialize system calls. */
void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
  lock_init (&fs_lock);
}

/* Terminates Pintos by calling power_off() (declared in "threads/init.h").
   This should be seldom used, because you lose some information about 
    possible deadlock situations, etc. */
static void 
syscall_halt (void) 
{
  shutdown_power_off ();
}

/* Terminates the current user program, returning status to the kernel. If the
   process's parent waits for it, this is the status that will be
   returned. Conventionally, a status of 0 indicates success and nonzero values
   indicate errors. */
static void
syscall_exit (int status)
{
  thread_current ()->parent_in_r->w_status = status;
  thread_exit ();
}

/* Runs the executable whose name is given in cmd_line, passing any given
   arguments, and returns the new process's program id (pid). Must return pid
   -1, which otherwise should not be a valid pid, if the program cannot load or
   run for any reason. Thus, the parent process cannot return from the exec
   until it knows whether the child process successfully loaded its executable.
   You must use appropriate synchronization to ensure this. */
static pid_t
syscall_exec (const char *cmd_line)
{
  if (!is_valid_ptr (cmd_line, 0))
    syscall_exit (-1);
  pid_t process = process_execute (cmd_line);
  return process;
}

/* Waits for a child process pid and retrieves the child's exit status.

   If pid is still alive, waits until it terminates. Then, returns the status
   that pid passed to exit. If pid did not call exit(), but was terminated by
   the kernel (e.g. killed due to an exception), wait(pid) must return -1. It
   is perfectly legal for a parent process to wait for child processes that
   have already terminated by the time the parent calls wait, but the kernel
   must still allow the parent to retrieve its child's exit status, or learn
   that the child was terminated by the kernel. */
static int
syscall_wait (pid_t pid)
{
  if (pid == TID_ERROR)
    return -1;
  int status = process_wait (pid);
  return status;
}

/* Creates a new file called file initially initial_size bytes in size. Returns
   true if successful, false otherwise. Creating a new file does not open it:
   opening the new file is a separate operation which would require a open
   system call. */
static bool
syscall_create (const char *file, uint32_t initial_size)
{
  if (!is_valid_ptr (file, 0))
    syscall_exit (-1); 
  bool success;
  lock_acquire (&fs_lock);
  success = filesys_create (file, (off_t) initial_size);
  lock_release (&fs_lock);
  return success;
}

/* Deletes the file called file. Returns true if successful, false otherwise. A
   file may be removed regardless of whether it is open or closed, and removing
   an open file does not close it. See Removing an Open File, for details. */
static bool
syscall_remove (const char *file)
{
  if (!is_valid_ptr (file, 0))
    syscall_exit (-1);
  bool success;
  lock_acquire (&fs_lock);
  success = filesys_remove (file);
  lock_release (&fs_lock);
  return success;
}

/* Opens the file called file. Returns a nonnegative integer handle called a
   "file descriptor" (fd), or -1 if the file could not be opened. */
static int
syscall_open (const char *file)
{
  if (!is_valid_ptr (file, 0))
    syscall_exit (-1);
  struct thread *t = thread_current ();

  struct file *open_file;
  lock_acquire (&fs_lock);
  open_file = filesys_open (file);
  lock_release (&fs_lock);
  if (open_file == NULL)
    return -1;
  struct file_descriptor *fdt_entry;
  if (!(fdt_entry = malloc (sizeof (struct file_descriptor))))
    return -1;
  fdt_entry->file_ = open_file;
  if (!fdt_insert (&t->fd_hash, fdt_entry))
    {
      lock_acquire (&fs_lock);
      file_close (open_file);
      lock_release (&fs_lock);
      free (fdt_entry);
      return -1;
    }
  return fdt_entry->fd;
}

/* Returns the size, in bytes, of the file open as fd. */
static int
syscall_filesize (int fd)
{
  int size;
  struct thread *t = thread_current ();
  struct hash_elem *fd_entry;
  if ((fd_entry = fdt_search (&t->fd_hash, fd)) == NULL)
    return -1;
  lock_acquire (&fs_lock);
  size = file_length (hash_entry (fd_entry, struct file_descriptor, elem)->file_);
  lock_release (&fs_lock);
  return size;
}

/* Reads size bytes from the file open as fd into buffer. Returns the number of
   bytes actually read (0 at end of file), or -1 if the file could not be read
   (due to a condition other than end of file). Fd 0 reads from the keyboard
   using input_getc(). */
static int
syscall_read (int fd, void *buf, uint32_t size)
{
  if (size == 0)
    return 0;
  if (!is_valid_ptr (buf, size - 1))
    syscall_exit (-1);
  int bytes_read = 0;
  if (fd == STDIN_FILENO)
    {
      uint32_t ch;
      char *buffer = buf;
      lock_acquire (&fs_lock);
      for (ch = 0; ch < size; ch++)
        {
          buffer[ch] = input_getc ();
          bytes_read++;
        }
       lock_release (&fs_lock);
       return bytes_read;
    }
  struct thread *t = thread_current ();
  struct hash_elem *found_elem;
  found_elem = fdt_search (&t->fd_hash, fd);
  if (found_elem == NULL)
    return -1;
  struct file *found_fd = 
  hash_entry (found_elem, struct file_descriptor, elem)->file_;
  lock_acquire (&fs_lock);
  bytes_read = file_read (found_fd, buf, size);
  lock_release (&fs_lock);
  return bytes_read;
}

/* Writes size bytes from buffer to the open file fd. Returns the number of
   bytes actually written, which may be less than size if some bytes could not
   be written. */
static int
syscall_write (int fd, const void *buf, uint32_t size)
{
  if (size == 0)
    return 0;
  if (!is_valid_ptr (buf, size - 1))
    syscall_exit (-1);
  int bytes_written = 0; 
  if (fd == STDOUT_FILENO)
    {
      lock_acquire (&fs_lock);
      putbuf (buf, size);
      lock_release (&fs_lock);
      return size;
    }
  struct thread *t = thread_current ();
  struct hash_elem *found_elem;
  found_elem = fdt_search (&t->fd_hash, fd);
  if (found_elem == NULL)
    return 0;
  struct file *found_fd = 
  hash_entry (found_elem, struct file_descriptor, elem)->file_;
  lock_acquire (&fs_lock);
  bytes_written = file_write (found_fd, buf, size);
  lock_release (&fs_lock);
  return bytes_written;
}

/* Changes the next byte to be read or written in open file fd to position,
   expressed in bytes from the beginning of the file. */
static void
syscall_seek (int fd, uint32_t position)
{
  struct thread *t = thread_current ();
  struct hash_elem *fd_entry;
  if ((fd_entry = fdt_search (&t->fd_hash, fd)) == NULL)
    return;
  lock_acquire (&fs_lock);
  file_seek (hash_entry (fd_entry, struct file_descriptor, elem)->file_,
             position);
  lock_release (&fs_lock);
}

/* Returns the position of the next byte to be read or written in open file fd,
   expressed in bytes from the beginning of the file. */
static uint32_t
syscall_tell (int fd) 
{
  struct thread *t = thread_current ();
  struct hash_elem *fd_entry;
  uint32_t pos;
  if ((fd_entry = fdt_search (&t->fd_hash, fd)) == NULL)
    return 0;
  lock_acquire (&fs_lock);
  pos = file_tell (hash_entry (fd_entry, struct file_descriptor, elem)->file_);
  lock_release (&fs_lock);
  return pos;
}

/* Closes file descriptor fd. Exiting or terminating a process implicitly
   closes all its open file descriptors, as if by calling this function for
   each one. */
static void
syscall_close (int fd)
{
  struct thread *t = thread_current ();
  struct hash_elem *fd_entry;
  if ((fd_entry = fdt_search (&t->fd_hash, fd)) == NULL)
    return;
  lock_acquire (&fs_lock);
  fdt_close (fd_entry, t->fd_hash.aux);
  lock_release (&fs_lock);
}

#ifdef VM
/* Map a file into the user vaddr space. Return the mapid of the file. */
static mapid_t syscall_mmap (int fd, void *addr) 
{
  /* check that addr is page aligned */
  if ((uint32_t) addr % PGSIZE != 0)
    return -1;

  size_t len = syscall_filesize (fd);
  if (len == 0)
    return -1;
  //if (!is_valid_mapping (addr, len))
    //return -1;
  //if (!is_valid_ptr (addr, len - 1))
    //syscall_exit (-1);

  struct thread *t = thread_current ();
  struct hash_elem *fd_entry;
  if ((fd_entry = fdt_search (&t->fd_hash, fd)) == NULL)
    return -1;
  struct file *fp = hash_entry (fd_entry, struct file_descriptor,
                                   elem)->file_;
  
  mapid_t mapid = (mapid_t) addr;
  size_t read_bytes = len;
  uint8_t *upage = addr;
  size_t page_no = 0;

  /* all pages allocated will recieve the virtual address 
     passed to mmap and the mapid. */
  while (read_bytes > 0)
    {
      size_t page_read_bytes = read_bytes < PGSIZE ? read_bytes : PGSIZE;   
      size_t page_zero_bytes = PGSIZE - page_read_bytes;
      struct sp_entry *spe = page_supp_alloc (t, upage);
      spe->read_bytes = page_read_bytes;
      spe->zero_bytes = page_zero_bytes;
      //spe->writable = fp->deny_write; 
      spe->fp = fp;
      spe->ofs = page_no * PGSIZE;
      spe->location = MAPPED;
      spe->upage = upage;
      spe->mapid = mapid;
      upage += PGSIZE; 
      read_bytes -= page_read_bytes;
      page_no++;
    }
  return mapid;  
}

/* Unmap a previously mapped file from the user vaddr space. */
static void syscall_munmap (mapid_t mapid)
{
  struct thread *t = thread_current ();
  struct sp_entry *spe;
  while (true) 
    {
      spe = page_find (t->tid, (void *) mapid);
      if (spe == NULL)
        break;
      if (spe->location == MAPPED) 
        {
          list_remove (&spe->l_elem);
          page_supp_delete (spe);
        }
    }
}
#endif

/* Retrieves an argument at a given index from the stack pointer 
   esp argument. */
static void *
syscall_arg (void *esp, int index)
{
  void *ret_arg = (char *) esp + (index * sizeof (void *));
  if (!is_valid_ptr (ret_arg, 0)) 
    syscall_exit (-1); 
  return ret_arg;
}


/* Switch all cases of interrupt signal to call system calls. */
static void
syscall_handler (struct intr_frame *f) 
{
  if (!is_valid_ptr (f->esp, 0)) 
    syscall_exit (-1);
  validate_esp (f->esp);
  int syscall = *(int *) f->esp;
  thread_current ()->saved_esp = f->esp;
  switch (syscall)
    {
      /* Halt the operating system. */                   
      case SYS_HALT:
        syscall_halt ();
        break; 
      /* Terminate this process. */
      case SYS_EXIT:            
        syscall_exit (*(int *) syscall_arg (f->esp, 1));
        break; 
      /* Start another process. */
      case SYS_EXEC:                   
        f->eax = syscall_exec (*(char **) syscall_arg (f->esp, 1));
        break;
       /* Wait for a child process to die. */
      case SYS_WAIT:                  
        f->eax = syscall_wait (*(pid_t *) syscall_arg (f->esp, 1));
        break;
      /* Create a file. */
      case SYS_CREATE:
        f->eax = syscall_create (*(char **) syscall_arg (f->esp, 1),
                                 *(uint32_t *) syscall_arg (f->esp, 2));
        break;                 
      /* Delete a file. */
      case SYS_REMOVE:
        f->eax = syscall_remove (*(char **) syscall_arg (f->esp, 1));
        break; 
      /* Open a file. */
      case SYS_OPEN:
        f->eax = syscall_open (*(char **) syscall_arg (f->esp, 1));
        break; 
      /* Obtain a file's size. */
      case SYS_FILESIZE:
        f->eax = syscall_filesize (*(int *) syscall_arg (f->esp, 1));
        break;
      /* Read from a file. */
      case SYS_READ:
        f->eax = syscall_read (*(int *) syscall_arg (f->esp, 1),
                               *(void **) syscall_arg (f->esp, 2),
                               *(uint32_t *) syscall_arg (f->esp, 3));
        break;
      /* Write to a file. */
      case SYS_WRITE:
         f->eax = syscall_write (*(int *) syscall_arg (f->esp, 1),
                                 *(void **) syscall_arg (f->esp, 2),
                                 *(uint32_t *) syscall_arg (f->esp, 3));
         break;
      /* Change position in a file. */
      case SYS_SEEK:
         syscall_seek (*(int *) syscall_arg (f->esp, 1),
                       *(uint32_t *) syscall_arg (f->esp, 2));
         break; 
      /* Report current position in a file. */
      case SYS_TELL:                   
         f->eax = syscall_tell (*(int *) syscall_arg (f->esp, 1));
         break;
      /* Close a file. */
      case SYS_CLOSE:
         syscall_close (*(int *) syscall_arg (f->esp, 1));
         break;
      #ifdef VM
      /* Map a file to user space. */
      case SYS_MMAP:
         f->eax = syscall_mmap (*(int *) syscall_arg (f->esp, 1), 
                                *(void **) syscall_arg (f->esp, 2));
         break;
      /* Unmap a file from user space.  */
      case SYS_MUNMAP:
         syscall_munmap (*(mapid_t *) syscall_arg (f->esp, 1));
         break;
      #endif
      default:
        printf ("system call!\n");
        thread_exit ();
  }
}

#ifdef VM
/* Validate mapping a file to the vaddr ptr. */
static bool
is_valid_mapping (const void *ptr, size_t len)
{
  if (len == 0)
    return false;
  if (!is_valid_ptr (ptr, len - 1))
    return false;
  struct thread *t = thread_current ();
  size_t curr_pg;
  for (curr_pg = 0; curr_pg < len; curr_pg += PGSIZE) 
    {
      if (pagedir_get_page (t->pagedir, ptr) != NULL)
        return false;
    }
  return true;
}
#endif

/* Validate the address of the pointer passed in. */
static bool
is_valid_ptr (const void *ptr, size_t len)
{
  /* The virtual addr region must both start and end in 
     user address space range. */
  if (!is_user_vaddr (ptr))
    return false;
  if (!is_user_vaddr ((void *)((char *) ptr + len)))
    return false;

  /* The region cannot wrap around across kernel virtual address space. */
  if ((char *) ptr + len < (char *) ptr)
    return false;
  if ((uint32_t) ptr < PGSIZE)
    return false;
#ifdef VM
  void *curr_pg = pg_round_down (ptr);
  struct sp_entry *curr_spe = page_find (thread_current ()->tid, curr_pg);
  if (curr_spe == NULL)
    {
      return false;
    }
#else
  if (pagedir_get_page (thread_current ()->pagedir, ptr) == NULL)
    return false;
#endif
  return true;
}

#ifdef VM
/* Validate the esp in case it extends over the page. */
static void
validate_esp (void *esp)
{
  void *curr_pg = pg_round_down (esp);
  struct sp_entry *curr_spe = page_find (thread_current ()->tid, curr_pg);
  if (curr_spe == NULL)
    {
      if (PHYS_BASE - curr_pg < STACK_LIMIT)
        {
          curr_spe = page_supp_alloc (thread_current (), curr_pg);
          curr_spe->writable = true;
          curr_spe->location = UNMAPPED; 
        }
      else
        syscall_exit (-1);
    }
}
#endif
