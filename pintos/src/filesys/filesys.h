#ifndef FILESYS_FILESYS_H
#define FILESYS_FILESYS_H

#include <stdbool.h>
#include "filesys/off_t.h"

/* Sectors of system file inodes. */
#define FREE_MAP_INODE 0       /* Free map file inode sector. */
#define ROOT_DIR_INODE 1       /* Root directory file inode sector. */
#define INODE_MAP_INODE 2      /* Inode free map inode sector. */
#define RESERVED_INODES 3      /* Number of reserved inodes. */

/* Block device that contains the file system. */
struct block *fs_device;
struct free_map inode_map;

void filesys_init (bool format);
void filesys_done (void);
bool filesys_create (const char *name, off_t initial_size);
struct file *filesys_open (const char *name);
bool filesys_remove (const char *name);

#endif /* filesys/filesys.h */
