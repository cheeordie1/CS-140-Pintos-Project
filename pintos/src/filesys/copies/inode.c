#include "filesys/inode.h"
#include <list.h>
#include <debug.h>
#include <round.h>
#include <string.h>
#include "filesys/cache.h"
#include "filesys/filesys.h"
#include "filesys/free-map.h"
#include "threads/malloc.h"

/* Identifies an inode. */
#define INODE_MAGIC 0x494e4f44

#define DIRECT_SECTORS 6
#define DOUBIND_SECTOR 5
#define BLOCKNUMS_PER_IND 128
#define INODES_PER_SECTOR 16
#define INODE_SIZE 32
#define PERCENT_INODES 1 / 100

/* On-disk inode.
   Must be INODE_SIZE bytes long. */
struct disk_inode
  {
    block_sector_t i_sectors[6];         /* Disk sector locations. */
    off_t length;                        /* File size in bytes. */
    bool dir;                            /* True if directory. */
    bool large;                          /* True if large block addressing.  */
    uint16_t magic;                      /* Magic number. */
  };

/* Returns the number of sectors to allocate for an inode SIZE
bytes long. */
static inline size_t
bytes_to_sectors (off_t size)
{
  return DIV_ROUND_UP (size, BLOCK_SECTOR_SIZE);
}

/* Number of bytes into an inode sector that an inode is. */ 
static inline size_t
byte_ofs_inode_sector (size_t inumber)
{
  return (inumber % INODES_PER_SECTOR) * INODE_SIZE;
}

/* In-memory inode. */
struct inode
  {
    struct list_elem elem;               /* Element in inode list. */
    struct shared_lock sl;               /* Read / write shared lock. */
    size_t sectors;                      /* Number of file data sectors. */
    size_t dir_sectors;                  /* Direct sectors in memory. */
    size_t ind_sectors;                  /* Indirect sectors in memory. */
    size_t dind_sectors;                 /* Doubly indirect sectors. */
    off_t length;                        /* On disk length of the file. */
    bool dir;                            /* True if directory. */
    bool large;                          /* True if large block addressing. */
    block_sector_t inumber;              /* Inumber that represents inode. */
    block_sector_t sector;               /* Sector that contains inode. */
    int open_cnt;                        /* Number of openers. */
    bool removed;                        /* True if deleted. */
    int deny_write_cnt;                  /* 0: writes ok, >0: deny writes. */
  };

static size_t num_inodes;
static block_sector_t max_sectors;

static block_sector_t direct_lookup (struct inode *inode, size_t block_idx);
static block_sector_t indirect_lookup (size_t indirect_sector,
                                       size_t block_idx);
static block_sector_t dindirect_lookup (size_t dindirect_sector, 
                                        size_t block_idx);
static block_sector_t lookup_in_sector (block_sector_t file_data_sector,
                                        size_t sector_ofs);
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
inode_lookup (struct inode *inode, size_t block_idx) 
{
  block_sector_t indirect_sector;
  if (inode->large)
    {
      size_t direct_idx = block_idx / BLOCKNUMS_PER_IND;
      if (direct_idx < DOUBIND_SECTOR)
	{
          indirect_sector = direct_lookup (inode, direct_idx);
	  return indirect_lookup (indirect_sector, block_idx);
	}
      else
	{
          indirect_sector = direct_lookup (inode, DOUBIND_SECTOR);
	  block_sector_t dindirect_sector = indirect_lookup (indirect_sector,
	                                                     block_idx);
	  return dindirect_lookup (dindirect_sector, block_idx);
	}
    }
  else 
    return direct_lookup (inode, block_idx); 
}

/* Runs inode fileblock lookup algorithm for small files. */
static block_sector_t 
direct_lookup (struct inode *inode, size_t block_idx)
{
  ASSERT (block_idx < DIRECT_SECTORS);
  struct cache_block *cached_inode_sector;
  struct disk_inode *inode_disk;
  block_sector_t file_data_sector;
  block_sector_t inode_ofs = inode->inumber % INODES_PER_SECTOR;
  lock_acquire (&GENGAR);
  if ((cached_inode_sector = cache_find_sector (inode->sector)) == NULL)
    cached_inode_sector = cache_fetch (inode->sector, INODE_DATA);
  inode_disk = ((struct disk_inode *) cached_inode_sector->data) + inode_ofs;
  file_data_sector = inode_disk->i_sectors[block_idx];
  lock_release (&GENGAR);
  return file_data_sector; 
}

