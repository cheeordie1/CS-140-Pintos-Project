#include "filesys/inode.h"
#include <list.h>
#include <debug.h>
#include <round.h>
#include <string.h>
#include "threads/malloc.h"
#include "filesys/filesys.h"
#include "filesys/free-map.h"
#include "filesys/cache.h"

/* Identifies an inode. */
#define INODE_MAGIC 0x494e4f44

#define DIRECT_SECTORS 6
#define DOUBIND_SECTOR 5
#define BLOCKNUMS_PER_IND 256
#define INODES_PER_SECTOR 16
#define INODE_SIZE 32
#define PERCENT_INODES 1 / 100

/* On-disk inode.
   Must be INODE_SIZE bytes long. */
struct disk_inode
  {
    off_t length;                        /* File size in bytes. */
    block_sector_t i_sectors[6];         /* Sector numbers of disk locations. */
    bool dir;                            /* True if directory, false otherwise. */
    bool large;                          /* True if large block addressing.  */
    uint16_t magic;                      /* Magic number. */
  };

static inline size_t
byte_ofs_inode_sector (size_t inumber)
{
  return (inumber % INODES_PER_SECTOR) * INODE_SIZE;
}

/* In-memory inode. */
struct inode
  {
    struct list_elem elem;               /* Element in inode list. */
    block_sector_t inumber;              /* Inumber that represents inode. */
    block_sector_t sector;               /* Sector that contains inode. */
    int open_cnt;                        /* Number of openers. */
    bool removed;                        /* True if deleted, false otherwise. */
    int deny_write_cnt;                  /* 0: writes ok, >0: deny writes. */
  };

static size_t file_block_start;
static size_t num_inodes;

static block_sector_t small_lookup (struct inode *inode, int block_idx);
static block_sector_t large_lookup (struct inode *inode, int block_idx);
static block_sector_t lookup_in_sector (block_sector_t file_data_sector,
                                        int sector_ofs);
static void inode_close_tree (struct inode *inode);

/* Returns the filesystem sector that contains the inode for the 
   given inumber. */
block_sector_t
inode_sector_from_inumber (block_sector_t inumber)
{
  if (num_inodes <= inumber)
    return INODE_ERROR;
  return inumber / INODES_PER_SECTOR;
}

/* Searches the inode for the index of the fileblock referred to by
   the inode's blockNum'th file block number in its file block number
   array. The function runs a direct index lookup on small files that 
   use 7 or less file blocks and indirect index lookups on large files
   that have more than 7 file blocks. Returns INODE_ERROR on error. */
block_sector_t 
inode_lookup (struct inode *inode, int block_idx) 
{
  struct cache_block *cached_inode_sector;
  struct disk_inode *inode_disk;
  bool large;
  block_sector_t sector_ofs = byte_ofs_inode_sector (inode->inumber);
  lock_acquire (&GENGAR);
  if ((cached_inode_sector = cache_find_sector (inode->sector)) == NULL)
    cached_inode_sector = cache_fetch (inode->sector, INODE_METADATA);
  inode_disk = ((struct disk_inode *) cached_inode_sector->data) + sector_ofs;
  large = inode_disk->large;
  lock_release (&GENGAR);
  if (inode_disk->large) 
    return large_lookup (inode, block_idx);
  else 
    return small_lookup (inode, block_idx); 
}

/* Runs inode fileblock lookup algorithm for small files. */
static block_sector_t 
small_lookup (struct inode *inode, int block_idx)
{
  ASSERT (block_idx >= 0 && block_idx < DIRECT_SECTORS);
  struct cache_block *cached_inode_sector;
  struct disk_inode *inode_disk;
  block_sector_t file_data_sector;
  block_sector_t sector_ofs = byte_ofs_inode_sector (inode->inumber);
  lock_acquire (&GENGAR);
  if ((cached_inode_sector = cache_find_sector (inode->sector)) == NULL)
    cached_inode_sector = cache_fetch (inode->sector, INODE_METADATA);
  inode_disk = ((struct disk_inode *) cached_inode_sector->data) + sector_ofs;
  file_data_sector = inode_disk->i_sectors[block_idx];
  lock_release (&GENGAR);
  if (file_data_sector < 0) 
    return INODE_ERROR;
  return file_data_sector; 
}

