#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/malloc.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "devices/input.h"
#include "devices/shutdown.h"
#include "lib/kernel/hash.h"

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
static void* syscall_arg (void *esp, int index);

static struct lock fs_lock;

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
  // TODO check address of status
  struct thread *t = thread_current ();
  printf ("%s: exit(%d)\n", t->exec_name, status);
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
  // TODO check address of cmd_line
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
  // TODO check address of pid
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
  // TODO check if file * is proper address and initial_size is proper address
  bool success;
  while (!lock_try_acquire (&fs_lock))
    barrier ();
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
  // TODO check if file * is proper address
  bool success;
  while (!lock_try_acquire (&fs_lock))
    barrier ();
  success = filesys_remove (file);
  lock_release (&fs_lock);
  return success;
}

/* Opens the file called file. Returns a nonnegative integer handle called a
   "file descriptor" (fd), or -1 if the file could not be opened. */
static int
syscall_open (const char *file)
{
  // TODO Check the file * for proper address
  struct thread *t = thread_current ();
  struct file_descriptor *fdt_entry;
  if (!(fdt_entry = malloc (sizeof (struct file_descriptor))))
    return -1;
  if (!fdt_insert (&t->fd_hash, fdt_entry))
    return -1;
  struct file *open_file;
  while (!lock_try_acquire (&fs_lock))
    barrier ();
  open_file = filesys_open (file);
  lock_release (&fs_lock);
  if (open_file == NULL)
    return -1;
  fdt_entry->file_ = open_file;
  return fdt_entry->fd;
}

/* Returns the size, in bytes, of the file open as fd. */
static int
syscall_filesize (int fd)
{
  // TODO check the address for proper addressing
  int size;
  struct thread *t = thread_current ();
  struct hash_elem *fd_entry;
  if ((fd_entry = fdt_search (&t->fd_hash, fd)) == NULL)
    return -1;
  while (!lock_try_acquire (&fs_lock))
    barrier ();
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
  // TODO check the addresses of fd, buf, size
  uint8_t buffer[size];
  int bytes_read = 0;
  if (fd == STDIN_FILENO)
    {
      uint32_t ch;
      while (!lock_try_acquire (&fs_lock))
        barrier ();
      for (ch = 0; ch < size; ch++)
        {
          buffer[ch] = input_getc ();
          bytes_read++;
        }
       lock_release (&fs_lock);
    }
  struct thread *t = thread_current ();
  struct hash_elem *found_elem;
  found_elem = fdt_search (&t->fd_hash, fd);
  if (found_elem == NULL)
    return -1;
  struct file *found_fd = 
  hash_entry (found_elem, struct file_descriptor, elem)->file_;
  while (!lock_try_acquire (&fs_lock))
    barrier ();
  bytes_read = file_read (found_fd, buf, 0);
  lock_release (&fs_lock);
  return bytes_read;
}

/* Writes size bytes from buffer to the open file fd. Returns the number of
   bytes actually written, which may be less than size if some bytes could not
   be written. */
static int
syscall_write (int fd, const void* buf, uint32_t size)
{
  // TODO check addresses of fd, buf, size
  int bytes_written = 0; 
  if (fd == STDOUT_FILENO)
    {
      while (!lock_try_acquire (&fs_lock))
        barrier ();   
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
  while (!lock_try_acquire (&fs_lock))
    barrier ();
  bytes_written = file_write (found_fd, buf, 0);
  lock_release (&fs_lock);
  return bytes_written;
}

/* Changes the next byte to be read or written in open file fd to position,
   expressed in bytes from the beginning of the file. */
static void
syscall_seek (int fd, uint32_t position)
{
  // TODO check addresses of fd, position struct
  struct thread *t = thread_current ();
  struct hash_elem *fd_entry;
  if ((fd_entry = fdt_search (&t->fd_hash, fd)) == NULL)
    return;
  while (!lock_try_acquire (&fs_lock))
    barrier ();
  file_seek (hash_entry (fd_entry, struct file_descriptor, elem)->file_,
             position);
  lock_release (&fs_lock);
}

/* Returns the position of the next byte to be read or written in open file fd,
   expressed in bytes from the beginning of the file. */
static uint32_t
syscall_tell (int fd) 
{
  // TODO check address of fd
  struct thread *t = thread_current ();
  struct hash_elem *fd_entry;
  if ((fd_entry = fdt_search (&t->fd_hash, fd)) == NULL)
    return 0;
  while (!lock_try_acquire (&fs_lock))
    barrier ();
  return file_tell (hash_entry (fd_entry, struct file_descriptor, elem)->file_);
  lock_release (&fs_lock);
}

/* Closes file descriptor fd. Exiting or terminating a process implicitly
   closes all its open file descriptors, as if by calling this function for
   each one. */
static void
syscall_close (int fd)
{
  // TODO check address of fd
  struct thread *t = thread_current ();
  struct hash_elem *fd_entry;
  if ((fd_entry = fdt_search (&t->fd_hash, fd)) == NULL)
    return;
  hash_delete (&t->fd_hash, fd_entry);
  struct file_descriptor *del_fd = 
  hash_entry (fd_entry, struct file_descriptor, elem);
  while (!lock_try_acquire (&fs_lock))
    barrier ();
  file_close (del_fd->file_);
  lock_release (&fs_lock);
  free (del_fd);
}

/* Retrieves an argument at a given index from the stack pointer 
   esp argument. */
static void *
syscall_arg (void *esp, int index)
{
  return (char *) esp + (index * sizeof (void *));
}

/* Switch all cases of interrupt signal to call system calls. */
static void
syscall_handler (struct intr_frame *f) 
{
  int syscall = *(int*) f->esp;
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
      default:
        printf ("system call!\n");
        thread_exit ();
  }
}