/* Passed in a block that is indirect, returns either a doubly
   indirect block sector or a file data sector. */
static block_sector_t
indirect_lookup (size_t indirect_sector, size_t block_idx)
{
  size_t indirect_idx = block_idx / BLOCKNUMS_PER_IND;
  size_t indirect_ofs;
  if (indirect_idx < DOUBIND_SECTOR)
    {
      indirect_ofs = block_idx % BLOCKNUMS_PER_IND;
      return lookup_in_sector (indirect_sector, indirect_ofs);
    }
  else
    {
      indirect_ofs = indirect_idx - DOUBIND_SECTOR;
      return lookup_in_sector (indirect_sector, indirect_ofs);
    }
}

/* Passed in a block that is doubly indirect and returns the
   file data sector that is requested. */
static block_sector_t
dindirect_lookup (size_t dindirect_sector, size_t block_idx)
{
  size_t dindirect_ofs = block_idx % BLOCKNUMS_PER_IND;
  return lookup_in_sector (dindirect_sector, dindirect_ofs);
}

/* Searches for a sector in an indirect sector. */
static block_sector_t
lookup_in_sector (block_sector_t file_data_sector, size_t sector_ofs)
{
  struct cache_block *cached_sector;
  block_sector_t ret_sector;
  lock_acquire (&GENGAR);
  if ((cached_sector = cache_find_sector (file_data_sector)) == NULL)
    cached_sector = cache_fetch (file_data_sector, INODE_METADATA);
  ret_sector = ((block_sector_t *) cached_sector->data)[sector_ofs];
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
  max_sectors = ((DIRECT_SECTORS - 1) * BLOCKNUMS_PER_IND) + 
                 (BLOCKNUMS_PER_IND * BLOCKNUMS_PER_IND);
}