/* Runs inode fileblock lookup algorithm for large files. */
static block_sector_t
large_lookup (struct inode *inode, int block_idx)
{
  int indirect_idx = block_idx / BLOCKNUMS_PER_IND;
  int indirect_ofs;
  block_sector_t file_data_sector;
  block_sector_t lookup_sector; 
  struct cache_block *cached_inode_sector;
  struct disk_inode *inode_disk;
  block_sector_t sector_ofs = byte_ofs_inode_sector (inode->inumber);
  lock_acquire (&GENGAR);
  if ((cached_inode_sector = cache_find_sector (inode->sector)) == NULL)
    cached_inode_sector = cache_fetch (inode->sector, INODE_METADATA);
  inode_disk = ((struct disk_inode *) cached_inode_sector->data) + sector_ofs;
  if (indirect_idx < DOUBIND_SECTOR)
    lookup_sector = inode_disk->i_sectors[indirect_idx];
  else
    lookup_sector = inode_disk->i_sectors[DOUBIND_SECTOR];
  lock_release (&GENGAR);
  if (indirect_idx < DOUBIND_SECTOR)
    {
      indirect_ofs = block_idx % BLOCKNUMS_PER_IND;
      file_data_sector = lookup_in_sector (lookup_sector, indirect_ofs);
    }
  else
    {
      indirect_ofs = (indirect_idx - DOUBIND_SECTOR) / BLOCKNUMS_PER_IND;
      int dindirect_ofs = (indirect_idx - DOUBIND_SECTOR) % BLOCKNUMS_PER_IND;
      block_sector_t dindirect_idx = lookup_in_sector (lookup_sector, 
				                       indirect_ofs);
      file_data_sector = lookup_in_sector (dindirect_idx, dindirect_ofs);
    }
  return file_data_sector;
}

/* Searches for a sector in an indirect sector. */
static block_sector_t
lookup_in_sector (block_sector_t file_data_sector, int sector_ofs)
{
  struct cache_block *cached_sector;
  block_sector_t ret_sector;
  lock_acquire (&GENGAR);
  if ((cached_sector = cache_find_sector (file_data_sector)) == NULL)
    cached_sector = cache_fetch (file_data_sector, INODE_METADATA);
  ret_sector = ((uint16_t *) cached_sector->data)[sector_ofs];
  lock_release (&GENGAR);
  return ret_sector;
}

/* List of open inodes, so that opening a single inode twice
   returns the same `struct inode'. */
static struct list open_inodes;

static struct lock inode_lock;

/* Initializes the inode module. */
void
inode_init (void) 
{
  list_init (&open_inodes);
  lock_init (&inode_lock);
  file_block_start = block_size (fs_device) * PERCENT_INODES;
  num_inodes = file_block_start * INODES_PER_SECTOR;
}

/* Initializes an inode with LENGTH bytes of data and
   writes the new inode to sector SECTOR on the file system
   device.
   Returns true if successful.
   Returns false if memory or disk allocation fails. */
bool
inode_create (block_sector_t inumber, bool is_dir)
{
  struct disk_inode inode_disk;
  bool success = false;
  
  /* If this assertion fails, inode algorithms will fail. */
  ASSERT (sizeof (inode_disk) == BLOCK_SECTOR_SIZE);
   
  inode_disk.dir = is_dir;
  inode_disk.large = false;
  inode_disk.magic = INODE_MAGIC;
  block_sector_t sector = inode_sector_from_inumber (inumber);
  if (sector == INODE_ERROR)
    success = false;  
  else
    {
      struct cache_block *cached_inode_sector; 
      lock_acquire (&GENGAR);
      if ((cached_inode_sector = cache_find_sector (sector)) == NULL)
        cached_inode_sector = cache_fetch (sector, INODE_DATA);
      cache_write (cached_inode_sector, &inode_disk,
                   byte_ofs_inode_sector (inumber), sizeof (inode_disk));
      lock_release (&GENGAR);
      success = true;
    }
  return success;
}

/* Reads an inode referred to by its inumber
   and returns a `struct inode' that contains it.
   Returns a null pointer if memory allocation fails. */
