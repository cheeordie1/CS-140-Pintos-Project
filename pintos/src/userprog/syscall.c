#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "filesys/filesys.h"
#include "devices/shutdown.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

/* Terminates Pintos by calling power_off() (declared in "threads/init.h").
   This should be seldom used, because you lose some information about 
    possible deadlock situations, etc. */
void 
syscall_halt (void) 
{
  shutdown_power_off ();
}

/* Terminates the current user program, returning status to the kernel. If the
   process's parent waits for it (see below), this is the status that will be
   returned. Conventionally, a status of 0 indicates success and nonzero values
   indicate errors. */
void
syscall_exit (int status)
{
  
  /* NOT YET IMPLEMENTED */
}

/* Runs the executable whose name is given in cmd_line, passing any given
   arguments, and returns the new process's program id (pid). Must return pid
   -1, which otherwise should not be a valid pid, if the program cannot load or
   run for any reason. Thus, the parent process cannot return from the exec
   until it knows whether the child process successfully loaded its executable.
   You must use appropriate synchronization to ensure this. */
pid_t
syscall_exec (const char *cmd_line UNUSED)
{
  /* NOT YET IMPLEMENTED */
  return -1;
}

/* Waits for a child process pid and retrieves the child's exit status.

   If pid is still alive, waits until it terminates. Then, returns the status
   that pid passed to exit. If pid did not call exit(), but was terminated by
   the kernel (e.g. killed due to an exception), wait(pid) must return -1. It
   is perfectly legal for a parent process to wait for child processes that
   have already terminated by the time the parent calls wait, but the kernel
   must still allow the parent to retrieve its child's exit status, or learn
   that the child was terminated by the kernel. */
int
syscall_wait (pid_t pid UNUSED)
{
  /* NOT YET IMPLEMENTED */
  return -1;
}

/* Creates a new file called file initially initial_size bytes in size. Returns
   true if successful, false otherwise. Creating a new file does not open it:
   opening the new file is a separate operation which would require a open
   system call. */
bool
syscall_create (const char *file UNUSED, uint32_t initial_size UNUSED)
{
  return filesys_create (file, (off_t) initial_size);
}

/* Deletes the file called file. Returns true if successful, false otherwise. A
   file may be removed regardless of whether it is open or closed, and removing
   an open file does not close it. See Removing an Open File, for details. */
bool
syscall_remove (const char* file UNUSED)
{
  return filesys_remove (file);
}

/* Opens the file called file. Returns a nonnegative integer handle called a
   "file descriptor" (fd), or -1 if the file could not be opened. */
int
syscall_open (const char* file UNUSED)
{
   /* NOT YET IMPLEMENTED */
  return -1;
}

/* Returns the size, in bytes, of the file open as fd. */
int
syscall_filesize (int fd UNUSED)
{
   /* NOT YET IMPLEMENTED */
  return 0;
}

/* Reads size bytes from the file open as fd into buffer. Returns the number of
   bytes actually read (0 at end of file), or -1 if the file could not be read
   (due to a condition other than end of file). Fd 0 reads from the keyboard
   using input_getc(). */
int
syscall_read (int fd UNUSED, void *buf UNUSED, uint32_t size UNUSED)
{
   /* NOT YET IMPLEMENTED */
  return -1;
}

/* Writes size bytes from buffer to the open file fd. Returns the number of
   bytes actually written, which may be less than size if some bytes could not
   be written. */
int
syscall_write (int fd, const void* buf, uint32_t size)
{
   /* NOT YET IMPLEMENTED */
  if (fd == 1) 
    {
      putbuf (buf, size);
    }
  return 0;
}

/* Changes the next byte to be read or written in open file fd to position,
   expressed in bytes from the beginning of the file. */
void
syscall_seek (int fd UNUSED, uint32_t position UNUSED)
{
   /* NOT YET IMPLEMENTED */
}

/* Returns the position of the next byte to be read or written in open file fd,
   expressed in bytes from the beginning of the file. */
uint32_t
syscall_tell (int fd UNUSED) 
{
   /* NOT YET IMPLEMENTED */
  return 0;
}