/* Append a reserved fileblock to the end of an inode. */
static block_sector_t
inode_append_sector (struct inode *inode)
{
  block_sector_t new_sector = INODE_ERROR;
  if(!free_map_allocate (&fs_map, file_block_start, 1, &new_sector))
    return INODE_ERROR;
  size_t ofs = byte_ofs_inode_sector (inode->inumber);
  struct cache_block *cached_inode_sector; 
  struct disk_inode *inode_disk;

  /* Setup file layout for new sector. */
  if (inode->sectors <= DIRECT_SECTORS)
    {
      if (inode->sectors < DIRECT_SECTORS)
        {
          lock_acquire (&GENGAR);
          cached_inode_sector = cache_find_sector (inode->sector);
          if (cached_inode_sector == NULL)
            cached_inode_sector = cache_fetch (inode->sector, INODE_DATA);
          inode_disk = (struct disk_inode *) &cached_inode_sector->data[ofs];
          inode_disk->i_sectors[inode->sectors] = new_sector; 
          cached_inode_sector->dirty = true;
          lock_release (&GENGAR);
	  inode->dir_sectors++;
        }
      else
        {
          block_sector_t new_ind;
          if(!free_map_allocate (&fs_map, file_block_start, 1, &new_ind))
            {
              free_map_release (&fs_map, new_sector, 1);
              return INODE_ERROR;
            }
	      struct cache_block *cached_new_ind;
          lock_acquire (&GENGAR);
          cached_inode_sector = cache_find_sector (inode->sector);
	      if (cached_inode_sector == NULL)
            cached_inode_sector = cache_fetch (inode->sector, INODE_DATA);
          inode_disk = (struct disk_inode *) &cached_inode_sector->data[ofs]; 
   	      cached_new_ind = cache_find_sector (new_ind);
	      if (cached_new_ind == NULL)
            cached_new_ind = cache_fetch (new_ind, INODE_METADATA);
	      cache_write (cached_new_ind, inode_disk->i_sectors, 0,
                           DIRECT_SECTORS * sizeof (block_sector_t));
	      cache_write (cached_new_ind, &new_sector, DIRECT_SECTORS *
	                   sizeof (block_sector_t), sizeof (block_sector_t));
	      inode_disk->i_sectors[0] = new_ind; 
          inode_disk->large = true;
	      cached_inode_sector->dirty = true;
          lock_release (&GENGAR);
          inode->large = true;
          inode->dir_sectors = 0;
          inode->ind_sectors++;           
        }
    }
  else
    {
      if (inode->sectors % BLOCKNUMS_PER_IND == 0)
        {
          block_sector_t new_ind;
          block_sector_t new_dind;
          size_t dind_ofs;
          struct cache_block *cached_new_ind;
          struct cache_block *cached_new_dind;
          if(!free_map_allocate (&fs_map, file_block_start, 1, &new_ind))
            {
              free_map_release (&fs_map, new_sector, 1);
              return INODE_ERROR;
            }
          if (inode->dir_sectors <= DIRECT_SECTORS - 1)
            {
              lock_acquire (&GENGAR);
              cached_inode_sector = cache_find_sector (inode->sector); 
              if (cached_inode_sector == NULL)
                cached_inode_sector = cache_fetch (inode->sector, INODE_DATA);
              inode_disk = (struct disk_inode *) 
                            &cached_inode_sector->data[ofs];
              inode_disk->i_sectors[inode->dir_sectors] = new_ind;
              cached_inode_sector->dirty = true;
              lock_acquire (&GENGAR);
              inode->dir_sectors++;
            }
          else if (inode->dir_sectors >= DIRECT_SECTORS - 1)
            {
              if (inode->dir_sectors == DIRECT_SECTORS - 1)
                {
                  new_dind = new_ind;
                  if(!free_map_allocate (&fs_map, file_block_start, 1,
                                         &new_ind))
                    {
                      free_map_release (&fs_map, new_sector, 1);
                      free_map_release (&fs_map, new_dind, 1);
                      return INODE_ERROR;
                    } 
                  dind_ofs = 0;
                  lock_acquire (&GENGAR);
                  cached_inode_sector = cache_find_sector (inode->sector); 
                  if (cached_inode_sector == NULL)
                    cached_inode_sector = cache_fetch (inode->sector, 
                                                       INODE_DATA);
                  inode_disk = (struct disk_inode *) 
                               &cached_inode_sector->data[ofs];
                  inode_disk->i_sectors[DIRECT_SECTORS] = new_dind;
                  cached_inode_sector->dirty = true;
                  lock_release (&GENGAR);
                  inode->dir_sectors++;
                }
              else
                {
                  if (inode->dind_sectors == max_sectors)
                    {
                      free_map_release (&fs_map, new_sector, 1);
                      free_map_release (&fs_map, new_ind, 1);
                      return INODE_ERROR;
                    }
                  dind_ofs = inode->dind_sectors;                
                  lock_acquire (&GENGAR);
                  cached_inode_sector = cache_find_sector (inode->sector); 
                  if (cached_inode_sector == NULL)
                    cached_inode_sector = cache_fetch (inode->sector, 
                                                       INODE_DATA);
                  inode_disk = (struct disk_inode *) 
                               &cached_inode_sector->data[ofs];
                  new_dind = inode_disk->i_sectors[DIRECT_SECTORS];
                  lock_release (&GENGAR);
                }
              lock_acquire (&GENGAR);
              if ((cached_new_dind = cache_find_sector (new_dind)) == NULL)
                cached_new_dind = cache_fetch (new_dind, INODE_METADATA);
              cache_write (cached_new_dind, &new_ind, dind_ofs, 
                           sizeof (block_sector_t));  
              lock_release (&GENGAR);
              inode->dind_sectors++;
            }
          lock_acquire (&GENGAR);
          cached_new_ind = cache_find_sector (new_ind);
          if (cached_new_ind == NULL)
            cached_new_ind = cache_fetch (new_ind, INODE_METADATA);
          cache_write (cached_new_ind, &new_sector, 0,
                       sizeof (block_sector_t));
          lock_release (&GENGAR);
          inode->ind_sectors++;
        }
      else
        {
          block_sector_t final_sector = inode->dir_sectors - 1;
          struct cache_block *cached_final;
          if (inode->ind_sectors == 0)
            {
              lock_acquire (&GENGAR);
              cached_inode_sector = cache_find_sector (inode->sector); 
              if (cached_inode_sector == NULL)
                cached_inode_sector = cache_fetch (inode->sector, 
                                                   INODE_DATA);
              inode_disk = (struct disk_inode *) 
                           &cached_inode_sector->data[ofs];
              inode_disk->i_sectors[final_sector] = new_sector;
              cached_inode_sector->dirty = true;
              lock_release (&GENGAR); 
            }
          else
            {
              final_sector = direct_lookup (inode, inode->dir_sectors - 1);
              size_t ofs = inode->sectors % BLOCKNUMS_PER_IND;
              if (inode->dind_sectors > 0)      
                final_sector = indirect_lookup (final_sector, inode->sectors);
              lock_acquire (&GENGAR);
              if ((cached_final = cache_find_sector (final_sector)) == NULL)
                cached_final = cache_fetch (final_sector, INODE_METADATA);
              cache_write (cached_final, &new_sector, ofs, 
                           sizeof (block_sector_t));  
              lock_release (&GENGAR);    
            }
        }
    }

  /* Zero the new sector if file setup succeeded. */
  static char zeroes[BLOCK_SECTOR_SIZE];
  struct cache_block *cached_new_sector;
  lock_acquire (&GENGAR);
  if ((cached_new_sector = cache_find_sector (new_sector)) == NULL)
    cached_new_sector = cache_fetch (new_sector, FILE_DATA);
  cache_write (cached_new_sector, zeroes, 0, BLOCK_SECTOR_SIZE); 
  lock_release (&GENGAR);
  inode->sectors++;
  return new_sector;
}