struct inode *
inode_open (block_sector_t inumber)
{
  struct list_elem *e;
  struct inode *inode;

  /* Check whether this inode is already open. */
  lock_acquire (&inode_lock);
  for (e = list_begin (&open_inodes); e != list_end (&open_inodes);
       e = list_next (e)) 
    {
      lock_release (&inode_lock);
      inode = list_entry (e, struct inode, elem);
      if (inode->inumber == inumber) 
        {
          inode_reopen (inode);
          return inode; 
        }
    }

  /* Allocate memory. */
  inode = malloc (sizeof *inode);
  if (inode == NULL)
    return NULL;

  /* Initialize. */
  list_push_front (&open_inodes, &inode->elem);
  inode->sector = inode_sector_from_inumber (inumber);
  inode->inumber = inumber;
  inode->open_cnt = 1;
  inode->deny_write_cnt = 0;
  inode->removed = false;
  lock_release (&inode_lock);
  return inode;
}

/* Reopens and returns INODE. */
struct inode *
inode_reopen (struct inode *inode)
{
  lock_acquire (&inode_lock);
  if (inode != NULL)
    inode->open_cnt++;
  lock_release (&inode_lock);
  return inode;
}

/* Closes INODE and writes it to disk.
   If this was the last reference to INODE, frees its memory.
   If INODE was also a removed inode, frees its blocks. */
void
inode_close (struct inode *inode) 
{
  /* Ignore null pointer. */
  if (inode == NULL)
    return;

  /* Release resources if this was the last opener. */
  lock_acquire (&inode_lock);
  if (--inode->open_cnt == 0)
    {
      /* Remove from inode list and release lock. */
      list_remove (&inode->elem);
      lock_release (&inode_lock);
      /* Deallocate blocks if removed. */
      if (inode->removed) 
        {
	  inode_close_tree (inode);
          free_map_release (&inode_map, inode->inumber, 1);
        }
      free (inode); 
    }
}

/* Releases all sectors obtained by an inode. */
static void
inode_close_tree (struct inode *inode)
{
  struct cache_block *cached_inode_sector;
  struct disk_inode *inode_disk;
  size_t num_blocks_to_delete;
  block_sector_t sector_ofs = byte_ofs_inode_sector (inode->inumber);
  lock_acquire (&GENGAR);
  if ((cached_inode_sector = cache_find_sector (inode->sector)) == NULL)
    cached_inode_sector = cache_fetch (inode->sector, INODE_METADATA);
  inode_disk = ((struct disk_inode *) cached_inode_sector->data) + sector_ofs;
  num_blocks_to_delete = inode_disk->length / 10; // TODO: replace filler value. 
  lock_release (&GENGAR);
  if (inode_disk->large)
    {
      //return large_lookup (inode, block_idx);
    }
  else
    {
      
    }
}

/* Marks INODE to be deleted when it is closed by the last caller who
   has it open. */
void
inode_remove (struct inode *inode) 
{
  ASSERT (inode != NULL);
  inode->removed = true;
}

/* Reads SIZE bytes from INODE into BUFFER, starting at position OFFSET.
   Returns the number of bytes actually read, which may be less
   than SIZE if an error occurs or end of file is reached. */
off_t
inode_read_at (struct inode *inode, void *buffer_, off_t size, off_t offset) 
{
  uint8_t *buffer = buffer_;
  off_t bytes_read = 0;
  uint8_t *bounce = NULL;

  while (size > 0) 
    {
      /* Disk sector to read, starting byte offset within sector. */
      block_sector_t sector_idx = byte_to_sector (inode, offset);
      int sector_ofs = offset % BLOCK_SECTOR_SIZE;

      /* Bytes left in inode, bytes left in sector, lesser of the two. */
      off_t inode_left = inode_length (inode) - offset;
      int sector_left = BLOCK_SECTOR_SIZE - sector_ofs;
      int min_left = inode_left < sector_left ? inode_left : sector_left;

      /* Number of bytes to actually copy out of this sector. */
      int chunk_size = size < min_left ? size : min_left;
      if (chunk_size <= 0)
        break;

      if (sector_ofs == 0 && chunk_size == BLOCK_SECTOR_SIZE)
        {
          /* Read full sector directly into caller's buffer. */
          block_read (fs_device, sector_idx, buffer + bytes_read);
        }
      else 
        {
          /* Read sector into bounce buffer, then partially copy
             into caller's buffer. */
          if (bounce == NULL) 
            {
              bounce = malloc (BLOCK_SECTOR_SIZE);
              if (bounce == NULL)
                break;
            }
          block_read (fs_device, sector_idx, bounce);
          memcpy (buffer + bytes_read, bounce + sector_ofs, chunk_size);
        }
      
      /* Advance. */
      size -= chunk_size;
      offset += chunk_size;
      bytes_read += chunk_size;
    }
  free (bounce);

  return bytes_read;
}