/* Closes file descriptor fd. Exiting or terminating a process implicitly
   closes all its open file descriptors, as if by calling this function for
   each one. */
void
syscall_close (int fd UNUSED)
{
   /* NOT YET IMPLEMENTED */
}

void*
syscall_arg (void *esp, int index)
{
  return (char *) esp + (index * sizeof (void *));
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  int syscall = *(int*) f->esp;
<<<<<<< HEAD
  switch(syscall){
    
    /* Halt the operating system. */                   
    case SYS_HALT:
      syscall_halt ();
    
    /* Terminate this process. */
    case SYS_EXIT:                   
      {
        int status = *(int*) syscall_arg (f->esp, 1);
        syscall_exit (status);
      }
      break;
    /* Start another process. */
    case SYS_EXEC:                   
      {
        char* cmd_line = (char*) syscall_arg (f->esp, 1);
        pid_t pid = syscall_exec (cmd_line);
        f->eax = (uint32_t) pid;
      }
      break;
    /* Wait for a child process to die. */
    case SYS_WAIT:                   
      {
        pid_t pid = *(pid_t*) syscall_arg (f->esp, 1);
        int status = syscall_wait (pid);
        f->eax = (uint32_t) status;
      }
      break;
    /* Create a file. */
    case SYS_CREATE:
      {
        char* file = (char*) syscall_arg (f->esp, 1);
        uint32_t initial_size = *(uint32_t*) syscall_arg (f->esp, 2);
        bool success = syscall_create (file, initial_size);
        f->eax = (uint32_t) success;
      }                 
      break;
    /* Delete a file. */
    case SYS_REMOVE:
      {
        char* file = (char*) syscall_arg (f->esp, 1);
        bool success = syscall_remove (file);
        f->eax = (uint32_t) success;
      }
      break;
    /* Open a file. */
    case SYS_OPEN:
      {
        char* file = (char*) syscall_arg (f->esp, 1);
        int fd = syscall_open (file);
        f->eax = (uint32_t) fd;
      }
      break;
    /* Obtain a file's size. */
    case SYS_FILESIZE:
      {
        int fd = *(int*) syscall_arg (f->esp, 1);
        int size = syscall_filesize (fd);
        f->eax = (uint32_t) size;
      }
      break;
    /* Read from a file. */
    case SYS_READ:
      {
        int fd = *(int*) syscall_arg (f->esp, 1);
        void *buf = syscall_arg (f->esp, 2);
        uint32_t size = *(uint32_t*) syscall_arg (f->esp, 3);
        int bytes_read = syscall_read (fd, buf, size);
        f->eax = (uint32_t) bytes_read;
      }
      break;
    /* Write to a file. */
    case SYS_WRITE:
       {
         int fd = *(int*) syscall_arg (f->esp, 1);
         void *buf = syscall_arg (f->esp, 2);
         uint32_t size = *(uint32_t*) syscall_arg (f->esp, 3);
         int bytes_written = syscall_write (fd, buf, size);
         f->eax = (uint32_t) bytes_written;
       }  
      break;
    /* Change position in a file. */
    case SYS_SEEK:
      {
         int fd = *(int*) syscall_arg (f->esp, 1);
         uint32_t position = *(uint32_t*) syscall_arg (f->esp, 2);
         syscall_seek (fd, position);
      } 
      break;
    /* Report current position in a file. */
    case SYS_TELL:                   
      {
         int fd = *(int*) syscall_arg (f->esp, 1);
         uint32_t position = syscall_tell (fd);
         f->eax = position;
      } 
      break;
    /* Close a file. */
    case SYS_CLOSE:
      {
         int fd = *(int*) syscall_arg (f->esp, 1);
         syscall_close (fd);
      }
      break;
    default:
      printf ("system call!\n");
      thread_exit ();
=======
  switch(syscall)
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
         printf ("<1>\n");
         f->eax = syscall_write (*(int *) syscall_arg (f->esp, 1),
                                 *(void **) syscall_arg (f->esp, 2),
                                 *(uint32_t *) syscall_arg (f->esp, 3));
         printf ("<2>\n");
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
>>>>>>> aa156fbe85f445274326a31ebdac4985d8cb378c
  }
}