/* Initializes an inode with LENGTH bytes of data and
   writes the new inode to sector SECTOR on the file system
   device.
   Returns true if successful.
   Returns false if memory or disk allocation fails. */
bool
inode_create (block_sector_t inumber, off_t length, bool is_dir)
{
  struct disk_inode *inode_disk = NULL;
  bool success = false;

  ASSERT (length >= 0);
  
  /* If this assertion fails, inode algorithms will fail. */
  ASSERT (sizeof *inode_disk == INODE_SIZE);

  inode_disk = calloc (1, sizeof *inode_disk);
  if (inode_disk == NULL)
    return false;
  size_t sectors = bytes_to_sectors (length);
  inode_disk->length = length;
  inode_disk->dir = is_dir;
  inode_disk->large = sectors > DIRECT_SECTORS;
  inode_disk->magic = (uint16_t) INODE_MAGIC;
  block_sector_t inode_sector = inode_sector_from_inumber (inumber);
  block_sector_t inode_ofs = byte_ofs_inode_sector (inumber);
  if (inode_sector == INODE_ERROR)
    success = false;
  else
    {
      struct cache_block *cached_inode_sector; 
      lock_acquire (&GENGAR);
      if ((cached_inode_sector = cache_find_sector (inode_sector)) == NULL)
        cached_inode_sector = cache_fetch (inode_sector, INODE_DATA);
      cache_write (cached_inode_sector, inode_disk, inode_ofs,
                   sizeof inode_disk);
      lock_release (&GENGAR);
      size_t curr_sector;
      struct inode dummy_inode;
      dummy_inode.sectors = 0;
      dummy_inode.dir_sectors = 0;
      dummy_inode.ind_sectors = 0;
      dummy_inode.dind_sectors = 0;
      dummy_inode.length = length;
      dummy_inode.large = inode_disk->large;
      dummy_inode.inumber = inumber;
      for (curr_sector = 0; curr_sector < sectors; curr_sector++)
        {
	  if (inode_append_sector (&dummy_inode) == INODE_ERROR)
	    {
	      inode_close_tree (&dummy_inode); 
	      success = false;
	      break;
	    }
        }
      success = true;
    }
  free (inode_disk);
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
  block_sector_t inode_sector = inode_sector_from_inumber (inumber);
  size_t inode_ofs = inumber % INODES_PER_SECTOR;

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
  slock_init (&inode->sl);
  struct cache_block *cached_inode_sector; 
  struct disk_inode *inode_disk;
  lock_acquire (&GENGAR);
  if ((cached_inode_sector = cache_find_sector (inode_sector)) == NULL)
    cached_inode_sector = cache_fetch (inode_sector, INODE_DATA);
  inode_disk = ((struct disk_inode *) cached_inode_sector->data) + inode_ofs;
  inode->length = inode_disk->length;
  inode->large = inode_disk->large;
  inode->dir = inode_disk->dir;
  lock_release (&GENGAR);
  inode->sector = inode_sector_from_inumber (inumber);
  inode->inumber = inumber;
  inode->sectors = bytes_to_sectors (inode->length);
  if (inode->large)
    {
      inode->dir_sectors = ((inode->sectors - 1) / BLOCKNUMS_PER_IND) + 1;
      if (inode->dir_sectors >= DIRECT_SECTORS)
        {
	  inode->dir_sectors = DIRECT_SECTORS;
          inode->ind_sectors = (DIRECT_SECTORS - 1) * BLOCKNUMS_PER_IND;
	  inode->dind_sectors = inode->sectors - inode->ind_sectors;
	  inode->ind_sectors += (inode->dind_sectors - 1) / BLOCKNUMS_PER_IND +
	                         1;
        }
      else
        {
          inode->ind_sectors = inode->sectors;
          inode->dind_sectors = 0;
        }
    }
  else
    {
      inode->dir_sectors = inode->sectors;
      inode->ind_sectors = 0;
      inode->dind_sectors = 0;
    }
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
  lock_acquire_exclusive (&inode->sl);
  if (inode != NULL)
    inode->open_cnt++;
  lock_release_exclusive (&inode->sl);
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
  size_t curr_dir;
  size_t curr_ind;
  size_t curr_dind;
  for (curr_dir = 0; curr_dir < inode->dir_sectors; curr_dir++)
    {
      block_sector_t del_dir;
      del_dir = direct_lookup (inode, curr_dir);
      size_t num_ind;
      if (inode->ind_sectors > 0)
        {
          if (curr_dir < DOUBIND_SECTOR)
            num_ind = (curr_dir == inode->dir_sectors - 1) ? 
                      ((inode->ind_sectors - 1) % BLOCKNUMS_PER_IND) + 1 : 
                      BLOCKNUMS_PER_IND;
          else
	        num_ind = ((inode->dind_sectors - 1) / BLOCKNUMS_PER_IND) + 1;
        }
      for (curr_ind = 0; curr_ind < num_ind; curr_ind++)
        {
          block_sector_t del_ind;
	      del_ind = lookup_in_sector (del_dir, curr_ind);
	      if (curr_dir == DOUBIND_SECTOR)
            {
		      size_t num_dind = (curr_ind == (num_ind - 1)) ? 
                                 ((inode->ind_sectors - 1) %
                                 BLOCKNUMS_PER_IND) + 1 :
                                 BLOCKNUMS_PER_IND;
		      for (curr_dind = 0; curr_dind < num_dind; curr_dind++)
		        {
		          block_sector_t del_dind;
		          del_dind = lookup_in_sector (del_ind, curr_dind);
		          free_map_release (&fs_map, del_dind, 1);
		        }
	        }   
	      free_map_release (&fs_map, del_ind, 1);
	    }
      free_map_release (&fs_map, del_dir, 1);
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
    
  /* Check if read access passed EOF */  
  lock_acquire_shared (&inode->sl);
  off_t length = inode_length (inode);
  if (length < offset)
    {
      lock_release_shared (&inode->sl);
      return 0;
    }
  while (size > 0) 
    {
      /* Disk sector to read, starting byte offset within sector. */
      size_t sectors_into_file = bytes_to_sectors (offset);
      block_sector_t sector = inode_lookup (inode, sectors_into_file);
      int sector_ofs = offset % BLOCK_SECTOR_SIZE;

      /* Bytes left in inode, bytes left in sector, lesser of the two. */
      off_t inode_left = inode_length (inode) - offset;
      int sector_left = BLOCK_SECTOR_SIZE - sector_ofs;
      int min_left = inode_left < sector_left ? inode_left : sector_left;

      /* Number of bytes to actually copy out of this sector. */
      int chunk_size = size < min_left ? size : min_left;
      if (chunk_size <= 0)
        break;
      
      struct cache_block *cached_sector;
      lock_acquire (&GENGAR);
      if ((cached_sector = cache_find_sector (sector)) == NULL)
        cached_sector = cache_fetch (sector, FILE_DATA);
      memcpy (buffer + bytes_read, cached_sector->data + sector_ofs,
              chunk_size);
      lock_release (&GENGAR);                                        

      /* Advance. */
      size -= chunk_size;
      offset += chunk_size;
      bytes_read += chunk_size;
    }
  lock_release_shared (&inode->sl);
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
  block_sector_t inode_sector = inode_sector_from_inumber (inode->inumber);
  size_t inode_ofs = inode->inumber % INODES_PER_SECTOR;
  struct cache_block *cached_sector;
  struct cache_block *cached_inode;
  struct disk_inode *inode_disk;

  lock_acquire_exclusive (&inode->sl);
  if (inode->deny_write_cnt)
    {
      lock_release_exclusive (&inode->sl);
      return 0;
    }

  /* Extend file with empty data sectors */
  off_t length = inode_length (inode);
  block_sector_t new_sector;
  while (length < offset)
    {
      new_sector = inode_append_sector (inode);
      if (new_sector == INODE_ERROR)
        {
          lock_release_exclusive (&inode->sl);
          return 0;
        }
      length += BLOCK_SECTOR_SIZE;      
    }
  while (size > 0) 
    {
      if (length < offset)
        {            
          new_sector = inode_append_sector (inode);
           if (new_sector == INODE_ERROR)
            {
              lock_release_exclusive (&inode->sl);
              return bytes_written;
            }
          length += BLOCK_SECTOR_SIZE;      
        }

      /* Sector to write, starting byte offset within sector. */
      size_t sectors_into_file = bytes_to_sectors (offset);
      block_sector_t sector = inode_lookup (inode, sectors_into_file);
      int sector_ofs = offset % BLOCK_SECTOR_SIZE;
      int sector_left = BLOCK_SECTOR_SIZE - sector_ofs;

      /* Bytes to write for this sector */
      int chunk_size = size < sector_left ? size : sector_left;
      if (chunk_size <= 0)
        break;
     
      lock_acquire (&GENGAR);
      if ((cached_sector = cache_find_sector (sector)) == NULL)
        cached_sector = cache_fetch (sector, FILE_DATA);
      cache_write (cached_sector, buffer, sector_ofs, chunk_size);  
      if ((cached_inode = cache_find_sector (inode_sector)) == NULL)
        cached_inode = cache_fetch (inode_sector, INODE_DATA);
      inode_disk = ((struct disk_inode *) cached_inode->data) + inode_ofs;
      inode_disk->length += (size_t) chunk_size;
      cached_inode->dirty = true;
      lock_release (&GENGAR);

      /* Advance. */
      size -= chunk_size;
      offset += chunk_size;
      bytes_written += chunk_size;
    }
  inode->length = offset;
  lock_release_exclusive (&inode->sl);
  return bytes_written;
}

/* Disables writes to INODE.
   May be called at most once per inode opener. */
void
inode_deny_write (struct inode *inode) 
{
  lock_acquire_exclusive (&inode->sl);
  inode->deny_write_cnt++;
  ASSERT (inode->deny_write_cnt <= inode->open_cnt);
  lock_release_exclusive (&inode->sl);
}

/* Re-enables writes to INODE.
   Must be called once by each inode opener who has called
   inode_deny_write() on the inode, before closing the inode. */
void
inode_allow_write (struct inode *inode) 
{
  lock_acquire_exclusive (&inode->sl);
  ASSERT (inode->deny_write_cnt > 0);
  ASSERT (inode->deny_write_cnt <= inode->open_cnt);
  inode->deny_write_cnt--;
  lock_release_exclusive (&inode->sl);
}

/* Returns the length, in bytes, of INODE's data. */
off_t
inode_length (const struct inode *inode)
{
  return inode->length;
}

/* Returns whether an inode is a directory or not. */
bool
inode_isdir (const struct inode *inode)
{
  return inode->dir;
}

/* Returns the inumber representing the inode. */
block_sector_t 
inode_inumber (const struct inode inode *);
{
  return inode->inumber;
}