/* Writes SIZE bytes from BUFFER into INODE, starting at OFFSET.
   Returns the number of bytes actually written, which may be
   less than SIZE if end of file is reached or an error occurs.
   (Normally a write at end of file would extend the inode, but
   growth is not yet implemented.) */
off_t
inode_write_at (struct inode *inode, const void *buffer_, off_t size,
                off_t offset) 
{
  const uint8_t *buffer = buffer_;
  off_t bytes_written = 0;
  uint8_t *bounce = NULL;

  if (inode->deny_write_cnt)
    return 0;

  while (size > 0) 
    {
      /* Sector to write, starting byte offset within sector. */
      block_sector_t sector_idx = byte_to_sector (inode, offset);
      int sector_ofs = offset % BLOCK_SECTOR_SIZE;

      /* Bytes left in inode, bytes left in sector, lesser of the two. */
      off_t inode_left = inode_length (inode) - offset;
      int sector_left = BLOCK_SECTOR_SIZE - sector_ofs;
      int min_left = inode_left < sector_left ? inode_left : sector_left;

      /* Number of bytes to actually write into this sector. */
      int chunk_size = size < min_left ? size : min_left;
      if (chunk_size <= 0)
        break;

      if (sector_ofs == 0 && chunk_size == BLOCK_SECTOR_SIZE)
        {
          /* Write full sector directly to disk. */
          block_write (fs_device, sector_idx, buffer + bytes_written);
        }
      else 
        {
          /* We need a bounce buffer. */
          if (bounce == NULL) 
            {
              bounce = malloc (BLOCK_SECTOR_SIZE);
              if (bounce == NULL)
                break;
            }

          /* If the sector contains data before or after the chunk
             we're writing, then we need to read in the sector
             first.  Otherwise we start with a sector of all zeros. */
          if (sector_ofs > 0 || chunk_size < sector_left) 
            block_read (fs_device, sector_idx, bounce);
          else
            memset (bounce, 0, BLOCK_SECTOR_SIZE);
          memcpy (bounce + sector_ofs, buffer + bytes_written, chunk_size);
          block_write (fs_device, sector_idx, bounce);
        }

      /* Advance. */
      size -= chunk_size;
      offset += chunk_size;
      bytes_written += chunk_size;
    }
  free (bounce);

  return bytes_written;
}

/* Disables writes to INODE.
   May be called at most once per inode opener. */
void
inode_deny_write (struct inode *inode) 
{
  inode->deny_write_cnt++;
  ASSERT (inode->deny_write_cnt <= inode->open_cnt);
}

/* Re-enables writes to INODE.
   Must be called once by each inode opener who has called
   inode_deny_write() on the inode, before closing the inode. */
void
inode_allow_write (struct inode *inode) 
{
  ASSERT (inode->deny_write_cnt > 0);
  ASSERT (inode->deny_write_cnt <= inode->open_cnt);
  inode->deny_write_cnt--;
}

/* Returns the length, in bytes, of INODE's data. */
off_t
inode_length (const struct inode *inode)
{
  struct cache_block *cached_inode_sector;
  struct disk_inode *inode_disk;
  size_t num_blocks_to_delete;
  block_sector_t sector_ofs = byte_ofs_inode_sector (inode->inumber);
  lock_acquire (&GENGAR);
  if ((cached_inode_sector = cache_find_sector (inode->sector)) == NULL)
    cached_inode_sector = cache_fetch (inode->sector, INODE_METADATA);
  inode_disk = ((struct disk_inode *) cached_inode_sector->data) + sector_ofs;
  return inode_disk->length;
}
