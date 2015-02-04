#ifndef PERROR_H
#define PERROR_H

#include <syscall-nr.h>
#include <stdio.h>
#include <stdarg.h>
#include "userprog/syscall.h"

/* Exit_unless takes a bool condition, a const char *format,
   and extra formatting and calls vfprintf to print the
   contents of format as an error message to stream if 
   bool is false. This function exits after printing the
   error to stdout. */

void
error_unless (bool condition, int exit_status, const char *format, ...)
{
  if (condition)
    return;
  va_list arg_list;
  va_start (arg_list, format);
  vprintf (format, arg_list);
  va_end (arg_list);
  exit (SYS_EXIT);
}

#endif /* userprog/perror.h */
