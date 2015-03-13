what would we do without you TM 69
diff --git a/pintos/src/filesys/.DESIGNDOC.swp b/pintos/src/filesys/.DESIGNDOC.swp
deleted file mode 100644
index c3da24a..0000000
Binary files a/pintos/src/filesys/.DESIGNDOC.swp and /dev/null differ
diff --git a/pintos/src/filesys/.cache.c.swn b/pintos/src/filesys/.cache.c.swn
deleted file mode 100644
index f82e91d..0000000
Binary files a/pintos/src/filesys/.cache.c.swn and /dev/null differ
diff --git a/pintos/src/filesys/.cache.c.swo b/pintos/src/filesys/.cache.c.swo
deleted file mode 100644
index e69de29..0000000
diff --git a/pintos/src/filesys/.cache.h.swn b/pintos/src/filesys/.cache.h.swn
deleted file mode 100644
index f84b750..0000000
Binary files a/pintos/src/filesys/.cache.h.swn and /dev/null differ
diff --git a/pintos/src/filesys/.cache.h.swo b/pintos/src/filesys/.cache.h.swo
deleted file mode 100644
index e69de29..0000000
diff --git a/pintos/src/filesys/.directory.c.swn b/pintos/src/filesys/.directory.c.swn
deleted file mode 100644
index 5dbd4bc..0000000
Binary files a/pintos/src/filesys/.directory.c.swn and /dev/null differ
diff --git a/pintos/src/filesys/.directory.c.swo b/pintos/src/filesys/.directory.c.swo
deleted file mode 100644
index b45c9de..0000000
Binary files a/pintos/src/filesys/.directory.c.swo and /dev/null differ
diff --git a/pintos/src/filesys/.directory.h.swo b/pintos/src/filesys/.directory.h.swo
deleted file mode 100644
index e69de29..0000000
diff --git a/pintos/src/filesys/.file.c.swi b/pintos/src/filesys/.file.c.swi
deleted file mode 100644
index cf96084..0000000
Binary files a/pintos/src/filesys/.file.c.swi and /dev/null differ
diff --git a/pintos/src/filesys/.filesys.c.swk b/pintos/src/filesys/.filesys.c.swk
deleted file mode 100644
index 50a6740..0000000
Binary files a/pintos/src/filesys/.filesys.c.swk and /dev/null differ
diff --git a/pintos/src/filesys/.filesys.c.swo b/pintos/src/filesys/.filesys.c.swo
index 1df8c7e..e69de29 100644
Binary files a/pintos/src/filesys/.filesys.c.swo and b/pintos/src/filesys/.filesys.c.swo differ
diff --git a/pintos/src/filesys/.filesys.h.swm b/pintos/src/filesys/.filesys.h.swm
deleted file mode 100644
index e69de29..0000000
diff --git a/pintos/src/filesys/.free-map.c.swo b/pintos/src/filesys/.free-map.c.swo
deleted file mode 100644
index 091d0a3..0000000
Binary files a/pintos/src/filesys/.free-map.c.swo and /dev/null differ
diff --git a/pintos/src/filesys/.free-map.h.swp b/pintos/src/filesys/.free-map.h.swp
deleted file mode 100644
index f4f3b1e..0000000
Binary files a/pintos/src/filesys/.free-map.h.swp and /dev/null differ
diff --git a/pintos/src/filesys/.inode.c.swm b/pintos/src/filesys/.inode.c.swm
deleted file mode 100644
index c9adf2f..0000000
Binary files a/pintos/src/filesys/.inode.c.swm and /dev/null differ
diff --git a/pintos/src/filesys/.inode.h.swn b/pintos/src/filesys/.inode.h.swn
deleted file mode 100644
index 98bfcfb..0000000
Binary files a/pintos/src/filesys/.inode.h.swn and /dev/null differ
diff --git a/pintos/src/filesys/DESIGNDOC b/pintos/src/filesys/DESIGNDOC
deleted file mode 100644
index 0ee6559..0000000
--- a/pintos/src/filesys/DESIGNDOC
+++ /dev/null
@@ -1,222 +0,0 @@
-       	     +-------------------------+
-		     |		   CS 140	       |
-		     | PROJECT 4: FILE SYSTEMS |
-		     |	   DESIGN DOCUMENT     |
-		     +-------------------------+
-
----- GROUP ----
-
->> Fill in the names and email addresses of your group members.
-
-FirstName LastName <email@domain.example>
-FirstName LastName <email@domain.example>
-FirstName LastName <email@domain.example>
-
----- PRELIMINARIES ----
-
->> If you have any preliminary comments on your submission, notes for the
->> TAs, or extra credit, please give them here.
-
->> Please cite any offline or online sources you consulted while
->> preparing your submission, other than the Pintos documentation, course
->> text, lecture notes, and course staff.
-
-We reviewed Unix filesystem code from CS 110 assignment 1
-
-		     INDEXED AND EXTENSIBLE FILES
-		     ============================
-
----- DATA STRUCTURES ----
-
->> A1: Copy here the declaration of each new or changed `struct' or
->> `struct' member, global or static variable, `typedef', or
->> enumeration.  Identify the purpose of each in 25 words or less.
-
->> A2: What is the maximum size of a file supported by your inode
->> structure?  Show your work.
-
-There's an array of 8 sectors in our filesystem. For large files, 7 of these are
-indirect sectors, meaning they hold the sector numbers of 256 other data
-sectors. The 8th sector is double indirect, which holds the sector numbers of
-256 indirect sectors. 
-This adds to 512 bytes per sector * (7*256 + 1*256^2) = 34,471,936 bytes
-
----- SYNCHRONIZATION ----
-
->> A3: Explain how your code avoids a race if two processes attempt to
->> extend a file at the same time.
-
->> A4: Suppose processes A and B both have file F open, both
->> positioned at end-of-file.  If A reads and B writes F at the same
->> time, A may read all, part, or none of what B writes.  However, A
->> may not read data other than what B writes, e.g. if B writes
->> nonzero data, A is not allowed to see all zeros.  Explain how your
->> code avoids this race.
-
->> A5: Explain how your synchronization design provides "fairness".
->> File access is "fair" if readers cannot indefinitely block writers
->> or vice versa.  That is, many processes reading from a file cannot
->> prevent forever another process from writing the file, and many
->> processes writing to a file cannot prevent another process forever
->> from reading the file.
-
----- RATIONALE ----
-
->> A6: Is your inode structure a multilevel index?  If so, why did you
->> choose this particular combination of direct, indirect, and doubly
->> indirect blocks?  If not, why did you choose an alternative inode
->> structure, and what advantages and disadvantages does your
->> structure have, compared to a multilevel index?
-
-We have two inode structure setups, small and large. Inodes representing small
-files just hold an array of 8 sectors. Each sector holds 512 bytes. We have such
-a simple case because most files are small enough to fit into just a small inode
-structure.
-Our second inode structure, the large structure, is 7 indirect and 1 doubly
-indirect inode. Unfortunately, this is a large jump in size from small to large 
-(4096 to 34,471,936), but there is still very little internal fragmentation. We 
-allocate sectors as they are needed. For example, say 4096 byte file needs one
-more byte. This won't fit in a small file, so all 4097 bytes are read into the
-first indirect sector of a large file. There's now an overhead of one sector; 9
-to hold the file data, and 1 to inderectly encapsulate the 9 data sectors. There
-would be 1 page of overhead for each indirect sector we use and a maximum of 257
-pages of overhead attributed to the doubly indirect sector.
-This is relatively small overhead considering the theoretical capacity we are
-adding to the file..
-Also, having such a large file size ensures we have enough space, considering or
-entire OS disk partition could fit in one large file.
-
-			    SUBDIRECTORIES
-			    ==============
-
----- DATA STRUCTURES ----
-
->> B1: Copy here the declaration of each new or changed `struct' or
->> `struct' member, global or static variable, `typedef', or
->> enumeration.  Identify the purpose of each in 25 words or less.
-
----- ALGORITHMS ----
-
->> B2: Describe your code for traversing a user-specified path.  How
->> do traversals of absolute and relative paths differ?
-
----- SYNCHRONIZATION ----
-
->> B4: How do you prevent races on directory entries?  For example,
->> only one of two simultaneous attempts to remove a single file
->> should succeed, as should only one of two simultaneous attempts to
->> create a file with the same name, and so on.
-
->> B5: Does your implementation allow a directory to be removed if it
->> is open by a process or if it is in use as a process's current
->> working directory?  If so, what happens to that process's future
->> file system operations?  If not, how do you prevent it?
-
----- RATIONALE ----
-
->> B6: Explain why you chose to represent the current directory of a
->> process the way you did.
-
-			     BUFFER CACHE
-			     ============
-
----- DATA STRUCTURES ----
-
->> C1: Copy here the declaration of each new or changed `struct' or
->> `struct' member, global or static variable, `typedef', or
->> enumeration.  Identify the purpose of each in 25 words or less.
-
-In cache.h
-----------
-
-The enums are to distinguish the different types of data that could be in the
-cache.
-
-enum sector_type
-  {
-    FILE_DATA = 1,     /* Cached fileblock represents file data. */
-    INODE_DATA = 2,    /* Cached fileblock represents inode. */
-    INODE_METADATA = 3 /* Cached fileblock represents inode metadata. */
-  };
-
-In cache.c
-----------
-struct buffer_cache
-  {
-    struct bitmap *used_blocks;
-    struct hash cache_segment;
-    struct cache_block *used_sectors[CACHE_MAX];
-    int cursor;                                  /* Keep track of clock hand */
-    int evicting_hand; 
-  };
-
-struct cache_block
-  {
-    struct hash_elem elem;
-    block_sector_t sector;
-    enum sector_type type;
-    char data[BLOCK_SECTOR_SIZE];
-    char accessed; /* For reads or writes */
-    bool in_use;
-    bool dirty; /* For writes only */
-  };
----- ALGORITHMS ----
-
->> C2: Describe how your cache replacement algorithm chooses a cache
->> block to evict.
-
-We use a clock algorithm to replace cache blocks. The cache is internally stored
-as a hash that links sector numbers to buckets of the hash. The clock algorithm
-however, circles around a statically sized array. The cursor hand goes through
-and will either it will decrement the cache block's usage frequency. The
-evicting hand will delete a cache block from the array, return the index it
-deleted at. This index will either be a position of a cache block that was freed
-since the last time the eviction mechanism was invoked or a cache block whose
-usage frequency was decremented to zero by the cursor before.
-We chose to represent the clock as an array because it's easy to continuously
-loop through an array of 64 elements, and replacing evicted or deleted cache
-blocks with new ones is constant time after a position is found. The parallel
-hash allows for contant time lookup into the cache when something tries to
-access file data. Using both data structures gives us the best of both worlds.
-
->> C3: Describe your implementation of write-behind.
-
->> C4: Describe your implementation of read-ahead.
-
----- SYNCHRONIZATION ----
-
->> C5: When one process is actively reading or writing data in a
->> buffer cache block, how are other processes prevented from evicting
->> that block?
-
->> C6: During the eviction of a block from the cache, how are other
->> processes prevented from attempting to access the block?
-
----- RATIONALE ----
-
->> C7: Describe a file workload likely to benefit from buffer caching,
->> and workloads likely to benefit from read-ahead and write-behind.
-
-			   SURVEY QUESTIONS
-			   ================
-
-Answering these questions is optional, but it will help us improve the
-course in future quarters.  Feel free to tell us anything you
-want--these questions are just to spur your thoughts.  You may also
-choose to respond anonymously in the course evaluations at the end of
-the quarter.
-
->> In your opinion, was this assignment, or any one of the three problems
->> in it, too easy or too hard?  Did it take too long or too little time?
-
->> Did you find that working on a particular part of the assignment gave
->> you greater insight into some aspect of OS design?
-
->> Is there some particular fact or hint we should give students in
->> future quarters to help them solve the problems?  Conversely, did you
->> find any of our guidance to be misleading?
-
->> Do you have any suggestions for the TAs to more effectively assist
->> students in future quarters?
-
->> Any other comments?
diff --git a/pintos/src/filesys/cache.c b/pintos/src/filesys/cache.c
index f4ece17..f8603fc 100644
--- a/pintos/src/filesys/cache.c
+++ b/pintos/src/filesys/cache.c
@@ -108,6 +108,15 @@ cache_delete (struct cache_block *cache_block)
     block_write (fs_device, cache_block->sector, cache_block->data);
 }
 
+/* Writes to a cached sector, setting its dirty bool. */
+void
+cache_write (struct cache_block *cached_sector, void *buf,
+	     size_t start, size_t cnt)
+{
+  cached_sector->dirty = true;
+  memcpy (&cached_sector->data[start], buf, cnt);
+}
+
 /* Flush all data from cache to filesystem, deleting cache. */
 void
 cache_flush (void)
diff --git a/pintos/src/filesys/cache.h b/pintos/src/filesys/cache.h
index 65242c6..6d3a858 100644
--- a/pintos/src/filesys/cache.h
+++ b/pintos/src/filesys/cache.h
@@ -30,6 +30,7 @@ struct lock GENGAR;
 void cache_init (void);
 struct cache_block *cache_find_sector (block_sector_t);
 struct cache_block *cache_fetch (block_sector_t, enum sector_type);
+void cache_write (struct cache_block *, void *, size_t, size_t);
 void cache_delete (struct cache_block *);
 void cache_flush (void);
 
diff --git a/pintos/src/filesys/directory.c b/pintos/src/filesys/directory.c
index 598e9f9..030c1c9 100644
--- a/pintos/src/filesys/directory.c
+++ b/pintos/src/filesys/directory.c
@@ -16,7 +16,6 @@ struct dir
 /* A single directory entry. */
 struct dir_entry 
   {
-    block_sector_t inumber;
     block_sector_t inode_sector;        /* Sector number of header. */
     char name[NAME_MAX + 1];            /* Null terminated file name. */
     bool in_use;                        /* In use or free? */
@@ -32,30 +31,27 @@ dir_create (block_sector_t sector, size_t entry_cnt)
 
 /* Opens and returns the directory for the given INODE, of which
    it takes ownership.  Returns a null pointer on failure. */
-struct file *
+struct dir *
 dir_open (struct inode *inode) 
 {
-  struct file *file = calloc (1, sizeof *file);
-  lock_acquire (&GENGAR);
-  struct disk_node *d_node;
-
-  if (inode->d_inode != NULL && file != NULL && inode->d_inode->dir)
+  struct dir *dir = calloc (1, sizeof *dir);
+  if (inode != NULL && dir != NULL)
     {
-      file->inode = inode;
-      file->pos = 0;
-      return file;
+      dir->inode = inode;
+      dir->pos = 0;
+      return dir;
     }
   else
     {
       inode_close (inode);
-      free (file);
+      free (dir);
       return NULL; 
     }
 }
 
 /* Opens the root directory and returns a directory for it.
    Return true if successful, false on failure. */
-struct file *
+struct dir *
 dir_open_root (void)
 {
   return dir_open (inode_open (ROOT_DIR_SECTOR));
@@ -63,19 +59,17 @@ dir_open_root (void)
 
 /* Opens and returns a new directory for the same inode as DIR.
    Returns a null pointer on failure. */
-struct file *
-dir_reopen (struct file *dir) 
+struct dir *
+dir_reopen (struct dir *dir) 
 {
-  if (dir->inode->dir) 
-    return dir_open (inode_reopen (dir->inode));
-  return NULL;
+  return dir_open (inode_reopen (dir->inode));
 }
 
 /* Destroys DIR and frees associated resources. */
 void
-dir_close (struct file *dir) 
+dir_close (struct dir *dir) 
 {
-  if (dir != NULL && dir->inode->dir)
+  if (dir != NULL)
     {
       inode_close (dir->inode);
       free (dir);
@@ -84,7 +78,7 @@ dir_close (struct file *dir)
 
 /* Returns the inode encapsulated by DIR. */
 struct inode *
-dir_get_inode (struct file *dir) 
+dir_get_inode (struct dir *dir) 
 {
   return dir->inode;
 }
diff --git a/pintos/src/filesys/directory.h b/pintos/src/filesys/directory.h
index 7bfd3ef..930acf9 100644
--- a/pintos/src/filesys/directory.h
+++ b/pintos/src/filesys/directory.h
@@ -17,14 +17,14 @@ struct inode;
 bool dir_create (block_sector_t sector, size_t entry_cnt);
 struct dir *dir_open (struct inode *);
 struct dir *dir_open_root (void);
-struct dir *dir_reopen (struct file *);
-void dir_close (struct file *);
-struct inode *dir_get_inode (struct file *);
+struct dir *dir_reopen (struct dir *);
+void dir_close (struct dir *);
+struct inode *dir_get_inode (struct dir *);
 
 /* Reading and writing. */
-bool dir_lookup (const struct file *, const char *name, struct inode **);
-bool dir_add (struct file *, const char *name, block_sector_t);
-bool dir_remove (struct file *, const char *name);
-bool dir_readdir (struct file *, char name[NAME_MAX + 1]);
+bool dir_lookup (const struct dir *, const char *name, struct inode **);
+bool dir_add (struct dir *, const char *name, block_sector_t);
+bool dir_remove (struct dir *, const char *name);
+bool dir_readdir (struct dir *, char name[NAME_MAX + 1]);
 
 #endif /* filesys/directory.h */
diff --git a/pintos/src/filesys/filesys.c b/pintos/src/filesys/filesys.c
index 7688c25..c75f17e 100644
--- a/pintos/src/filesys/filesys.c
+++ b/pintos/src/filesys/filesys.c
@@ -7,15 +7,10 @@
 #include "filesys/inode.h"
 #include "filesys/directory.h"
 
-#define PERCENT_INODES 1 / 100
-
 /* Partition that contains the file system. */
 struct block *fs_device;
 struct free_map fs_map;
 
-/* First sector of non-inode data */
-block_size_t file_block_start;
-
 static void do_format (void);
 
 /* Initializes the file system module.
@@ -27,18 +22,14 @@ filesys_init (bool format)
   if (fs_device == NULL)
     PANIC ("No file system device found, can't initialize file system.");
 
-  file_block_start = block_size (fs_device) * PERCENT_INODES;
   inode_init ();
-  free_map_init (&inode_map, file_block_start, INODE_MAP_INODE);
   free_map_init (&fs_map, block_size (fs_device), FREE_MAP_INODE);
-  
+
   if (format) 
     do_format ();
 
-  free_map_open (&inode_map);
   free_map_open (&fs_map);
-  ASSERT (free_map_set_multiple (&inode_map, 0, RESERVED_INODES);
-  ASSERT (free_map_set_multiple (&fs_map, 0, file_block_start);
+  ASSERT (free_map_set_multiple (&fs_map, 0, file_block_start));
 }
 
 /* Shuts down the file system module, writing any unwritten data
@@ -47,7 +38,6 @@ void
 filesys_done (void) 
 {
   free_map_close (&fs_map);
-  free_map_close (&inode_map);
 }
 
 /* Creates a file named NAME with the given INITIAL_SIZE.
@@ -65,7 +55,7 @@ filesys_create (const char *name, off_t initial_size)
                   && inode_create (inode_sector)
                   && dir_add (dir, name, inode_sector));
   if (!success && inode_sector != INODE_ERROR) 
-    free_map_release (&inode_map, inode_sector, 1);
+    free_map_release (inode_sector, 1);
   dir_close (dir);
 
   return success;
@@ -107,11 +97,9 @@ static void
 do_format (void)
 {
   printf ("Formatting file system...");
-  free_map_create (&inode_map);
-  free_map_create (&fs_map);
+  free_map_create ();
   if (!dir_create (ROOT_DIR_SECTOR, 16))
     PANIC ("root directory creation failed");
-  free_map_close (&inode_map);
-  free_map_clost (&fs_map);
+  free_map_close ();
   printf ("done.\n");
 }
diff --git a/pintos/src/filesys/filesys.h b/pintos/src/filesys/filesys.h
index d0f9437..ddf4733 100644
--- a/pintos/src/filesys/filesys.h
+++ b/pintos/src/filesys/filesys.h
@@ -12,7 +12,6 @@
 
 /* Block device that contains the file system. */
 struct block *fs_device;
-struct free_map inode_map;
 
 void filesys_init (bool format);
 void filesys_done (void);
diff --git a/pintos/src/filesys/free-map.c b/pintos/src/filesys/free-map.c
index c76f8e0..977edf0 100644
--- a/pintos/src/filesys/free-map.c
+++ b/pintos/src/filesys/free-map.c
@@ -16,7 +16,6 @@ free_map_init (struct free_map *free_map, size_t size, size_t reserved_inode)
     PANIC ("bitmap creation failed-- map is too large");
   free_map->free_map_file = NULL;
   free_map->reserved_inode = reserved_inode;
-  lock_init (&free_map->bitmap_lock);
 }
 
 /* Allocates CNT consecutive sectors from the free map and stores
@@ -28,18 +27,14 @@ bool
 free_map_allocate (struct free_map *free_map, size_t start,
                    size_t cnt, block_sector_t *sectorp)
 {
-  lock_acquire (&free_map->bitmap_lock);
   block_sector_t sector = bitmap_scan_and_flip (free_map->free_sectors, start, 
                                                 cnt, false);
-  lock_release (&free_map->bitmap_lock);
   if (sector != BITMAP_ERROR
       && free_map->free_map_file != NULL
       && !bitmap_write (free_map->free_sectors, free_map->free_map_file))
     {
-      lock_acquire (&free_map->bitmap_lock);
       bitmap_set_multiple (free_map->free_sectors, sector, cnt, false); 
       sector = BITMAP_ERROR;
-      lock_release (&free_map->bitmap_lock);
     }
   if (sector != BITMAP_ERROR)
     *sectorp = sector;
@@ -51,21 +46,18 @@ free_map_allocate (struct free_map *free_map, size_t start,
 void
 free_map_set_multiple (struct free_map *free_map, size_t start, size_t cnt)
 {
-  lock_acquire (&free_map->bitmap_lock);
   bitmap_set_multiple (free_map->free_sectors, start, cnt, true);
-  lock_release (&free_map->bitmap_lock); //TODO check lock placement
-  ASSERT (free_map->free_map_file != NULL);
-  ASSERT (bitmap_write (free_map->free_sectors, free_map->free_map_file));
+  if (free_map->free_map_file == NULL)
+    return false;
+  return bitmap_write (free_map->free_sectors, free_map->free_map_file);
 }
 
 /* Makes CNT sectors starting at SECTOR available for use. */
 void
 free_map_release (struct free_map *free_map, block_sector_t sector, size_t cnt)
 {
-  lock_acquire (&free_map->bitmap_lock);
   ASSERT (bitmap_all (free_map->free_sectors, sector, cnt));
   bitmap_set_multiple (free_map->free_sectors, sector, cnt, false);
-  lock_release (&free_map->bitmap_lock);
   bitmap_write (free_map->free_sectors, free_map->free_map_file);
 }
 
diff --git a/pintos/src/filesys/free-map.h b/pintos/src/filesys/free-map.h
index 752504b..afde6ba 100644
--- a/pintos/src/filesys/free-map.h
+++ b/pintos/src/filesys/free-map.h
@@ -9,8 +9,7 @@ struct free_map
   {
     struct file *free_map_file;            /* Free map file. */
     struct bitmap *free_sectors;           /* Free map, one bit per sector. */
-    struct block_sector_t reserved_inode;  /* Reserve a growing file. */
-    struct lock bitmap_lock;               /* Lock for doing bitmap ops */
+    block_sector_t reserved_inode;         /* Reserve a growing file. */
   };
 
 void free_map_init (struct free_map *, size_t, size_t);
@@ -19,7 +18,7 @@ void free_map_open (struct free_map *);
 void free_map_close (struct free_map *);
 
 bool free_map_allocate (struct free_map *, size_t, size_t, block_sector_t *);
-void free_map_set_multiple (struct free_map *, size_t, size_t);
+bool free_map_set_multiple (struct free_map *, size_t, size_t);
 void free_map_release (struct free_map *, block_sector_t, size_t);
 
 #endif /* filesys/free-map.h */
diff --git a/pintos/src/filesys/inode.c b/pintos/src/filesys/inode.c
index 6baa565..150234d 100644
--- a/pintos/src/filesys/inode.c
+++ b/pintos/src/filesys/inode.c
@@ -11,29 +11,41 @@
 #define INODE_MAGIC 0x494e4f44
 #define INODE_ERROR -1
 
-#define DIRECT_SECTORS 8
-#define DOUBIND_SECTOR 7
+#define DIRECT_SECTORS 6
+#define DOUBIND_SECTOR 5
 #define BLOCKNUMS_PER_IND 256
 #define INODES_PER_SECTOR 16
 #define INODE_SIZE 32
 #define PERCENT_INODES 1 / 100
 
 /* On-disk inode.
-   Must be exactly BLOCK_SECTOR_SIZE bytes long. */
+   Must be INODE_SIZE bytes long. */
+struct disk_inode
+  {
+    off_t length;                        /* File size in bytes. */
+    block_sector_t i_sectors[6];         /* Sector numbers of disk locations. */
+    bool dir;                            /* True if directory, false otherwise. */
+    bool large;                          /* True if large block addressing.  */
+    uint16_t magic;                      /* Magic number. */
+  };
+
+static inline size_t
+byte_ofs_inode_sector (size_t inumber)
+{
+  return (inumber % INODES_PER_SECTOR) * INODE_SIZE;
+}
+
+/* In-memory inode. */
 struct inode
   {
     struct list_elem elem;               /* Element in inode list. */
-    off_t length;                        /* File size in bytes. */
-    block_sector_t i_sectors[8];         /* Sector numbers of disk locations. */
+    block_sector_t inumber;              /* Inumber that represents inode. */
     int open_cnt;                        /* Number of openers. */
     bool removed;                        /* True if deleted, false otherwise. */
-    bool dir;                            /* True if directory, false otherwise. */
-    bool large;                          /* True if large block addressing.  */
     int deny_write_cnt;                  /* 0: writes ok, >0: deny writes. */
-    unsigned magic;                      /* Magic number. */
-    char unused[8];                      /* Unused bytes to align to 32 byte inode. */
   };
 
+static size_t file_block_start;
 static size_t num_inodes;
 
 static block_sector_t small_lookup (struct inode *inode, int block_idx);
@@ -106,7 +118,9 @@ large_lookup (struct inode *inode, int block_idx)
     {
       indirect_ofs = (indirect_idx - DOUBIND_SECTOR) / BLOCKNUMS_PER_IND;
       int dindirect_ofs = (indirect_idx - DOUBIND_SECTOR) % BLOCKNUMS_PER_IND;
-      block_sector_t dindirect_idx = lookup_in_sector (fs, inode->i_addr[7], indirect_ofs);
+      block_sector_t dindirect_idx = lookup_in_sector (fs, 
+                                     inode->i_addr[DOUBIND_SECTOR], 
+				     indirect_ofs);
       file_data_sector = lookup_in_sector (fs, dindirect_idx, dindirect_ofs);
     }
   return file_data_sector;
@@ -120,7 +134,7 @@ lookup_in_sector (block_sector_t file_data_sector, int sector_ofs)
   block_sector_t ret_sector;
   lock_acquire (&GENGAR);
   if ((cached_sector = cache_find_sector (file_data_sector)) == NULL)
-    cache_fetch (file_data_sector, INODE_METADATA, cached_sector);
+    cached_sector = cache_fetch (file_data_sector, INODE_METADATA);
   ret_sector = ((unit16_t *) cached_sector->data)[sector_ofs];
   lock_release (&GENGAR);
   return cached_sector[sector_ofs];
@@ -155,41 +169,48 @@ inode_create (block_sector_t inumber, bool is_dir)
 
   ASSERT (length >= 0);
   
-  ASSERT (sizeof new_inode == INODE_SIZE);
-  new_inode->removed = false;
-  new_inode->dir = is_dir;
-  new_inode->large = false;
-  new_inode->magic = INODE_MAGIC;
+  /* If this assertion fails, inode algorithms will fail. */
+  ASSERT (sizeof *inode_disk == BLOCK_SECTOR_SIZE);
+
+  inode_disk->dir = is_dir;
+  inode_disk->large = false;
+  inode_disk->magic = INODE_MAGIC;
   block_sector_t sector = inode_sector_from_inumber (inumber);
   if (sector == INODE_ERROR)
-    return false;
-  struct cache_block *cached_inode_sector; 
-  lock_acquire (&GENGAR);
-  if ((*cached_inode_sector = cache_find_sector (sector)) == NULL)
-    cached_inode_sector = cache_fetch (sector, INODE_DATA);
-  cache_write (cached_inode_sector, inode_disk, 
-               byte_ofs_inode_sector (inumber), sizeof (inode_disk));
-  lock_release (&GENGAR);
-  return true;
+    success = false;  
+  else
+    {
+      struct cache_block *cached_inode_sector; 
+      lock_acquire (&GENGAR);
+      if ((*cached_inode_sector = cache_find_sector (sector)) == NULL)
+        cached_inode_sector = cache_fetch (sector, INODE_DATA);
+      cache_write (cached_inode_sector, inode_disk,
+                   byte_ofs_inode_sector (inumber), sizeof inode_disk);
+      lock_release (&GENGAR);
+      success = true;
+    }
+  return success;
 }
 
-/* Reads an inode from SECTOR
+/* Reads an inode referred to by its inumber
    and returns a `struct inode' that contains it.
    Returns a null pointer if memory allocation fails. */
 struct inode *
-inode_open (block_sector_t sector)
+inode_open (block_sector_t inumber)
 {
   struct list_elem *e;
   struct inode *inode;
 
   /* Check whether this inode is already open. */
+  lock_acquire (&inode_lock);
   for (e = list_begin (&open_inodes); e != list_end (&open_inodes);
        e = list_next (e)) 
     {
       inode = list_entry (e, struct inode, elem);
-      if (inode->sector == sector) 
+      if (inode->inumber == inumber) 
         {
           inode_reopen (inode);
+          lock_release (&inode_lock);
           return inode; 
         }
     }
@@ -201,11 +222,11 @@ inode_open (block_sector_t sector)
 
   /* Initialize. */
   list_push_front (&open_inodes, &inode->elem);
-  inode->sector = sector;
+  inode->inumber = inumber;
   inode->open_cnt = 1;
   inode->deny_write_cnt = 0;
   inode->removed = false;
-  block_read (fs_device, inode->sector, &inode->data);
+  lock_release (&inode_lock);
   return inode;
 }
 
@@ -229,6 +250,7 @@ inode_close (struct inode *inode)
     return;
 
   /* Release resources if this was the last opener. */
+  lock_acquire (&inode_lock);
   if (--inode->open_cnt == 0)
     {
       /* Remove from inode list and release lock. */
@@ -237,11 +259,9 @@ inode_close (struct inode *inode)
       /* Deallocate blocks if removed. */
       if (inode->removed) 
         {
-          free_map_release (inode->sector, 1);
-          free_map_release (inode->data.start,
-                            bytes_to_sectors (inode->data.length)); 
+	  inode_close_tree (inode->inumber);
+          free_map_release (&inode_map, inode->inumber, 1);
         }
-
       free (inode); 
     }
 }
diff --git a/pintos/src/filesys/inode.h b/pintos/src/filesys/inode.h
index e48bc66..3a700a2 100644
--- a/pintos/src/filesys/inode.h
+++ b/pintos/src/filesys/inode.h
@@ -6,11 +6,11 @@
 #include "devices/block.h"
 
 /* First sector of non-inode data. */
-block_size_t file_block_start;
+block_sector_t file_block_start;
 
+struct free_map inode_map;
  
 void inode_init (void);
-block_sector_t inode_next_inumber ();
 bool inode_create (block_sector_t, bool);
 struct inode *inode_open (block_sector_t);
 struct inode *inode_reopen (struct inode *);
diff --git a/pintos/src/lib/kernel/.bitmap.c.swm b/pintos/src/lib/kernel/.bitmap.c.swm
deleted file mode 100644
index 6b93c53..0000000
Binary files a/pintos/src/lib/kernel/.bitmap.c.swm and /dev/null differ
diff --git a/pintos/src/lib/kernel/.hash.h.swn b/pintos/src/lib/kernel/.hash.h.swn
deleted file mode 100644
index e69de29..0000000
diff --git a/pintos/src/lib/kernel/.hash.h.swo b/pintos/src/lib/kernel/.hash.h.swo
deleted file mode 100644
index e69de29..0000000
diff --git a/pintos/src/lib/kernel/bitmap.c b/pintos/src/lib/kernel/bitmap.c
index 8bfba51..7a68485 100644
--- a/pintos/src/lib/kernel/bitmap.c
+++ b/pintos/src/lib/kernel/bitmap.c
@@ -103,7 +103,7 @@ bitmap_extend (struct bitmap *bitmap, size_t bit_cnt)
     return bitmap;
   size_t old_count = bitmap->bit_cnt;
   bitmap->bit_cnt = bit_cnt;
-  bitmap->bits = realloc (byte_cnt (bit_cnt));
+  bitmap->bits = realloc (bitmap->bits, byte_cnt (bit_cnt));
   if (bitmap->bits != NULL)
     {
       bitmap_set_multiple (bitmap, old_count + 1, bitmap->bit_cnt, false);
diff --git a/pintos/src/threads/malloc.c b/pintos/src/threads/malloc.c
index 4c6c887..f6f803b 100644
--- a/pintos/src/threads/malloc.c
+++ b/pintos/src/threads/malloc.c
@@ -292,4 +292,3 @@ arena_to_block (struct arena *a, size_t idx)
                            + sizeof *a
                            + idx * a->desc->block_size);
 }
-
diff --git a/pintos/src/threads/synch.c b/pintos/src/threads/synch.c
index 7db6309..9368fd0 100644
--- a/pintos/src/threads/synch.c
+++ b/pintos/src/threads/synch.c
@@ -464,7 +464,7 @@ cond_sort_priority (struct condition *cond)
 }
 
 /* Initialize a shared lock. */
-void slock_init (shared_lock *sl)
+void slock_init (struct shared_lock *sl)
 {
   lock_init (&sl->slock);
   cond_init (&sl->cond);
@@ -473,7 +473,7 @@ void slock_init (shared_lock *sl)
 
 /* Acquire exclusive access to a shared lock. */
 void
-lock_acquire_exclusive (shared_lock *sl) 
+lock_acquire_exclusive (struct shared_lock *sl) 
 {
   lock_acquire (&sl->slock);
   while (sl->i != 0) 
@@ -484,7 +484,7 @@ lock_acquire_exclusive (shared_lock *sl)
 
 /* Acquire shared access to a shared lock. */
 void
-lock_acquire_shared (shared_lock *sl)
+lock_acquire_shared (struct shared_lock *sl)
 {
   lock_acquire (&sl->slock);
   while (sl->i < 0) 
@@ -495,20 +495,20 @@ lock_acquire_shared (shared_lock *sl)
 
 /* Release shared access to a shared lock. */
 void
-lock_release_shared (shared_lock *sl)
+lock_release_shared (struct shared_lock *sl)
 {
   lock_acquire (&sl->slock);
   if (!--sl->i)
-    cond_signal (&sl->cond);
+    cond_signal (&sl->cond, &sl->slock);
   lock_release (&sl->slock);
 }
 
 /* Release exclusive access to a shared lock. */
 void
-lock_release_exclusive (shared_lock *sl)
+lock_release_exclusive (struct shared_lock *sl)
 {
-  lock_acquire (sl->slock);
+  lock_acquire (&sl->slock);
   sl->i = 0;
-  cond_broadcast (&sl->cond);
+  cond_broadcast (&sl->cond, &sl->slock);
   lock_release (&sl->slock);
 }
diff --git a/pintos/src/threads/synch.h b/pintos/src/threads/synch.h
index 6f6bc4f..f59a682 100644
--- a/pintos/src/threads/synch.h
+++ b/pintos/src/threads/synch.h
@@ -53,11 +53,11 @@ struct shared_lock
     struct condition cond; /* Condition variable to share. */
   };
 
-void slock_init (shared_lock *sl);
-void lock_acquire_exclusive (shared_lock *sl);
-void lock_acquire_shared (shared_lock *sl);
-void lock_release_shared (shared_lock *sl);
-void lock_release_exclusive (shared_lock *sl);
+void slock_init (struct shared_lock *sl);
+void lock_acquire_exclusive (struct shared_lock *sl);
+void lock_acquire_shared (struct shared_lock *sl);
+void lock_release_shared (struct shared_lock *sl);
+void lock_release_exclusive (struct shared_lock *sl);
 
 /* Optimization barrier.
 
diff --git a/pintos/src/threads/thread.h b/pintos/src/threads/thread.h
index fb11600..060cb8a 100644
--- a/pintos/src/threads/thread.h
+++ b/pintos/src/threads/thread.h
@@ -150,15 +150,9 @@ struct thread
     uint32_t *pagedir;                  /* Page directory. */
     struct hash fd_hash;                /* Table of all file descriptors. */
 
-<<<<<<< HEAD
-    struct list children_in_r;          /* List of this thread's children threads. */
-    struct relation *parent_in_r;       /* Pointer to this thread's parent thread 
-                                           relationship resource. */
-=======
     struct lock children_lock;          /* Lock that every child uses to synchronize. */
     struct list children_in_r;          /* This thread is a parent to this relationship. */
     struct relation *parent_in_r;       /* This thread is a child to this relationship. */
->>>>>>> 163b3fb508b9671f40e31dd633f81709dc0e20a7
 
 #endif
 
diff --git a/pintos/src/userprog/.exception.c.swm b/pintos/src/userprog/.exception.c.swm
index e69de29..dc815ae 100644
Binary files a/pintos/src/userprog/.exception.c.swm and b/pintos/src/userprog/.exception.c.swm differ
diff --git a/pintos/src/userprog/.exception.h.swl b/pintos/src/userprog/.exception.h.swl
deleted file mode 100644
index e69de29..0000000
diff --git a/pintos/src/userprog/.exception.h.swm b/pintos/src/userprog/.exception.h.swm
deleted file mode 100644
index e69de29..0000000
diff --git a/pintos/src/userprog/.gdt.c.swl b/pintos/src/userprog/.gdt.c.swl
deleted file mode 100644
index e69de29..0000000
diff --git a/pintos/src/userprog/.gdt.c.swm b/pintos/src/userprog/.gdt.c.swm
deleted file mode 100644
index e69de29..0000000
diff --git a/pintos/src/userprog/.gdt.c.swn b/pintos/src/userprog/.gdt.c.swn
deleted file mode 100644
index e69de29..0000000
diff --git a/pintos/src/userprog/.gdt.h.swl b/pintos/src/userprog/.gdt.h.swl
deleted file mode 100644
index e69de29..0000000
diff --git a/pintos/src/userprog/.gdt.h.swm b/pintos/src/userprog/.gdt.h.swm
deleted file mode 100644
index e69de29..0000000
diff --git a/pintos/src/userprog/.gdt.h.swn b/pintos/src/userprog/.gdt.h.swn
deleted file mode 100644
index e69de29..0000000
diff --git a/pintos/src/userprog/.pagedir.h.swl b/pintos/src/userprog/.pagedir.h.swl
deleted file mode 100644
index e69de29..0000000
diff --git a/pintos/src/userprog/.pagedir.h.swm b/pintos/src/userprog/.pagedir.h.swm
deleted file mode 100644
index e69de29..0000000
diff --git a/pintos/src/userprog/.parse-example.c.swo b/pintos/src/userprog/.parse-example.c.swo
deleted file mode 100644
index e69de29..0000000
diff --git a/pintos/src/userprog/.parse-example.c.swp b/pintos/src/userprog/.parse-example.c.swp
deleted file mode 100644
index e69de29..0000000
diff --git a/pintos/src/userprog/.process.c.swf b/pintos/src/userprog/.process.c.swf
deleted file mode 100644
index d4defbb..0000000
Binary files a/pintos/src/userprog/.process.c.swf and /dev/null differ
diff --git a/pintos/src/userprog/.process.c.swg b/pintos/src/userprog/.process.c.swg
deleted file mode 100644
index e69de29..0000000
diff --git a/pintos/src/userprog/.process.c.swh b/pintos/src/userprog/.process.c.swh
deleted file mode 100644
index 60b01ad..0000000
Binary files a/pintos/src/userprog/.process.c.swh and /dev/null differ
diff --git a/pintos/src/userprog/.process.c.swi b/pintos/src/userprog/.process.c.swi
index d188423..e69de29 100644
Binary files a/pintos/src/userprog/.process.c.swi and b/pintos/src/userprog/.process.c.swi differ
diff --git a/pintos/src/userprog/.process.c.swl b/pintos/src/userprog/.process.c.swl
index e69de29..a726ec0 100644
Binary files a/pintos/src/userprog/.process.c.swl and b/pintos/src/userprog/.process.c.swl differ
diff --git a/pintos/src/userprog/.process.h.swi b/pintos/src/userprog/.process.h.swi
deleted file mode 100644
index a9a91c1..0000000
Binary files a/pintos/src/userprog/.process.h.swi and /dev/null differ
diff --git a/pintos/src/userprog/.process.h.swj b/pintos/src/userprog/.process.h.swj
deleted file mode 100644
index e69de29..0000000
diff --git a/pintos/src/userprog/.syscall.c.swe b/pintos/src/userprog/.syscall.c.swe
index d4ae5d7..ce0efb4 100644
Binary files a/pintos/src/userprog/.syscall.c.swe and b/pintos/src/userprog/.syscall.c.swe differ
diff --git a/pintos/src/userprog/.syscall.c.swf b/pintos/src/userprog/.syscall.c.swf
index e69de29..9fe3a3e 100644
Binary files a/pintos/src/userprog/.syscall.c.swf and b/pintos/src/userprog/.syscall.c.swf differ
diff --git a/pintos/src/userprog/.syscall.c.swg b/pintos/src/userprog/.syscall.c.swg
index e69de29..9c695bd 100644
Binary files a/pintos/src/userprog/.syscall.c.swg and b/pintos/src/userprog/.syscall.c.swg differ
diff --git a/pintos/src/userprog/.syscall.c.swh b/pintos/src/userprog/.syscall.c.swh
index e69de29..ff34c4f 100644
Binary files a/pintos/src/userprog/.syscall.c.swh and b/pintos/src/userprog/.syscall.c.swh differ
diff --git a/pintos/src/userprog/.syscall.c.swi b/pintos/src/userprog/.syscall.c.swi
index e69de29..06f6226 100644
Binary files a/pintos/src/userprog/.syscall.c.swi and b/pintos/src/userprog/.syscall.c.swi differ
diff --git a/pintos/src/userprog/.syscall.c.swj b/pintos/src/userprog/.syscall.c.swj
index 0a8fa48..e69de29 100644
Binary files a/pintos/src/userprog/.syscall.c.swj and b/pintos/src/userprog/.syscall.c.swj differ
diff --git a/pintos/src/userprog/.syscall.c.swl b/pintos/src/userprog/.syscall.c.swl
index f0d7cff..e69de29 100644
Binary files a/pintos/src/userprog/.syscall.c.swl and b/pintos/src/userprog/.syscall.c.swl differ
diff --git a/pintos/src/userprog/.syscall.h.swl b/pintos/src/userprog/.syscall.h.swl
deleted file mode 100644
index 4150c26..0000000
Binary files a/pintos/src/userprog/.syscall.h.swl and /dev/null differ
diff --git a/pintos/src/userprog/.syscall.h.swm b/pintos/src/userprog/.syscall.h.swm
deleted file mode 100644
index e69de29..0000000
diff --git a/pintos/src/userprog/.syscall.h.swo b/pintos/src/userprog/.syscall.h.swo
index 9fb827a..e69de29 100644
Binary files a/pintos/src/userprog/.syscall.h.swo and b/pintos/src/userprog/.syscall.h.swo differ
diff --git a/pintos/src/userprog/.tss.c.swl b/pintos/src/userprog/.tss.c.swl
deleted file mode 100644
index e69de29..0000000
diff --git a/pintos/src/userprog/.tss.c.swm b/pintos/src/userprog/.tss.c.swm
deleted file mode 100644
index e69de29..0000000
diff --git a/pintos/src/userprog/.tss.c.swn b/pintos/src/userprog/.tss.c.swn
deleted file mode 100644
index e69de29..0000000
diff --git a/pintos/src/userprog/bochsout.txt b/pintos/src/userprog/bochsout.txt
deleted file mode 100644
index 7faa850..0000000
--- a/pintos/src/userprog/bochsout.txt
+++ /dev/null
@@ -1,206 +0,0 @@
-00000000000i[     ] Bochs x86 Emulator 2.6.2
-00000000000i[     ]   Built from SVN snapshot on May 26, 2013
-00000000000i[     ] Compiled on Jan 10 2014 at 07:09:07
-00000000000i[     ] System configuration
-00000000000i[     ]   processors: 1 (cores=1, HT threads=1)
-00000000000i[     ]   A20 line support: yes
-00000000000i[     ] IPS is set to 1000000
-00000000000i[     ] CPU configuration
-00000000000i[     ]   SMP support: no
-00000000000i[     ]   level: 6
-00000000000i[     ]   APIC support: xapic
-00000000000i[     ]   FPU support: yes
-00000000000i[     ]   MMX support: yes
-00000000000i[     ]   3dnow! support: no
-00000000000i[     ]   SEP support: yes
-00000000000i[     ]   SSE support: sse2
-00000000000i[     ]   XSAVE support: no 
-00000000000i[     ]   AES support: no
-00000000000i[     ]   MOVBE support: no
-00000000000i[     ]   ADX support: no
-00000000000i[     ]   x86-64 support: no
-00000000000i[     ]   MWAIT support: yes
-00000000000i[     ] Optimization configuration
-00000000000i[     ]   RepeatSpeedups support: no
-00000000000i[     ]   Fast function calls: no
-00000000000i[     ]   Handlers Chaining speedups: no
-00000000000i[     ] Devices configuration
-00000000000i[     ]   NE2000 support: no
-00000000000i[     ]   PCI support: yes, enabled=yes
-00000000000i[     ]   SB16 support: no
-00000000000i[     ]   USB support: no
-00000000000i[     ]   VGA extension support: vbe
-00000000000i[MEM0 ] allocated memory at 0x7f737369a010. after alignment, vector=0x7f737369b000
-00000000000i[MEM0 ] 4.00MB
-00000000000i[MEM0 ] mem block size = 0x00100000, blocks=4
-00000000000i[MEM0 ] rom at 0xfffe0000/131072 ('/usr/class/cs140/x86_64/share/bochs/BIOS-bochs-latest')
-00000000000i[     ] init_dev of 'pci' plugin device by virtual method
-00000000000i[DEV  ] i440FX PMC present at device 0, function 0
-00000000000i[     ] init_dev of 'pci2isa' plugin device by virtual method
-00000000000i[DEV  ] PIIX3 PCI-to-ISA bridge present at device 1, function 0
-00000000000i[     ] init_dev of 'cmos' plugin device by virtual method
-00000000000i[CMOS ] Using specified time for initial clock
-00000000000i[CMOS ] Setting initial clock to: Wed Dec 31 16:00:00 1969 (time0=0)
-00000000000i[     ] init_dev of 'dma' plugin device by virtual method
-00000000000i[DMA  ] channel 4 used by cascade
-00000000000i[     ] init_dev of 'pic' plugin device by virtual method
-00000000000i[     ] init_dev of 'pit' plugin device by virtual method
-00000000000i[     ] init_dev of 'floppy' plugin device by virtual method
-00000000000i[DMA  ] channel 2 used by Floppy Drive
-00000000000i[     ] init_dev of 'vga' plugin device by virtual method
-00000000000i[MEM0 ] Register memory access handlers: 0x0000000a0000 - 0x0000000bffff
-00000000000i[VGA  ] interval=200000
-00000000000i[MEM0 ] Register memory access handlers: 0x0000e0000000 - 0x0000e0ffffff
-00000000000i[BXVGA] VBE Bochs Display Extension Enabled
-00000000000i[MEM0 ] rom at 0xc0000/41472 ('/usr/class/cs140/x86_64/share/bochs/VGABIOS-lgpl-latest')
-00000000000i[     ] init_dev of 'acpi' plugin device by virtual method
-00000000000i[DEV  ] ACPI Controller present at device 1, function 3
-00000000000i[     ] init_dev of 'ioapic' plugin device by virtual method
-00000000000i[IOAP ] initializing I/O APIC
-00000000000i[MEM0 ] Register memory access handlers: 0x0000fec00000 - 0x0000fec00fff
-00000000000i[IOAP ] IOAPIC enabled (base address = 0xfec00000)
-00000000000i[     ] init_dev of 'keyboard' plugin device by virtual method
-00000000000i[KBD  ] will paste characters every 400 keyboard ticks
-00000000000i[     ] init_dev of 'harddrv' plugin device by virtual method
-00000000000i[HD   ] HD on ata0-0: '/tmp/zUupVo0dKg.dsk', 'flat' mode
-00000000000i[IMG  ] hd_size: 2580480
-00000000000i[HD   ] ata0-0: using specified geometry: CHS=5/16/63
-00000000000i[HD   ] Using boot sequence disk, none, none
-00000000000i[HD   ] Floppy boot signature check is enabled
-00000000000i[     ] init_dev of 'pci_ide' plugin device by virtual method
-00000000000i[DEV  ] PIIX3 PCI IDE controller present at device 1, function 1
-00000000000i[     ] init_dev of 'unmapped' plugin device by virtual method
-00000000000i[     ] init_dev of 'biosdev' plugin device by virtual method
-00000000000i[     ] init_dev of 'speaker' plugin device by virtual method
-00000000000i[     ] init_dev of 'extfpuirq' plugin device by virtual method
-00000000000i[     ] init_dev of 'parallel' plugin device by virtual method
-00000000000i[PAR  ] parallel port 1 at 0x0378 irq 7
-00000000000i[     ] init_dev of 'serial' plugin device by virtual method
-00000000000i[SER  ] com1 at 0x03f8 irq 4
-00000000000i[     ] register state of 'pci' plugin device by virtual method
-00000000000i[     ] register state of 'pci2isa' plugin device by virtual method
-00000000000i[     ] register state of 'cmos' plugin device by virtual method
-00000000000i[     ] register state of 'dma' plugin device by virtual method
-00000000000i[     ] register state of 'pic' plugin device by virtual method
-00000000000i[     ] register state of 'pit' plugin device by virtual method
-00000000000i[     ] register state of 'floppy' plugin device by virtual method
-00000000000i[     ] register state of 'vga' plugin device by virtual method
-00000000000i[     ] register state of 'unmapped' plugin device by virtual method
-00000000000i[     ] register state of 'biosdev' plugin device by virtual method
-00000000000i[     ] register state of 'speaker' plugin device by virtual method
-00000000000i[     ] register state of 'extfpuirq' plugin device by virtual method
-00000000000i[     ] register state of 'parallel' plugin device by virtual method
-00000000000i[     ] register state of 'serial' plugin device by virtual method
-00000000000i[     ] register state of 'acpi' plugin device by virtual method
-00000000000i[     ] register state of 'ioapic' plugin device by virtual method
-00000000000i[     ] register state of 'keyboard' plugin device by virtual method
-00000000000i[     ] register state of 'harddrv' plugin device by virtual method
-00000000000i[     ] register state of 'pci_ide' plugin device by virtual method
-00000000000i[SYS  ] bx_pc_system_c::Reset(HARDWARE) called
-00000000000i[CPU0 ] cpu hardware reset
-00000000000i[APIC0] allocate APIC id=0 (MMIO enabled) to 0x0000fee00000
-00000000000i[CPU0 ] CPUID[0x00000000]: 00000005 756e6547 6c65746e 49656e69
-00000000000i[CPU0 ] CPUID[0x00000001]: 00000633 00010800 00000008 1fcbfbff
-00000000000i[CPU0 ] CPUID[0x00000002]: 00410601 00000000 00000000 00000000
-00000000000i[CPU0 ] CPUID[0x00000003]: 00000000 00000000 00000000 00000000
-00000000000i[CPU0 ] CPUID[0x00000004]: 00000000 00000000 00000000 00000000
-00000000000i[CPU0 ] CPUID[0x00000005]: 00000040 00000040 00000003 00000020
-00000000000i[CPU0 ] CPUID[0x80000000]: 80000008 00000000 00000000 00000000
-00000000000i[CPU0 ] CPUID[0x80000001]: 00000000 00000000 00000000 00000000
-00000000000i[CPU0 ] CPUID[0x80000002]: 20202020 20202020 20202020 6e492020
-00000000000i[CPU0 ] CPUID[0x80000003]: 286c6574 50202952 69746e65 52286d75
-00000000000i[CPU0 ] CPUID[0x80000004]: 20342029 20555043 20202020 00202020
-00000000000i[CPU0 ] CPUID[0x80000005]: 01ff01ff 01ff01ff 40020140 40020140
-00000000000i[CPU0 ] CPUID[0x80000006]: 00000000 42004200 02008140 00000000
-00000000000i[CPU0 ] CPUID[0x80000007]: 00000000 00000000 00000000 00000000
-00000000000i[CPU0 ] CPUID[0x80000008]: 00002028 00000000 00000000 00000000
-00000000000i[     ] reset of 'pci' plugin device by virtual method
-00000000000i[     ] reset of 'pci2isa' plugin device by virtual method
-00000000000i[     ] reset of 'cmos' plugin device by virtual method
-00000000000i[     ] reset of 'dma' plugin device by virtual method
-00000000000i[     ] reset of 'pic' plugin device by virtual method
-00000000000i[     ] reset of 'pit' plugin device by virtual method
-00000000000i[     ] reset of 'floppy' plugin device by virtual method
-00000000000i[     ] reset of 'vga' plugin device by virtual method
-00000000000i[     ] reset of 'acpi' plugin device by virtual method
-00000000000i[     ] reset of 'ioapic' plugin device by virtual method
-00000000000i[     ] reset of 'keyboard' plugin device by virtual method
-00000000000i[     ] reset of 'harddrv' plugin device by virtual method
-00000000000i[     ] reset of 'pci_ide' plugin device by virtual method
-00000000000i[     ] reset of 'unmapped' plugin device by virtual method
-00000000000i[     ] reset of 'biosdev' plugin device by virtual method
-00000000000i[     ] reset of 'speaker' plugin device by virtual method
-00000000000e[SPEAK] Failed to open /dev/console: Permission denied
-00000000000e[SPEAK] Deactivating beep on console
-00000000000i[     ] reset of 'extfpuirq' plugin device by virtual method
-00000000000i[     ] reset of 'parallel' plugin device by virtual method
-00000000000i[     ] reset of 'serial' plugin device by virtual method
-00000000025i[MEM0 ] allocate_block: block=0x0 used 0x1 of 0x4
-00000004661i[BIOS ] $Revision: 11545 $ $Date: 2012-11-11 09:11:17 +0100 (So, 11. Nov 2012) $
-00000317041i[KBD  ] reset-disable command received
-00000318331i[BIOS ] Starting rombios32
-00000318765i[BIOS ] Shutdown flag 0
-00000319348i[BIOS ] ram_size=0x00400000
-00000319746i[BIOS ] ram_end=4MB
-00000330593i[BIOS ] Found 1 cpu(s)
-00000344780i[BIOS ] bios_table_addr: 0x000fa448 end=0x000fcc00
-00000672577i[PCI  ] i440FX PMC write to PAM register 59 (TLB Flush)
-00001000504i[P2I  ] PCI IRQ routing: PIRQA# set to 0x0b
-00001000523i[P2I  ] PCI IRQ routing: PIRQB# set to 0x09
-00001000542i[P2I  ] PCI IRQ routing: PIRQC# set to 0x0b
-00001000561i[P2I  ] PCI IRQ routing: PIRQD# set to 0x09
-00001000571i[P2I  ] write: ELCR2 = 0x0a
-00001001337i[BIOS ] PIIX3/PIIX4 init: elcr=00 0a
-00001009010i[BIOS ] PCI: bus=0 devfn=0x00: vendor_id=0x8086 device_id=0x1237 class=0x0600
-00001011280i[BIOS ] PCI: bus=0 devfn=0x08: vendor_id=0x8086 device_id=0x7000 class=0x0601
-00001013389i[BIOS ] PCI: bus=0 devfn=0x09: vendor_id=0x8086 device_id=0x7010 class=0x0101
-00001013618i[PIDE ] new BM-DMA address: 0xc000
-00001014235i[BIOS ] region 4: 0x0000c000
-00001016260i[BIOS ] PCI: bus=0 devfn=0x0b: vendor_id=0x8086 device_id=0x7113 class=0x0680
-00001016493i[ACPI ] new irq line = 11
-00001016505i[ACPI ] new irq line = 9
-00001016534i[ACPI ] new PM base address: 0xb000
-00001016548i[ACPI ] new SM base address: 0xb100
-00001016576i[PCI  ] setting SMRAM control register to 0x4a
-00001180667i[CPU0 ] Enter to System Management Mode
-00001180678i[CPU0 ] RSM: Resuming from System Management Mode
-00001344696i[PCI  ] setting SMRAM control register to 0x0a
-00001359589i[BIOS ] MP table addr=0x000fa520 MPC table addr=0x000fa450 size=0xc8
-00001361345i[BIOS ] SMBIOS table addr=0x000fa530
-00001361403i[MEM0 ] allocate_block: block=0x3 used 0x2 of 0x4
-00001363527i[BIOS ] ACPI tables: RSDP addr=0x000fa650 ACPI DATA addr=0x003f0000 size=0xf72
-00001366701i[BIOS ] Firmware waking vector 0x3f00cc
-00001368499i[PCI  ] i440FX PMC write to PAM register 59 (TLB Flush)
-00001369227i[BIOS ] bios_table_cur_addr: 0x000fa674
-00001496844i[VBIOS] VGABios $Id: vgabios.c,v 1.75 2011/10/15 14:07:21 vruppert Exp $
-00001496915i[BXVGA] VBE known Display Interface b0c0
-00001496947i[BXVGA] VBE known Display Interface b0c5
-00001499872i[VBIOS] VBE Bios $Id: vbe.c,v 1.64 2011/07/19 18:25:05 vruppert Exp $
-00001839180i[BIOS ] ata0-0: PCHS=5/16/63 translation=none LCHS=5/16/63
-00005717573i[BIOS ] IDE time out
-00008761641i[BIOS ] Booting from 0000:7c00
-00009292951i[MEM0 ] allocate_block: block=0x1 used 0x3 of 0x4
-00009349409i[MEM0 ] allocate_block: block=0x2 used 0x4 of 0x4
-00016372489p[UNMP ] >>PANIC<< Shutdown port: shutdown requested
-00016372489i[CPU0 ] CPU is in protected mode (active)
-00016372489i[CPU0 ] CS.mode = 32 bit
-00016372489i[CPU0 ] SS.mode = 32 bit
-00016372489i[CPU0 ] EFER   = 0x00000000
-00016372489i[CPU0 ] | EAX=0000006e  EBX=000000d9  ECX=c00b8000  EDX=00008900
-00016372489i[CPU0 ] | ESP=c000ef80  EBP=c000ef88  ESI=000e01be  EDI=00000000
-00016372489i[CPU0 ] | IOPL=0 id vip vif ac vm rf nt of df IF tf SF zf af pf cf
-00016372489i[CPU0 ] | SEG sltr(index|ti|rpl)     base    limit G D
-00016372489i[CPU0 ] |  CS:0008( 0001| 0|  0) 00000000 ffffffff 1 1
-00016372489i[CPU0 ] |  DS:0010( 0002| 0|  0) 00000000 ffffffff 1 1
-00016372489i[CPU0 ] |  SS:0010( 0002| 0|  0) 00000000 ffffffff 1 1
-00016372489i[CPU0 ] |  ES:0010( 0002| 0|  0) 00000000 ffffffff 1 1
-00016372489i[CPU0 ] |  FS:0010( 0002| 0|  0) 00000000 ffffffff 1 1
-00016372489i[CPU0 ] |  GS:0010( 0002| 0|  0) 00000000 ffffffff 1 1
-00016372489i[CPU0 ] | EIP=c002806c (c002806b)
-00016372489i[CPU0 ] | CR0=0xe0010015 CR2=0x00000000
-00016372489i[CPU0 ] | CR3=0x00101000 CR4=0x00000000
-00016372489i[CPU0 ] 0xc002806b>> out dx, al : EE
-00016372489i[CMOS ] Last time is 16 (Wed Dec 31 16:00:16 1969)
-00016372489i[NGUI ] bx_nogui_gui_c::exit() not implemented yet.
-00016372489i[     ] restoring default signal behavior
-00016372489i[CTRL ] quit_sim called with exit code 1
diff --git a/pintos/src/userprog/bochsrc.txt b/pintos/src/userprog/bochsrc.txt
deleted file mode 100644
index a5b0751..0000000
--- a/pintos/src/userprog/bochsrc.txt
+++ /dev/null
@@ -1,12 +0,0 @@
-romimage: file=$BXSHARE/BIOS-bochs-latest
-vgaromimage: file=$BXSHARE/VGABIOS-lgpl-latest
-boot: disk
-cpu: ips=1000000
-megs: 4
-log: bochsout.txt
-panic: action=fatal
-user_shortcut: keys=ctrlaltdel
-clock: sync=none, time0=0
-ata0-master: type=disk, path=/tmp/zUupVo0dKg.dsk, mode=flat, cylinders=5, heads=16, spt=63, translation=none
-com1: enabled=1, mode=term, dev=/dev/stdout
-display_library: nogui
diff --git a/pintos/src/userprog/build/Makefile b/pintos/src/userprog/build/Makefile
deleted file mode 100644
index e997d27..0000000
--- a/pintos/src/userprog/build/Makefile
+++ /dev/null
@@ -1,109 +0,0 @@
-# -*- makefile -*-
-
-SRCDIR = ../..
-
-all: kernel.bin loader.bin
-
-include ../../Make.config
-include ../Make.vars
-include ../../tests/Make.tests
-
-# Compiler and assembler options.
-kernel.bin: CPPFLAGS += -I$(SRCDIR)/lib/kernel
-
-# Core kernel.
-threads_SRC  = threads/start.S		# Startup code.
-threads_SRC += threads/init.c		# Main program.
-threads_SRC += threads/thread.c		# Thread management core.
-threads_SRC += threads/switch.S		# Thread switch routine.
-threads_SRC += threads/interrupt.c	# Interrupt core.
-threads_SRC += threads/intr-stubs.S	# Interrupt stubs.
-threads_SRC += threads/synch.c		# Synchronization.
-threads_SRC += threads/palloc.c		# Page allocator.
-threads_SRC += threads/malloc.c		# Subpage allocator.
-
-# Device driver code.
-devices_SRC  = devices/pit.c		# Programmable interrupt timer chip.
-devices_SRC += devices/timer.c		# Periodic timer device.
-devices_SRC += devices/kbd.c		# Keyboard device.
-devices_SRC += devices/vga.c		# Video device.
-devices_SRC += devices/serial.c		# Serial port device.
-devices_SRC += devices/block.c		# Block device abstraction layer.
-devices_SRC += devices/partition.c	# Partition block device.
-devices_SRC += devices/ide.c		# IDE disk block device.
-devices_SRC += devices/input.c		# Serial and keyboard input.
-devices_SRC += devices/intq.c		# Interrupt queue.
-devices_SRC += devices/rtc.c		# Real-time clock.
-devices_SRC += devices/shutdown.c	# Reboot and power off.
-devices_SRC += devices/speaker.c	# PC speaker.
-
-# Library code shared between kernel and user programs.
-lib_SRC  = lib/debug.c			# Debug helpers.
-lib_SRC += lib/random.c			# Pseudo-random numbers.
-lib_SRC += lib/stdio.c			# I/O library.
-lib_SRC += lib/stdlib.c			# Utility functions.
-lib_SRC += lib/string.c			# String functions.
-lib_SRC += lib/arithmetic.c		# 64-bit arithmetic for GCC.
-lib_SRC += lib/ustar.c			# Unix standard tar format utilities.
-
-# Kernel-specific library code.
-lib/kernel_SRC  = lib/kernel/debug.c	# Debug helpers.
-lib/kernel_SRC += lib/kernel/list.c	# Doubly-linked lists.
-lib/kernel_SRC += lib/kernel/bitmap.c	# Bitmaps.
-lib/kernel_SRC += lib/kernel/hash.c	# Hash tables.
-lib/kernel_SRC += lib/kernel/console.c	# printf(), putchar().
-
-# User process code.
-userprog_SRC  = userprog/process.c	# Process loading.
-userprog_SRC += userprog/pagedir.c	# Page directories.
-userprog_SRC += userprog/exception.c	# User exception handler.
-userprog_SRC += userprog/syscall.c	# System call handler.
-userprog_SRC += userprog/gdt.c		# GDT initialization.
-userprog_SRC += userprog/tss.c		# TSS management.
-
-# No virtual memory code yet.
-#vm_SRC = vm/file.c			# Some file.
-
-# Filesystem code.
-filesys_SRC  = filesys/filesys.c	# Filesystem core.
-filesys_SRC += filesys/free-map.c	# Free sector bitmap.
-filesys_SRC += filesys/file.c		# Files.
-filesys_SRC += filesys/directory.c	# Directories.
-filesys_SRC += filesys/inode.c		# File headers.
-filesys_SRC += filesys/fsutil.c		# Utilities.
-
-SOURCES = $(foreach dir,$(KERNEL_SUBDIRS),$($(dir)_SRC))
-OBJECTS = $(patsubst %.c,%.o,$(patsubst %.S,%.o,$(SOURCES)))
-DEPENDS = $(patsubst %.o,%.d,$(OBJECTS))
-
-threads/kernel.lds.s: CPPFLAGS += -P
-threads/kernel.lds.s: threads/kernel.lds.S threads/loader.h
-
-kernel.o: threads/kernel.lds.s $(OBJECTS) 
-	$(LD) -T $< -o $@ $(OBJECTS)
-
-kernel.bin: kernel.o
-	$(OBJCOPY) -R .note -R .comment -S $< $@
-
-threads/loader.o: threads/loader.S
-	$(CC) -c $< -o $@ $(ASFLAGS) $(CPPFLAGS) $(DEFINES)
-
-loader.bin: threads/loader.o
-	$(LD) -N -e 0 -Ttext 0x7c00 --oformat binary -o $@ $<
-
-os.dsk: kernel.bin
-	cat $^ > $@
-
-clean::
-	rm -f $(OBJECTS) $(DEPENDS) 
-	rm -f threads/loader.o threads/kernel.lds.s threads/loader.d
-	rm -f kernel.bin.tmp
-	rm -f kernel.o kernel.lds.s
-	rm -f kernel.bin loader.bin
-	rm -f bochsout.txt bochsrc.txt
-	rm -f results grade
-
-Makefile: $(SRCDIR)/Makefile.build
-	cp $< $@
-
--include $(DEPENDS)
diff --git a/pintos/src/userprog/build/devices/block.d b/pintos/src/userprog/build/devices/block.d
deleted file mode 100644
index 7cd1de7..0000000
--- a/pintos/src/userprog/build/devices/block.d
+++ /dev/null
@@ -1,5 +0,0 @@
-devices/block.o: ../../devices/block.c ../../devices/block.h \
- ../../lib/stddef.h ../../lib/inttypes.h ../../lib/stdint.h \
- ../../lib/kernel/list.h ../../lib/stdbool.h ../../lib/string.h \
- ../../lib/stdio.h ../../lib/debug.h ../../lib/stdarg.h \
- ../../lib/kernel/stdio.h ../../devices/ide.h ../../threads/malloc.h
diff --git a/pintos/src/userprog/build/devices/block.o b/pintos/src/userprog/build/devices/block.o
deleted file mode 100644
index 601df10..0000000
Binary files a/pintos/src/userprog/build/devices/block.o and /dev/null differ
diff --git a/pintos/src/userprog/build/devices/ide.d b/pintos/src/userprog/build/devices/ide.d
deleted file mode 100644
index a8d8230..0000000
--- a/pintos/src/userprog/build/devices/ide.d
+++ /dev/null
@@ -1,7 +0,0 @@
-devices/ide.o: ../../devices/ide.c ../../devices/ide.h ../../lib/ctype.h \
- ../../lib/debug.h ../../lib/stdbool.h ../../lib/stdio.h \
- ../../lib/stdarg.h ../../lib/stddef.h ../../lib/stdint.h \
- ../../lib/kernel/stdio.h ../../devices/block.h ../../lib/inttypes.h \
- ../../devices/partition.h ../../devices/timer.h ../../lib/round.h \
- ../../threads/io.h ../../threads/interrupt.h ../../threads/synch.h \
- ../../lib/kernel/list.h
diff --git a/pintos/src/userprog/build/devices/ide.o b/pintos/src/userprog/build/devices/ide.o
deleted file mode 100644
index 8010895..0000000
Binary files a/pintos/src/userprog/build/devices/ide.o and /dev/null differ
diff --git a/pintos/src/userprog/build/devices/input.d b/pintos/src/userprog/build/devices/input.d
deleted file mode 100644
index d41b6a0..0000000
--- a/pintos/src/userprog/build/devices/input.d
+++ /dev/null
@@ -1,4 +0,0 @@
-devices/input.o: ../../devices/input.c ../../devices/input.h \
- ../../lib/stdbool.h ../../lib/stdint.h ../../lib/debug.h \
- ../../devices/intq.h ../../threads/interrupt.h ../../threads/synch.h \
- ../../lib/kernel/list.h ../../lib/stddef.h ../../devices/serial.h
diff --git a/pintos/src/userprog/build/devices/input.o b/pintos/src/userprog/build/devices/input.o
deleted file mode 100644
index 5beddb5..0000000
Binary files a/pintos/src/userprog/build/devices/input.o and /dev/null differ
diff --git a/pintos/src/userprog/build/devices/intq.d b/pintos/src/userprog/build/devices/intq.d
deleted file mode 100644
index 1ef0525..0000000
--- a/pintos/src/userprog/build/devices/intq.d
+++ /dev/null
@@ -1,6 +0,0 @@
-devices/intq.o: ../../devices/intq.c ../../devices/intq.h \
- ../../threads/interrupt.h ../../lib/stdbool.h ../../lib/stdint.h \
- ../../threads/synch.h ../../lib/kernel/list.h ../../lib/stddef.h \
- ../../lib/debug.h ../../threads/thread.h ../../threads/fixed-point.h \
- ../../threads/vaddr.h ../../threads/loader.h ../../lib/kernel/hash.h \
- ../../lib/kernel/list.h
diff --git a/pintos/src/userprog/build/devices/intq.o b/pintos/src/userprog/build/devices/intq.o
deleted file mode 100644
index b090afd..0000000
Binary files a/pintos/src/userprog/build/devices/intq.o and /dev/null differ
diff --git a/pintos/src/userprog/build/devices/kbd.d b/pintos/src/userprog/build/devices/kbd.d
deleted file mode 100644
index d361323..0000000
--- a/pintos/src/userprog/build/devices/kbd.d
+++ /dev/null
@@ -1,5 +0,0 @@
-devices/kbd.o: ../../devices/kbd.c ../../devices/kbd.h ../../lib/stdint.h \
- ../../lib/ctype.h ../../lib/debug.h ../../lib/stdio.h ../../lib/stdarg.h \
- ../../lib/stdbool.h ../../lib/stddef.h ../../lib/kernel/stdio.h \
- ../../lib/string.h ../../devices/input.h ../../devices/shutdown.h \
- ../../threads/interrupt.h ../../threads/io.h
diff --git a/pintos/src/userprog/build/devices/kbd.o b/pintos/src/userprog/build/devices/kbd.o
deleted file mode 100644
index 92f1243..0000000
Binary files a/pintos/src/userprog/build/devices/kbd.o and /dev/null differ
diff --git a/pintos/src/userprog/build/devices/partition.d b/pintos/src/userprog/build/devices/partition.d
deleted file mode 100644
index 88485ec..0000000
--- a/pintos/src/userprog/build/devices/partition.d
+++ /dev/null
@@ -1,6 +0,0 @@
-devices/partition.o: ../../devices/partition.c ../../devices/partition.h \
- ../../lib/packed.h ../../lib/stdlib.h ../../lib/stddef.h \
- ../../lib/string.h ../../lib/stdio.h ../../lib/debug.h \
- ../../lib/stdarg.h ../../lib/stdbool.h ../../lib/stdint.h \
- ../../lib/kernel/stdio.h ../../devices/block.h ../../lib/inttypes.h \
- ../../threads/malloc.h
diff --git a/pintos/src/userprog/build/devices/partition.o b/pintos/src/userprog/build/devices/partition.o
deleted file mode 100644
index 701817f..0000000
Binary files a/pintos/src/userprog/build/devices/partition.o and /dev/null differ
diff --git a/pintos/src/userprog/build/devices/pit.d b/pintos/src/userprog/build/devices/pit.d
deleted file mode 100644
index 5ff7bd9..0000000
--- a/pintos/src/userprog/build/devices/pit.d
+++ /dev/null
@@ -1,3 +0,0 @@
-devices/pit.o: ../../devices/pit.c ../../devices/pit.h ../../lib/stdint.h \
- ../../lib/debug.h ../../threads/interrupt.h ../../lib/stdbool.h \
- ../../threads/io.h ../../lib/stddef.h
diff --git a/pintos/src/userprog/build/devices/pit.o b/pintos/src/userprog/build/devices/pit.o
deleted file mode 100644
index f427d8c..0000000
Binary files a/pintos/src/userprog/build/devices/pit.o and /dev/null differ
diff --git a/pintos/src/userprog/build/devices/rtc.d b/pintos/src/userprog/build/devices/rtc.d
deleted file mode 100644
index a2ca386..0000000
--- a/pintos/src/userprog/build/devices/rtc.d
+++ /dev/null
@@ -1,4 +0,0 @@
-devices/rtc.o: ../../devices/rtc.c ../../devices/rtc.h ../../lib/stdio.h \
- ../../lib/debug.h ../../lib/stdarg.h ../../lib/stdbool.h \
- ../../lib/stddef.h ../../lib/stdint.h ../../lib/kernel/stdio.h \
- ../../threads/io.h
diff --git a/pintos/src/userprog/build/devices/rtc.o b/pintos/src/userprog/build/devices/rtc.o
deleted file mode 100644
index e135c61..0000000
Binary files a/pintos/src/userprog/build/devices/rtc.o and /dev/null differ
diff --git a/pintos/src/userprog/build/devices/serial.d b/pintos/src/userprog/build/devices/serial.d
deleted file mode 100644
index f2e0e2d..0000000
--- a/pintos/src/userprog/build/devices/serial.d
+++ /dev/null
@@ -1,7 +0,0 @@
-devices/serial.o: ../../devices/serial.c ../../devices/serial.h \
- ../../lib/stdint.h ../../lib/debug.h ../../devices/input.h \
- ../../lib/stdbool.h ../../devices/intq.h ../../threads/interrupt.h \
- ../../threads/synch.h ../../lib/kernel/list.h ../../lib/stddef.h \
- ../../devices/timer.h ../../lib/round.h ../../threads/io.h \
- ../../threads/thread.h ../../threads/fixed-point.h ../../threads/vaddr.h \
- ../../threads/loader.h ../../lib/kernel/hash.h ../../lib/kernel/list.h
diff --git a/pintos/src/userprog/build/devices/serial.o b/pintos/src/userprog/build/devices/serial.o
deleted file mode 100644
index 89ca2e9..0000000
Binary files a/pintos/src/userprog/build/devices/serial.o and /dev/null differ
diff --git a/pintos/src/userprog/build/devices/shutdown.d b/pintos/src/userprog/build/devices/shutdown.d
deleted file mode 100644
index 2dd3e16..0000000
--- a/pintos/src/userprog/build/devices/shutdown.d
+++ /dev/null
@@ -1,10 +0,0 @@
-devices/shutdown.o: ../../devices/shutdown.c ../../devices/shutdown.h \
- ../../lib/debug.h ../../lib/kernel/console.h ../../lib/stdio.h \
- ../../lib/stdarg.h ../../lib/stdbool.h ../../lib/stddef.h \
- ../../lib/stdint.h ../../lib/kernel/stdio.h ../../devices/kbd.h \
- ../../devices/serial.h ../../devices/timer.h ../../lib/round.h \
- ../../threads/io.h ../../threads/thread.h ../../lib/kernel/list.h \
- ../../threads/fixed-point.h ../../threads/vaddr.h ../../threads/loader.h \
- ../../lib/kernel/hash.h ../../lib/kernel/list.h ../../threads/synch.h \
- ../../userprog/exception.h ../../devices/block.h ../../lib/inttypes.h \
- ../../filesys/filesys.h ../../filesys/off_t.h
diff --git a/pintos/src/userprog/build/devices/shutdown.o b/pintos/src/userprog/build/devices/shutdown.o
deleted file mode 100644
index 11589df..0000000
Binary files a/pintos/src/userprog/build/devices/shutdown.o and /dev/null differ
diff --git a/pintos/src/userprog/build/devices/speaker.d b/pintos/src/userprog/build/devices/speaker.d
deleted file mode 100644
index 7552050..0000000
--- a/pintos/src/userprog/build/devices/speaker.d
+++ /dev/null
@@ -1,4 +0,0 @@
-devices/speaker.o: ../../devices/speaker.c ../../devices/speaker.h \
- ../../devices/pit.h ../../lib/stdint.h ../../threads/io.h \
- ../../lib/stddef.h ../../threads/interrupt.h ../../lib/stdbool.h \
- ../../devices/timer.h ../../lib/round.h
diff --git a/pintos/src/userprog/build/devices/speaker.o b/pintos/src/userprog/build/devices/speaker.o
deleted file mode 100644
index ac671e0..0000000
Binary files a/pintos/src/userprog/build/devices/speaker.o and /dev/null differ
diff --git a/pintos/src/userprog/build/devices/timer.d b/pintos/src/userprog/build/devices/timer.d
deleted file mode 100644
index fb96593..0000000
--- a/pintos/src/userprog/build/devices/timer.d
+++ /dev/null
@@ -1,8 +0,0 @@
-devices/timer.o: ../../devices/timer.c ../../devices/timer.h \
- ../../lib/round.h ../../lib/stdint.h ../../lib/debug.h \
- ../../lib/inttypes.h ../../lib/stdio.h ../../lib/stdarg.h \
- ../../lib/stdbool.h ../../lib/stddef.h ../../lib/kernel/stdio.h \
- ../../devices/pit.h ../../threads/interrupt.h ../../threads/synch.h \
- ../../lib/kernel/list.h ../../threads/thread.h \
- ../../threads/fixed-point.h ../../threads/vaddr.h ../../threads/loader.h \
- ../../lib/kernel/hash.h ../../lib/kernel/list.h
diff --git a/pintos/src/userprog/build/devices/timer.o b/pintos/src/userprog/build/devices/timer.o
deleted file mode 100644
index a4e9f70..0000000
Binary files a/pintos/src/userprog/build/devices/timer.o and /dev/null differ
diff --git a/pintos/src/userprog/build/devices/vga.d b/pintos/src/userprog/build/devices/vga.d
deleted file mode 100644
index 277ca37..0000000
--- a/pintos/src/userprog/build/devices/vga.d
+++ /dev/null
@@ -1,5 +0,0 @@
-devices/vga.o: ../../devices/vga.c ../../devices/vga.h ../../lib/round.h \
- ../../lib/stdint.h ../../lib/stddef.h ../../lib/string.h \
- ../../devices/speaker.h ../../threads/io.h ../../threads/interrupt.h \
- ../../lib/stdbool.h ../../threads/vaddr.h ../../lib/debug.h \
- ../../threads/loader.h
diff --git a/pintos/src/userprog/build/devices/vga.o b/pintos/src/userprog/build/devices/vga.o
deleted file mode 100644
index f925c72..0000000
Binary files a/pintos/src/userprog/build/devices/vga.o and /dev/null differ
diff --git a/pintos/src/userprog/build/filesys/directory.d b/pintos/src/userprog/build/filesys/directory.d
deleted file mode 100644
index 9102e0e..0000000
--- a/pintos/src/userprog/build/filesys/directory.d
+++ /dev/null
@@ -1,6 +0,0 @@
-filesys/directory.o: ../../filesys/directory.c ../../filesys/directory.h \
- ../../lib/stdbool.h ../../lib/stddef.h ../../devices/block.h \
- ../../lib/inttypes.h ../../lib/stdint.h ../../lib/stdio.h \
- ../../lib/debug.h ../../lib/stdarg.h ../../lib/kernel/stdio.h \
- ../../lib/string.h ../../lib/kernel/list.h ../../filesys/filesys.h \
- ../../filesys/off_t.h ../../filesys/inode.h ../../threads/malloc.h
diff --git a/pintos/src/userprog/build/filesys/directory.o b/pintos/src/userprog/build/filesys/directory.o
deleted file mode 100644
index 5321ff2..0000000
Binary files a/pintos/src/userprog/build/filesys/directory.o and /dev/null differ
diff --git a/pintos/src/userprog/build/filesys/file.d b/pintos/src/userprog/build/filesys/file.d
deleted file mode 100644
index de0147e..0000000
--- a/pintos/src/userprog/build/filesys/file.d
+++ /dev/null
@@ -1,4 +0,0 @@
-filesys/file.o: ../../filesys/file.c ../../filesys/file.h \
- ../../filesys/off_t.h ../../lib/stdint.h ../../lib/debug.h \
- ../../filesys/inode.h ../../lib/stdbool.h ../../devices/block.h \
- ../../lib/stddef.h ../../lib/inttypes.h ../../threads/malloc.h
diff --git a/pintos/src/userprog/build/filesys/file.o b/pintos/src/userprog/build/filesys/file.o
deleted file mode 100644
index 19e5d67..0000000
Binary files a/pintos/src/userprog/build/filesys/file.o and /dev/null differ
diff --git a/pintos/src/userprog/build/filesys/filesys.d b/pintos/src/userprog/build/filesys/filesys.d
deleted file mode 100644
index 2edf23c..0000000
--- a/pintos/src/userprog/build/filesys/filesys.d
+++ /dev/null
@@ -1,6 +0,0 @@
-filesys/filesys.o: ../../filesys/filesys.c ../../filesys/filesys.h \
- ../../lib/stdbool.h ../../filesys/off_t.h ../../lib/stdint.h \
- ../../lib/debug.h ../../lib/stdio.h ../../lib/stdarg.h \
- ../../lib/stddef.h ../../lib/kernel/stdio.h ../../lib/string.h \
- ../../filesys/file.h ../../filesys/free-map.h ../../devices/block.h \
- ../../lib/inttypes.h ../../filesys/inode.h ../../filesys/directory.h
diff --git a/pintos/src/userprog/build/filesys/filesys.o b/pintos/src/userprog/build/filesys/filesys.o
deleted file mode 100644
index 95ef9ff..0000000
Binary files a/pintos/src/userprog/build/filesys/filesys.o and /dev/null differ
diff --git a/pintos/src/userprog/build/filesys/free-map.d b/pintos/src/userprog/build/filesys/free-map.d
deleted file mode 100644
index 6b41f48..0000000
--- a/pintos/src/userprog/build/filesys/free-map.d
+++ /dev/null
@@ -1,5 +0,0 @@
-filesys/free-map.o: ../../filesys/free-map.c ../../filesys/free-map.h \
- ../../lib/stdbool.h ../../lib/stddef.h ../../devices/block.h \
- ../../lib/inttypes.h ../../lib/stdint.h ../../lib/kernel/bitmap.h \
- ../../lib/debug.h ../../filesys/file.h ../../filesys/off_t.h \
- ../../filesys/filesys.h ../../filesys/inode.h
diff --git a/pintos/src/userprog/build/filesys/free-map.o b/pintos/src/userprog/build/filesys/free-map.o
deleted file mode 100644
index 9a8e3b7..0000000
Binary files a/pintos/src/userprog/build/filesys/free-map.o and /dev/null differ
diff --git a/pintos/src/userprog/build/filesys/fsutil.d b/pintos/src/userprog/build/filesys/fsutil.d
deleted file mode 100644
index 70afa45..0000000
--- a/pintos/src/userprog/build/filesys/fsutil.d
+++ /dev/null
@@ -1,8 +0,0 @@
-filesys/fsutil.o: ../../filesys/fsutil.c ../../filesys/fsutil.h \
- ../../lib/debug.h ../../lib/stdio.h ../../lib/stdarg.h \
- ../../lib/stdbool.h ../../lib/stddef.h ../../lib/stdint.h \
- ../../lib/kernel/stdio.h ../../lib/stdlib.h ../../lib/string.h \
- ../../lib/ustar.h ../../filesys/directory.h ../../devices/block.h \
- ../../lib/inttypes.h ../../filesys/file.h ../../filesys/off_t.h \
- ../../filesys/filesys.h ../../threads/malloc.h ../../threads/palloc.h \
- ../../threads/vaddr.h ../../threads/loader.h
diff --git a/pintos/src/userprog/build/filesys/fsutil.o b/pintos/src/userprog/build/filesys/fsutil.o
deleted file mode 100644
index f8969ab..0000000
Binary files a/pintos/src/userprog/build/filesys/fsutil.o and /dev/null differ
diff --git a/pintos/src/userprog/build/filesys/inode.d b/pintos/src/userprog/build/filesys/inode.d
deleted file mode 100644
index 402ea56..0000000
--- a/pintos/src/userprog/build/filesys/inode.d
+++ /dev/null
@@ -1,6 +0,0 @@
-filesys/inode.o: ../../filesys/inode.c ../../filesys/inode.h \
- ../../lib/stdbool.h ../../filesys/off_t.h ../../lib/stdint.h \
- ../../devices/block.h ../../lib/stddef.h ../../lib/inttypes.h \
- ../../lib/kernel/list.h ../../lib/debug.h ../../lib/round.h \
- ../../lib/string.h ../../filesys/filesys.h ../../filesys/free-map.h \
- ../../threads/malloc.h
diff --git a/pintos/src/userprog/build/filesys/inode.o b/pintos/src/userprog/build/filesys/inode.o
deleted file mode 100644
index fdf9a00..0000000
Binary files a/pintos/src/userprog/build/filesys/inode.o and /dev/null differ
diff --git a/pintos/src/userprog/build/kernel.bin b/pintos/src/userprog/build/kernel.bin
deleted file mode 100755
index ae72109..0000000
Binary files a/pintos/src/userprog/build/kernel.bin and /dev/null differ
diff --git a/pintos/src/userprog/build/kernel.o b/pintos/src/userprog/build/kernel.o
deleted file mode 100755
index 3057143..0000000
Binary files a/pintos/src/userprog/build/kernel.o and /dev/null differ
diff --git a/pintos/src/userprog/build/lib/arithmetic.d b/pintos/src/userprog/build/lib/arithmetic.d
deleted file mode 100644
index 58fda34..0000000
--- a/pintos/src/userprog/build/lib/arithmetic.d
+++ /dev/null
@@ -1 +0,0 @@
-lib/arithmetic.o: ../../lib/arithmetic.c ../../lib/stdint.h
diff --git a/pintos/src/userprog/build/lib/arithmetic.o b/pintos/src/userprog/build/lib/arithmetic.o
deleted file mode 100644
index 8cb7689..0000000
Binary files a/pintos/src/userprog/build/lib/arithmetic.o and /dev/null differ
diff --git a/pintos/src/userprog/build/lib/debug.d b/pintos/src/userprog/build/lib/debug.d
deleted file mode 100644
index 4ec2a54..0000000
--- a/pintos/src/userprog/build/lib/debug.d
+++ /dev/null
@@ -1,3 +0,0 @@
-lib/debug.o: ../../lib/debug.c ../../lib/debug.h ../../lib/stdarg.h \
- ../../lib/stdbool.h ../../lib/stddef.h ../../lib/stdio.h \
- ../../lib/stdint.h ../../lib/kernel/stdio.h ../../lib/string.h
diff --git a/pintos/src/userprog/build/lib/debug.o b/pintos/src/userprog/build/lib/debug.o
deleted file mode 100644
index 0d26b3b..0000000
Binary files a/pintos/src/userprog/build/lib/debug.o and /dev/null differ
diff --git a/pintos/src/userprog/build/lib/kernel/bitmap.d b/pintos/src/userprog/build/lib/kernel/bitmap.d
deleted file mode 100644
index e79f065..0000000
--- a/pintos/src/userprog/build/lib/kernel/bitmap.d
+++ /dev/null
@@ -1,6 +0,0 @@
-lib/kernel/bitmap.o: ../../lib/kernel/bitmap.c ../../lib/kernel/bitmap.h \
- ../../lib/stdbool.h ../../lib/stddef.h ../../lib/inttypes.h \
- ../../lib/stdint.h ../../lib/debug.h ../../lib/limits.h \
- ../../lib/round.h ../../lib/stdio.h ../../lib/stdarg.h \
- ../../lib/kernel/stdio.h ../../threads/malloc.h ../../filesys/file.h \
- ../../filesys/off_t.h
diff --git a/pintos/src/userprog/build/lib/kernel/bitmap.o b/pintos/src/userprog/build/lib/kernel/bitmap.o
deleted file mode 100644
index 0d0719a..0000000
Binary files a/pintos/src/userprog/build/lib/kernel/bitmap.o and /dev/null differ
diff --git a/pintos/src/userprog/build/lib/kernel/console.d b/pintos/src/userprog/build/lib/kernel/console.d
deleted file mode 100644
index 1a17e3d..0000000
--- a/pintos/src/userprog/build/lib/kernel/console.d
+++ /dev/null
@@ -1,6 +0,0 @@
-lib/kernel/console.o: ../../lib/kernel/console.c \
- ../../lib/kernel/console.h ../../lib/stdarg.h ../../lib/stdio.h \
- ../../lib/debug.h ../../lib/stdbool.h ../../lib/stddef.h \
- ../../lib/stdint.h ../../lib/kernel/stdio.h ../../devices/serial.h \
- ../../devices/vga.h ../../threads/init.h ../../threads/interrupt.h \
- ../../threads/synch.h ../../lib/kernel/list.h
diff --git a/pintos/src/userprog/build/lib/kernel/console.o b/pintos/src/userprog/build/lib/kernel/console.o
deleted file mode 100644
index a592416..0000000
Binary files a/pintos/src/userprog/build/lib/kernel/console.o and /dev/null differ
diff --git a/pintos/src/userprog/build/lib/kernel/debug.d b/pintos/src/userprog/build/lib/kernel/debug.d
deleted file mode 100644
index beb77b8..0000000
--- a/pintos/src/userprog/build/lib/kernel/debug.d
+++ /dev/null
@@ -1,8 +0,0 @@
-lib/kernel/debug.o: ../../lib/kernel/debug.c ../../lib/debug.h \
- ../../lib/kernel/console.h ../../lib/stdarg.h ../../lib/stdbool.h \
- ../../lib/stddef.h ../../lib/stdio.h ../../lib/stdint.h \
- ../../lib/kernel/stdio.h ../../lib/string.h ../../threads/init.h \
- ../../threads/interrupt.h ../../threads/thread.h ../../lib/kernel/list.h \
- ../../threads/fixed-point.h ../../threads/vaddr.h ../../threads/loader.h \
- ../../lib/kernel/hash.h ../../lib/kernel/list.h ../../threads/synch.h \
- ../../threads/switch.h ../../devices/serial.h ../../devices/shutdown.h
diff --git a/pintos/src/userprog/build/lib/kernel/debug.o b/pintos/src/userprog/build/lib/kernel/debug.o
deleted file mode 100644
index c0ab6b4..0000000
Binary files a/pintos/src/userprog/build/lib/kernel/debug.o and /dev/null differ
diff --git a/pintos/src/userprog/build/lib/kernel/hash.d b/pintos/src/userprog/build/lib/kernel/hash.d
deleted file mode 100644
index 97856be..0000000
--- a/pintos/src/userprog/build/lib/kernel/hash.d
+++ /dev/null
@@ -1,4 +0,0 @@
-lib/kernel/hash.o: ../../lib/kernel/hash.c ../../lib/kernel/hash.h \
- ../../lib/stdbool.h ../../lib/stddef.h ../../lib/stdint.h \
- ../../lib/kernel/list.h ../../lib/kernel/../debug.h \
- ../../threads/malloc.h ../../lib/debug.h
diff --git a/pintos/src/userprog/build/lib/kernel/hash.o b/pintos/src/userprog/build/lib/kernel/hash.o
deleted file mode 100644
index e8fc5d7..0000000
Binary files a/pintos/src/userprog/build/lib/kernel/hash.o and /dev/null differ
diff --git a/pintos/src/userprog/build/lib/kernel/list.d b/pintos/src/userprog/build/lib/kernel/list.d
deleted file mode 100644
index f68f29e..0000000
--- a/pintos/src/userprog/build/lib/kernel/list.d
+++ /dev/null
@@ -1,3 +0,0 @@
-lib/kernel/list.o: ../../lib/kernel/list.c ../../lib/kernel/list.h \
- ../../lib/stdbool.h ../../lib/stddef.h ../../lib/stdint.h \
- ../../lib/kernel/../debug.h
diff --git a/pintos/src/userprog/build/lib/kernel/list.o b/pintos/src/userprog/build/lib/kernel/list.o
deleted file mode 100644
index f3443e6..0000000
Binary files a/pintos/src/userprog/build/lib/kernel/list.o and /dev/null differ
diff --git a/pintos/src/userprog/build/lib/random.d b/pintos/src/userprog/build/lib/random.d
deleted file mode 100644
index effd660..0000000
--- a/pintos/src/userprog/build/lib/random.d
+++ /dev/null
@@ -1,2 +0,0 @@
-lib/random.o: ../../lib/random.c ../../lib/random.h ../../lib/stddef.h \
- ../../lib/stdbool.h ../../lib/stdint.h ../../lib/debug.h
diff --git a/pintos/src/userprog/build/lib/random.o b/pintos/src/userprog/build/lib/random.o
deleted file mode 100644
index c1dc857..0000000
Binary files a/pintos/src/userprog/build/lib/random.o and /dev/null differ
diff --git a/pintos/src/userprog/build/lib/stdio.d b/pintos/src/userprog/build/lib/stdio.d
deleted file mode 100644
index 7aa4187..0000000
--- a/pintos/src/userprog/build/lib/stdio.d
+++ /dev/null
@@ -1,4 +0,0 @@
-lib/stdio.o: ../../lib/stdio.c ../../lib/stdio.h ../../lib/debug.h \
- ../../lib/stdarg.h ../../lib/stdbool.h ../../lib/stddef.h \
- ../../lib/stdint.h ../../lib/kernel/stdio.h ../../lib/ctype.h \
- ../../lib/inttypes.h ../../lib/round.h ../../lib/string.h
diff --git a/pintos/src/userprog/build/lib/stdio.o b/pintos/src/userprog/build/lib/stdio.o
deleted file mode 100644
index 2876c0c..0000000
Binary files a/pintos/src/userprog/build/lib/stdio.o and /dev/null differ
diff --git a/pintos/src/userprog/build/lib/stdlib.d b/pintos/src/userprog/build/lib/stdlib.d
deleted file mode 100644
index 30517a6..0000000
--- a/pintos/src/userprog/build/lib/stdlib.d
+++ /dev/null
@@ -1,3 +0,0 @@
-lib/stdlib.o: ../../lib/stdlib.c ../../lib/ctype.h ../../lib/debug.h \
- ../../lib/random.h ../../lib/stddef.h ../../lib/stdlib.h \
- ../../lib/stdbool.h
diff --git a/pintos/src/userprog/build/lib/stdlib.o b/pintos/src/userprog/build/lib/stdlib.o
deleted file mode 100644
index 4f6043c..0000000
Binary files a/pintos/src/userprog/build/lib/stdlib.o and /dev/null differ
diff --git a/pintos/src/userprog/build/lib/string.d b/pintos/src/userprog/build/lib/string.d
deleted file mode 100644
index f72771c..0000000
--- a/pintos/src/userprog/build/lib/string.d
+++ /dev/null
@@ -1,2 +0,0 @@
-lib/string.o: ../../lib/string.c ../../lib/string.h ../../lib/stddef.h \
- ../../lib/debug.h
diff --git a/pintos/src/userprog/build/lib/string.o b/pintos/src/userprog/build/lib/string.o
deleted file mode 100644
index fbae97e..0000000
Binary files a/pintos/src/userprog/build/lib/string.o and /dev/null differ
diff --git a/pintos/src/userprog/build/lib/user/console.d b/pintos/src/userprog/build/lib/user/console.d
deleted file mode 100644
index ac4e040..0000000
--- a/pintos/src/userprog/build/lib/user/console.d
+++ /dev/null
@@ -1,4 +0,0 @@
-lib/user/console.o: ../../lib/user/console.c ../../lib/stdio.h \
- ../../lib/debug.h ../../lib/stdarg.h ../../lib/stdbool.h \
- ../../lib/stddef.h ../../lib/stdint.h ../../lib/user/stdio.h \
- ../../lib/string.h ../../lib/user/syscall.h ../../lib/syscall-nr.h
diff --git a/pintos/src/userprog/build/lib/user/console.o b/pintos/src/userprog/build/lib/user/console.o
deleted file mode 100644
index 7917e21..0000000
Binary files a/pintos/src/userprog/build/lib/user/console.o and /dev/null differ
diff --git a/pintos/src/userprog/build/lib/user/debug.d b/pintos/src/userprog/build/lib/user/debug.d
deleted file mode 100644
index ccbdfb1..0000000
--- a/pintos/src/userprog/build/lib/user/debug.d
+++ /dev/null
@@ -1,4 +0,0 @@
-lib/user/debug.o: ../../lib/user/debug.c ../../lib/debug.h \
- ../../lib/stdarg.h ../../lib/stdbool.h ../../lib/stdio.h \
- ../../lib/stddef.h ../../lib/stdint.h ../../lib/user/stdio.h \
- ../../lib/user/syscall.h
diff --git a/pintos/src/userprog/build/lib/user/debug.o b/pintos/src/userprog/build/lib/user/debug.o
deleted file mode 100644
index 57f5bf4..0000000
Binary files a/pintos/src/userprog/build/lib/user/debug.o and /dev/null differ
diff --git a/pintos/src/userprog/build/lib/user/entry.d b/pintos/src/userprog/build/lib/user/entry.d
deleted file mode 100644
index 8e44e30..0000000
--- a/pintos/src/userprog/build/lib/user/entry.d
+++ /dev/null
@@ -1,2 +0,0 @@
-lib/user/entry.o: ../../lib/user/entry.c ../../lib/user/syscall.h \
- ../../lib/stdbool.h ../../lib/debug.h
diff --git a/pintos/src/userprog/build/lib/user/entry.o b/pintos/src/userprog/build/lib/user/entry.o
deleted file mode 100644
index 574be37..0000000
Binary files a/pintos/src/userprog/build/lib/user/entry.o and /dev/null differ
diff --git a/pintos/src/userprog/build/lib/user/syscall.d b/pintos/src/userprog/build/lib/user/syscall.d
deleted file mode 100644
index 0520aec..0000000
--- a/pintos/src/userprog/build/lib/user/syscall.d
+++ /dev/null
@@ -1,2 +0,0 @@
-lib/user/syscall.o: ../../lib/user/syscall.c ../../lib/user/syscall.h \
- ../../lib/stdbool.h ../../lib/debug.h ../../lib/user/../syscall-nr.h
diff --git a/pintos/src/userprog/build/lib/user/syscall.o b/pintos/src/userprog/build/lib/user/syscall.o
deleted file mode 100644
index af706c8..0000000
Binary files a/pintos/src/userprog/build/lib/user/syscall.o and /dev/null differ
diff --git a/pintos/src/userprog/build/lib/ustar.d b/pintos/src/userprog/build/lib/ustar.d
deleted file mode 100644
index ba59070..0000000
--- a/pintos/src/userprog/build/lib/ustar.d
+++ /dev/null
@@ -1,4 +0,0 @@
-lib/ustar.o: ../../lib/ustar.c ../../lib/ustar.h ../../lib/stdbool.h \
- ../../lib/limits.h ../../lib/packed.h ../../lib/stdio.h \
- ../../lib/debug.h ../../lib/stdarg.h ../../lib/stddef.h \
- ../../lib/stdint.h ../../lib/kernel/stdio.h ../../lib/string.h
diff --git a/pintos/src/userprog/build/lib/ustar.o b/pintos/src/userprog/build/lib/ustar.o
deleted file mode 100644
index f01fe45..0000000
Binary files a/pintos/src/userprog/build/lib/ustar.o and /dev/null differ
diff --git a/pintos/src/userprog/build/libc.a b/pintos/src/userprog/build/libc.a
deleted file mode 100644
index d5ed530..0000000
Binary files a/pintos/src/userprog/build/libc.a and /dev/null differ
diff --git a/pintos/src/userprog/build/loader.bin b/pintos/src/userprog/build/loader.bin
deleted file mode 100755
index 8b3ebf6..0000000
Binary files a/pintos/src/userprog/build/loader.bin and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/filesys/base/child-syn-read b/pintos/src/userprog/build/tests/filesys/base/child-syn-read
deleted file mode 100755
index df7081a..0000000
Binary files a/pintos/src/userprog/build/tests/filesys/base/child-syn-read and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/filesys/base/child-syn-read.d b/pintos/src/userprog/build/tests/filesys/base/child-syn-read.d
deleted file mode 100644
index e60fff3..0000000
--- a/pintos/src/userprog/build/tests/filesys/base/child-syn-read.d
+++ /dev/null
@@ -1,6 +0,0 @@
-tests/filesys/base/child-syn-read.o: \
- ../../tests/filesys/base/child-syn-read.c ../../lib/random.h \
- ../../lib/stddef.h ../../lib/stdio.h ../../lib/debug.h \
- ../../lib/stdarg.h ../../lib/stdbool.h ../../lib/stdint.h \
- ../../lib/user/stdio.h ../../lib/stdlib.h ../../lib/user/syscall.h \
- ../../tests/lib.h ../../tests/filesys/base/syn-read.h
diff --git a/pintos/src/userprog/build/tests/filesys/base/child-syn-read.o b/pintos/src/userprog/build/tests/filesys/base/child-syn-read.o
deleted file mode 100644
index 785e42c..0000000
Binary files a/pintos/src/userprog/build/tests/filesys/base/child-syn-read.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/filesys/base/child-syn-wrt b/pintos/src/userprog/build/tests/filesys/base/child-syn-wrt
deleted file mode 100755
index 4be3da2..0000000
Binary files a/pintos/src/userprog/build/tests/filesys/base/child-syn-wrt and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/filesys/base/child-syn-wrt.d b/pintos/src/userprog/build/tests/filesys/base/child-syn-wrt.d
deleted file mode 100644
index aa6360d..0000000
--- a/pintos/src/userprog/build/tests/filesys/base/child-syn-wrt.d
+++ /dev/null
@@ -1,5 +0,0 @@
-tests/filesys/base/child-syn-wrt.o: \
- ../../tests/filesys/base/child-syn-wrt.c ../../lib/random.h \
- ../../lib/stddef.h ../../lib/stdlib.h ../../lib/user/syscall.h \
- ../../lib/stdbool.h ../../lib/debug.h ../../tests/lib.h \
- ../../tests/filesys/base/syn-write.h
diff --git a/pintos/src/userprog/build/tests/filesys/base/child-syn-wrt.o b/pintos/src/userprog/build/tests/filesys/base/child-syn-wrt.o
deleted file mode 100644
index e9092eb..0000000
Binary files a/pintos/src/userprog/build/tests/filesys/base/child-syn-wrt.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/filesys/base/lg-create b/pintos/src/userprog/build/tests/filesys/base/lg-create
deleted file mode 100755
index afdac6e..0000000
Binary files a/pintos/src/userprog/build/tests/filesys/base/lg-create and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/filesys/base/lg-create.d b/pintos/src/userprog/build/tests/filesys/base/lg-create.d
deleted file mode 100644
index 99a27a4..0000000
--- a/pintos/src/userprog/build/tests/filesys/base/lg-create.d
+++ /dev/null
@@ -1,4 +0,0 @@
-tests/filesys/base/lg-create.o: ../../tests/filesys/base/lg-create.c \
- ../../tests/filesys/create.inc ../../lib/user/syscall.h \
- ../../lib/stdbool.h ../../lib/debug.h ../../tests/lib.h \
- ../../lib/stddef.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/filesys/base/lg-create.o b/pintos/src/userprog/build/tests/filesys/base/lg-create.o
deleted file mode 100644
index 754d8b7..0000000
Binary files a/pintos/src/userprog/build/tests/filesys/base/lg-create.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/filesys/base/lg-full b/pintos/src/userprog/build/tests/filesys/base/lg-full
deleted file mode 100755
index 47a78f6..0000000
Binary files a/pintos/src/userprog/build/tests/filesys/base/lg-full and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/filesys/base/lg-full.d b/pintos/src/userprog/build/tests/filesys/base/lg-full.d
deleted file mode 100644
index fb7d787..0000000
--- a/pintos/src/userprog/build/tests/filesys/base/lg-full.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/filesys/base/lg-full.o: ../../tests/filesys/base/lg-full.c \
- ../../tests/filesys/base/full.inc ../../tests/filesys/seq-test.h \
- ../../lib/stddef.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/filesys/base/lg-full.o b/pintos/src/userprog/build/tests/filesys/base/lg-full.o
deleted file mode 100644
index 22dabbc..0000000
Binary files a/pintos/src/userprog/build/tests/filesys/base/lg-full.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/filesys/base/lg-random b/pintos/src/userprog/build/tests/filesys/base/lg-random
deleted file mode 100755
index 8f5d3d7..0000000
Binary files a/pintos/src/userprog/build/tests/filesys/base/lg-random and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/filesys/base/lg-random.d b/pintos/src/userprog/build/tests/filesys/base/lg-random.d
deleted file mode 100644
index fba44a6..0000000
--- a/pintos/src/userprog/build/tests/filesys/base/lg-random.d
+++ /dev/null
@@ -1,6 +0,0 @@
-tests/filesys/base/lg-random.o: ../../tests/filesys/base/lg-random.c \
- ../../tests/filesys/base/random.inc ../../lib/random.h \
- ../../lib/stddef.h ../../lib/stdio.h ../../lib/debug.h \
- ../../lib/stdarg.h ../../lib/stdbool.h ../../lib/stdint.h \
- ../../lib/user/stdio.h ../../lib/string.h ../../lib/user/syscall.h \
- ../../tests/lib.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/filesys/base/lg-random.o b/pintos/src/userprog/build/tests/filesys/base/lg-random.o
deleted file mode 100644
index 1c43b6f..0000000
Binary files a/pintos/src/userprog/build/tests/filesys/base/lg-random.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/filesys/base/lg-seq-block b/pintos/src/userprog/build/tests/filesys/base/lg-seq-block
deleted file mode 100755
index 9c75101..0000000
Binary files a/pintos/src/userprog/build/tests/filesys/base/lg-seq-block and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/filesys/base/lg-seq-block.d b/pintos/src/userprog/build/tests/filesys/base/lg-seq-block.d
deleted file mode 100644
index 0e2224d..0000000
--- a/pintos/src/userprog/build/tests/filesys/base/lg-seq-block.d
+++ /dev/null
@@ -1,4 +0,0 @@
-tests/filesys/base/lg-seq-block.o: \
- ../../tests/filesys/base/lg-seq-block.c \
- ../../tests/filesys/base/seq-block.inc ../../tests/filesys/seq-test.h \
- ../../lib/stddef.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/filesys/base/lg-seq-block.o b/pintos/src/userprog/build/tests/filesys/base/lg-seq-block.o
deleted file mode 100644
index 88057c5..0000000
Binary files a/pintos/src/userprog/build/tests/filesys/base/lg-seq-block.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/filesys/base/lg-seq-random b/pintos/src/userprog/build/tests/filesys/base/lg-seq-random
deleted file mode 100755
index 7c3fb11..0000000
Binary files a/pintos/src/userprog/build/tests/filesys/base/lg-seq-random and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/filesys/base/lg-seq-random.d b/pintos/src/userprog/build/tests/filesys/base/lg-seq-random.d
deleted file mode 100644
index 849d7d6..0000000
--- a/pintos/src/userprog/build/tests/filesys/base/lg-seq-random.d
+++ /dev/null
@@ -1,4 +0,0 @@
-tests/filesys/base/lg-seq-random.o: \
- ../../tests/filesys/base/lg-seq-random.c \
- ../../tests/filesys/base/seq-random.inc ../../lib/random.h \
- ../../lib/stddef.h ../../tests/filesys/seq-test.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/filesys/base/lg-seq-random.o b/pintos/src/userprog/build/tests/filesys/base/lg-seq-random.o
deleted file mode 100644
index e71fb5a..0000000
Binary files a/pintos/src/userprog/build/tests/filesys/base/lg-seq-random.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/filesys/base/sm-create b/pintos/src/userprog/build/tests/filesys/base/sm-create
deleted file mode 100755
index a09e5cc..0000000
Binary files a/pintos/src/userprog/build/tests/filesys/base/sm-create and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/filesys/base/sm-create.d b/pintos/src/userprog/build/tests/filesys/base/sm-create.d
deleted file mode 100644
index c24b844..0000000
--- a/pintos/src/userprog/build/tests/filesys/base/sm-create.d
+++ /dev/null
@@ -1,4 +0,0 @@
-tests/filesys/base/sm-create.o: ../../tests/filesys/base/sm-create.c \
- ../../tests/filesys/create.inc ../../lib/user/syscall.h \
- ../../lib/stdbool.h ../../lib/debug.h ../../tests/lib.h \
- ../../lib/stddef.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/filesys/base/sm-create.o b/pintos/src/userprog/build/tests/filesys/base/sm-create.o
deleted file mode 100644
index 6562365..0000000
Binary files a/pintos/src/userprog/build/tests/filesys/base/sm-create.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/filesys/base/sm-full b/pintos/src/userprog/build/tests/filesys/base/sm-full
deleted file mode 100755
index 207d1f4..0000000
Binary files a/pintos/src/userprog/build/tests/filesys/base/sm-full and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/filesys/base/sm-full.d b/pintos/src/userprog/build/tests/filesys/base/sm-full.d
deleted file mode 100644
index d591dbd..0000000
--- a/pintos/src/userprog/build/tests/filesys/base/sm-full.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/filesys/base/sm-full.o: ../../tests/filesys/base/sm-full.c \
- ../../tests/filesys/base/full.inc ../../tests/filesys/seq-test.h \
- ../../lib/stddef.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/filesys/base/sm-full.o b/pintos/src/userprog/build/tests/filesys/base/sm-full.o
deleted file mode 100644
index a871453..0000000
Binary files a/pintos/src/userprog/build/tests/filesys/base/sm-full.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/filesys/base/sm-random b/pintos/src/userprog/build/tests/filesys/base/sm-random
deleted file mode 100755
index a91a095..0000000
Binary files a/pintos/src/userprog/build/tests/filesys/base/sm-random and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/filesys/base/sm-random.d b/pintos/src/userprog/build/tests/filesys/base/sm-random.d
deleted file mode 100644
index 41e4974..0000000
--- a/pintos/src/userprog/build/tests/filesys/base/sm-random.d
+++ /dev/null
@@ -1,6 +0,0 @@
-tests/filesys/base/sm-random.o: ../../tests/filesys/base/sm-random.c \
- ../../tests/filesys/base/random.inc ../../lib/random.h \
- ../../lib/stddef.h ../../lib/stdio.h ../../lib/debug.h \
- ../../lib/stdarg.h ../../lib/stdbool.h ../../lib/stdint.h \
- ../../lib/user/stdio.h ../../lib/string.h ../../lib/user/syscall.h \
- ../../tests/lib.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/filesys/base/sm-random.o b/pintos/src/userprog/build/tests/filesys/base/sm-random.o
deleted file mode 100644
index b7dffb2..0000000
Binary files a/pintos/src/userprog/build/tests/filesys/base/sm-random.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/filesys/base/sm-seq-block b/pintos/src/userprog/build/tests/filesys/base/sm-seq-block
deleted file mode 100755
index 2f21ab6..0000000
Binary files a/pintos/src/userprog/build/tests/filesys/base/sm-seq-block and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/filesys/base/sm-seq-block.d b/pintos/src/userprog/build/tests/filesys/base/sm-seq-block.d
deleted file mode 100644
index c0332e1..0000000
--- a/pintos/src/userprog/build/tests/filesys/base/sm-seq-block.d
+++ /dev/null
@@ -1,4 +0,0 @@
-tests/filesys/base/sm-seq-block.o: \
- ../../tests/filesys/base/sm-seq-block.c \
- ../../tests/filesys/base/seq-block.inc ../../tests/filesys/seq-test.h \
- ../../lib/stddef.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/filesys/base/sm-seq-block.o b/pintos/src/userprog/build/tests/filesys/base/sm-seq-block.o
deleted file mode 100644
index 3a54854..0000000
Binary files a/pintos/src/userprog/build/tests/filesys/base/sm-seq-block.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/filesys/base/sm-seq-random b/pintos/src/userprog/build/tests/filesys/base/sm-seq-random
deleted file mode 100755
index 3ffd49c..0000000
Binary files a/pintos/src/userprog/build/tests/filesys/base/sm-seq-random and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/filesys/base/sm-seq-random.d b/pintos/src/userprog/build/tests/filesys/base/sm-seq-random.d
deleted file mode 100644
index 3f6d274..0000000
--- a/pintos/src/userprog/build/tests/filesys/base/sm-seq-random.d
+++ /dev/null
@@ -1,4 +0,0 @@
-tests/filesys/base/sm-seq-random.o: \
- ../../tests/filesys/base/sm-seq-random.c \
- ../../tests/filesys/base/seq-random.inc ../../lib/random.h \
- ../../lib/stddef.h ../../tests/filesys/seq-test.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/filesys/base/sm-seq-random.o b/pintos/src/userprog/build/tests/filesys/base/sm-seq-random.o
deleted file mode 100644
index 6288718..0000000
Binary files a/pintos/src/userprog/build/tests/filesys/base/sm-seq-random.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/filesys/base/syn-read b/pintos/src/userprog/build/tests/filesys/base/syn-read
deleted file mode 100755
index 41103d4..0000000
Binary files a/pintos/src/userprog/build/tests/filesys/base/syn-read and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/filesys/base/syn-read.d b/pintos/src/userprog/build/tests/filesys/base/syn-read.d
deleted file mode 100644
index 5daf09a..0000000
--- a/pintos/src/userprog/build/tests/filesys/base/syn-read.d
+++ /dev/null
@@ -1,5 +0,0 @@
-tests/filesys/base/syn-read.o: ../../tests/filesys/base/syn-read.c \
- ../../lib/random.h ../../lib/stddef.h ../../lib/stdio.h \
- ../../lib/debug.h ../../lib/stdarg.h ../../lib/stdbool.h \
- ../../lib/stdint.h ../../lib/user/stdio.h ../../lib/user/syscall.h \
- ../../tests/lib.h ../../tests/main.h ../../tests/filesys/base/syn-read.h
diff --git a/pintos/src/userprog/build/tests/filesys/base/syn-read.o b/pintos/src/userprog/build/tests/filesys/base/syn-read.o
deleted file mode 100644
index b5f045e..0000000
Binary files a/pintos/src/userprog/build/tests/filesys/base/syn-read.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/filesys/base/syn-remove b/pintos/src/userprog/build/tests/filesys/base/syn-remove
deleted file mode 100755
index a96706b..0000000
Binary files a/pintos/src/userprog/build/tests/filesys/base/syn-remove and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/filesys/base/syn-remove.d b/pintos/src/userprog/build/tests/filesys/base/syn-remove.d
deleted file mode 100644
index 16f3f35..0000000
--- a/pintos/src/userprog/build/tests/filesys/base/syn-remove.d
+++ /dev/null
@@ -1,4 +0,0 @@
-tests/filesys/base/syn-remove.o: ../../tests/filesys/base/syn-remove.c \
- ../../lib/random.h ../../lib/stddef.h ../../lib/string.h \
- ../../lib/user/syscall.h ../../lib/stdbool.h ../../lib/debug.h \
- ../../tests/lib.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/filesys/base/syn-remove.o b/pintos/src/userprog/build/tests/filesys/base/syn-remove.o
deleted file mode 100644
index 57ca80d..0000000
Binary files a/pintos/src/userprog/build/tests/filesys/base/syn-remove.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/filesys/base/syn-write b/pintos/src/userprog/build/tests/filesys/base/syn-write
deleted file mode 100755
index 86e8028..0000000
Binary files a/pintos/src/userprog/build/tests/filesys/base/syn-write and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/filesys/base/syn-write.d b/pintos/src/userprog/build/tests/filesys/base/syn-write.d
deleted file mode 100644
index 3e19d42..0000000
--- a/pintos/src/userprog/build/tests/filesys/base/syn-write.d
+++ /dev/null
@@ -1,6 +0,0 @@
-tests/filesys/base/syn-write.o: ../../tests/filesys/base/syn-write.c \
- ../../lib/random.h ../../lib/stddef.h ../../lib/stdio.h \
- ../../lib/debug.h ../../lib/stdarg.h ../../lib/stdbool.h \
- ../../lib/stdint.h ../../lib/user/stdio.h ../../lib/string.h \
- ../../lib/user/syscall.h ../../tests/filesys/base/syn-write.h \
- ../../tests/lib.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/filesys/base/syn-write.o b/pintos/src/userprog/build/tests/filesys/base/syn-write.o
deleted file mode 100644
index 9c0206f..0000000
Binary files a/pintos/src/userprog/build/tests/filesys/base/syn-write.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/filesys/seq-test.d b/pintos/src/userprog/build/tests/filesys/seq-test.d
deleted file mode 100644
index d12ea84..0000000
--- a/pintos/src/userprog/build/tests/filesys/seq-test.d
+++ /dev/null
@@ -1,4 +0,0 @@
-tests/filesys/seq-test.o: ../../tests/filesys/seq-test.c \
- ../../tests/filesys/seq-test.h ../../lib/stddef.h ../../lib/random.h \
- ../../lib/user/syscall.h ../../lib/stdbool.h ../../lib/debug.h \
- ../../tests/lib.h
diff --git a/pintos/src/userprog/build/tests/filesys/seq-test.o b/pintos/src/userprog/build/tests/filesys/seq-test.o
deleted file mode 100644
index 12e07a0..0000000
Binary files a/pintos/src/userprog/build/tests/filesys/seq-test.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/lib.d b/pintos/src/userprog/build/tests/lib.d
deleted file mode 100644
index 3c35ac2..0000000
--- a/pintos/src/userprog/build/tests/lib.d
+++ /dev/null
@@ -1,4 +0,0 @@
-tests/lib.o: ../../tests/lib.c ../../tests/lib.h ../../lib/debug.h \
- ../../lib/stdbool.h ../../lib/stddef.h ../../lib/user/syscall.h \
- ../../lib/random.h ../../lib/stdarg.h ../../lib/stdio.h \
- ../../lib/stdint.h ../../lib/user/stdio.h ../../lib/string.h
diff --git a/pintos/src/userprog/build/tests/lib.o b/pintos/src/userprog/build/tests/lib.o
deleted file mode 100644
index d2b842c..0000000
Binary files a/pintos/src/userprog/build/tests/lib.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/main.d b/pintos/src/userprog/build/tests/main.d
deleted file mode 100644
index d33bec2..0000000
--- a/pintos/src/userprog/build/tests/main.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/main.o: ../../tests/main.c ../../lib/random.h ../../lib/stddef.h \
- ../../tests/lib.h ../../lib/debug.h ../../lib/stdbool.h \
- ../../lib/user/syscall.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/main.o b/pintos/src/userprog/build/tests/main.o
deleted file mode 100644
index c15b20c..0000000
Binary files a/pintos/src/userprog/build/tests/main.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/args-dbl-space b/pintos/src/userprog/build/tests/userprog/args-dbl-space
deleted file mode 100755
index ec4772c..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/args-dbl-space and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/args-many b/pintos/src/userprog/build/tests/userprog/args-many
deleted file mode 100755
index ec4772c..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/args-many and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/args-multiple b/pintos/src/userprog/build/tests/userprog/args-multiple
deleted file mode 100755
index ec4772c..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/args-multiple and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/args-none b/pintos/src/userprog/build/tests/userprog/args-none
deleted file mode 100755
index ec4772c..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/args-none and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/args-single b/pintos/src/userprog/build/tests/userprog/args-single
deleted file mode 100755
index ec4772c..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/args-single and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/args.d b/pintos/src/userprog/build/tests/userprog/args.d
deleted file mode 100644
index 6af25be..0000000
--- a/pintos/src/userprog/build/tests/userprog/args.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/args.o: ../../tests/userprog/args.c ../../tests/lib.h \
- ../../lib/debug.h ../../lib/stdbool.h ../../lib/stddef.h \
- ../../lib/user/syscall.h
diff --git a/pintos/src/userprog/build/tests/userprog/args.o b/pintos/src/userprog/build/tests/userprog/args.o
deleted file mode 100644
index abccc96..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/args.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/bad-jump b/pintos/src/userprog/build/tests/userprog/bad-jump
deleted file mode 100755
index 861131e..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/bad-jump and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/bad-jump.d b/pintos/src/userprog/build/tests/userprog/bad-jump.d
deleted file mode 100644
index 2a2b401..0000000
--- a/pintos/src/userprog/build/tests/userprog/bad-jump.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/bad-jump.o: ../../tests/userprog/bad-jump.c \
- ../../tests/lib.h ../../lib/debug.h ../../lib/stdbool.h \
- ../../lib/stddef.h ../../lib/user/syscall.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/bad-jump.o b/pintos/src/userprog/build/tests/userprog/bad-jump.o
deleted file mode 100644
index 3de476c..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/bad-jump.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/bad-jump2 b/pintos/src/userprog/build/tests/userprog/bad-jump2
deleted file mode 100755
index 6942762..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/bad-jump2 and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/bad-jump2.d b/pintos/src/userprog/build/tests/userprog/bad-jump2.d
deleted file mode 100644
index 5a7f075..0000000
--- a/pintos/src/userprog/build/tests/userprog/bad-jump2.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/bad-jump2.o: ../../tests/userprog/bad-jump2.c \
- ../../tests/lib.h ../../lib/debug.h ../../lib/stdbool.h \
- ../../lib/stddef.h ../../lib/user/syscall.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/bad-jump2.o b/pintos/src/userprog/build/tests/userprog/bad-jump2.o
deleted file mode 100644
index 36f63c2..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/bad-jump2.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/bad-read b/pintos/src/userprog/build/tests/userprog/bad-read
deleted file mode 100755
index 84d6eb9..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/bad-read and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/bad-read.d b/pintos/src/userprog/build/tests/userprog/bad-read.d
deleted file mode 100644
index 0aee5c3..0000000
--- a/pintos/src/userprog/build/tests/userprog/bad-read.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/bad-read.o: ../../tests/userprog/bad-read.c \
- ../../tests/lib.h ../../lib/debug.h ../../lib/stdbool.h \
- ../../lib/stddef.h ../../lib/user/syscall.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/bad-read.o b/pintos/src/userprog/build/tests/userprog/bad-read.o
deleted file mode 100644
index 44b17e6..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/bad-read.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/bad-read2 b/pintos/src/userprog/build/tests/userprog/bad-read2
deleted file mode 100755
index 506dc82..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/bad-read2 and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/bad-read2.d b/pintos/src/userprog/build/tests/userprog/bad-read2.d
deleted file mode 100644
index f13072c..0000000
--- a/pintos/src/userprog/build/tests/userprog/bad-read2.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/bad-read2.o: ../../tests/userprog/bad-read2.c \
- ../../tests/lib.h ../../lib/debug.h ../../lib/stdbool.h \
- ../../lib/stddef.h ../../lib/user/syscall.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/bad-read2.o b/pintos/src/userprog/build/tests/userprog/bad-read2.o
deleted file mode 100644
index 664dabd..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/bad-read2.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/bad-write b/pintos/src/userprog/build/tests/userprog/bad-write
deleted file mode 100755
index 3c28df8..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/bad-write and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/bad-write.d b/pintos/src/userprog/build/tests/userprog/bad-write.d
deleted file mode 100644
index 3ca4a63..0000000
--- a/pintos/src/userprog/build/tests/userprog/bad-write.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/bad-write.o: ../../tests/userprog/bad-write.c \
- ../../tests/lib.h ../../lib/debug.h ../../lib/stdbool.h \
- ../../lib/stddef.h ../../lib/user/syscall.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/bad-write.o b/pintos/src/userprog/build/tests/userprog/bad-write.o
deleted file mode 100644
index af1aa03..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/bad-write.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/bad-write2 b/pintos/src/userprog/build/tests/userprog/bad-write2
deleted file mode 100755
index aad2747..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/bad-write2 and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/bad-write2.d b/pintos/src/userprog/build/tests/userprog/bad-write2.d
deleted file mode 100644
index 3160e03..0000000
--- a/pintos/src/userprog/build/tests/userprog/bad-write2.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/bad-write2.o: ../../tests/userprog/bad-write2.c \
- ../../tests/lib.h ../../lib/debug.h ../../lib/stdbool.h \
- ../../lib/stddef.h ../../lib/user/syscall.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/bad-write2.o b/pintos/src/userprog/build/tests/userprog/bad-write2.o
deleted file mode 100644
index f29554a..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/bad-write2.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/boundary.d b/pintos/src/userprog/build/tests/userprog/boundary.d
deleted file mode 100644
index 1d794d7..0000000
--- a/pintos/src/userprog/build/tests/userprog/boundary.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/boundary.o: ../../tests/userprog/boundary.c \
- ../../lib/inttypes.h ../../lib/stdint.h ../../lib/round.h \
- ../../lib/string.h ../../lib/stddef.h ../../tests/userprog/boundary.h
diff --git a/pintos/src/userprog/build/tests/userprog/boundary.o b/pintos/src/userprog/build/tests/userprog/boundary.o
deleted file mode 100644
index 4927a1d..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/boundary.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/child-args b/pintos/src/userprog/build/tests/userprog/child-args
deleted file mode 100755
index ec4772c..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/child-args and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/child-bad b/pintos/src/userprog/build/tests/userprog/child-bad
deleted file mode 100755
index 53aa943..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/child-bad and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/child-bad.d b/pintos/src/userprog/build/tests/userprog/child-bad.d
deleted file mode 100644
index bde37b8..0000000
--- a/pintos/src/userprog/build/tests/userprog/child-bad.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/child-bad.o: ../../tests/userprog/child-bad.c \
- ../../tests/lib.h ../../lib/debug.h ../../lib/stdbool.h \
- ../../lib/stddef.h ../../lib/user/syscall.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/child-bad.o b/pintos/src/userprog/build/tests/userprog/child-bad.o
deleted file mode 100644
index a69a109..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/child-bad.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/child-close b/pintos/src/userprog/build/tests/userprog/child-close
deleted file mode 100755
index 6740d92..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/child-close and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/child-close.d b/pintos/src/userprog/build/tests/userprog/child-close.d
deleted file mode 100644
index 1cb2777..0000000
--- a/pintos/src/userprog/build/tests/userprog/child-close.d
+++ /dev/null
@@ -1,5 +0,0 @@
-tests/userprog/child-close.o: ../../tests/userprog/child-close.c \
- ../../lib/ctype.h ../../lib/stdio.h ../../lib/debug.h ../../lib/stdarg.h \
- ../../lib/stdbool.h ../../lib/stddef.h ../../lib/stdint.h \
- ../../lib/user/stdio.h ../../lib/stdlib.h ../../lib/user/syscall.h \
- ../../tests/lib.h
diff --git a/pintos/src/userprog/build/tests/userprog/child-close.o b/pintos/src/userprog/build/tests/userprog/child-close.o
deleted file mode 100644
index b1a5200..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/child-close.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/child-rox b/pintos/src/userprog/build/tests/userprog/child-rox
deleted file mode 100755
index 302ee1e..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/child-rox and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/child-rox.d b/pintos/src/userprog/build/tests/userprog/child-rox.d
deleted file mode 100644
index d7542ee..0000000
--- a/pintos/src/userprog/build/tests/userprog/child-rox.d
+++ /dev/null
@@ -1,5 +0,0 @@
-tests/userprog/child-rox.o: ../../tests/userprog/child-rox.c \
- ../../lib/ctype.h ../../lib/stdio.h ../../lib/debug.h ../../lib/stdarg.h \
- ../../lib/stdbool.h ../../lib/stddef.h ../../lib/stdint.h \
- ../../lib/user/stdio.h ../../lib/stdlib.h ../../lib/user/syscall.h \
- ../../tests/lib.h
diff --git a/pintos/src/userprog/build/tests/userprog/child-rox.o b/pintos/src/userprog/build/tests/userprog/child-rox.o
deleted file mode 100644
index c738c0e..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/child-rox.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/child-simple b/pintos/src/userprog/build/tests/userprog/child-simple
deleted file mode 100755
index ca95314..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/child-simple and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/child-simple.d b/pintos/src/userprog/build/tests/userprog/child-simple.d
deleted file mode 100644
index a58c2b8..0000000
--- a/pintos/src/userprog/build/tests/userprog/child-simple.d
+++ /dev/null
@@ -1,4 +0,0 @@
-tests/userprog/child-simple.o: ../../tests/userprog/child-simple.c \
- ../../lib/stdio.h ../../lib/debug.h ../../lib/stdarg.h \
- ../../lib/stdbool.h ../../lib/stddef.h ../../lib/stdint.h \
- ../../lib/user/stdio.h ../../tests/lib.h ../../lib/user/syscall.h
diff --git a/pintos/src/userprog/build/tests/userprog/child-simple.o b/pintos/src/userprog/build/tests/userprog/child-simple.o
deleted file mode 100644
index b342e1f..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/child-simple.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/close-bad-fd b/pintos/src/userprog/build/tests/userprog/close-bad-fd
deleted file mode 100755
index f28336b..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/close-bad-fd and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/close-bad-fd.d b/pintos/src/userprog/build/tests/userprog/close-bad-fd.d
deleted file mode 100644
index 3543939..0000000
--- a/pintos/src/userprog/build/tests/userprog/close-bad-fd.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/close-bad-fd.o: ../../tests/userprog/close-bad-fd.c \
- ../../lib/user/syscall.h ../../lib/stdbool.h ../../lib/debug.h \
- ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/close-bad-fd.o b/pintos/src/userprog/build/tests/userprog/close-bad-fd.o
deleted file mode 100644
index cbd9588..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/close-bad-fd.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/close-normal b/pintos/src/userprog/build/tests/userprog/close-normal
deleted file mode 100755
index 8cf8bc1..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/close-normal and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/close-normal.d b/pintos/src/userprog/build/tests/userprog/close-normal.d
deleted file mode 100644
index 4c51ae9..0000000
--- a/pintos/src/userprog/build/tests/userprog/close-normal.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/close-normal.o: ../../tests/userprog/close-normal.c \
- ../../lib/user/syscall.h ../../lib/stdbool.h ../../lib/debug.h \
- ../../tests/lib.h ../../lib/stddef.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/close-normal.o b/pintos/src/userprog/build/tests/userprog/close-normal.o
deleted file mode 100644
index 0a2e82f..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/close-normal.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/close-stdin b/pintos/src/userprog/build/tests/userprog/close-stdin
deleted file mode 100755
index 9847bab..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/close-stdin and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/close-stdin.d b/pintos/src/userprog/build/tests/userprog/close-stdin.d
deleted file mode 100644
index f4394e4..0000000
--- a/pintos/src/userprog/build/tests/userprog/close-stdin.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/close-stdin.o: ../../tests/userprog/close-stdin.c \
- ../../lib/user/syscall.h ../../lib/stdbool.h ../../lib/debug.h \
- ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/close-stdin.o b/pintos/src/userprog/build/tests/userprog/close-stdin.o
deleted file mode 100644
index 81a6485..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/close-stdin.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/close-stdout b/pintos/src/userprog/build/tests/userprog/close-stdout
deleted file mode 100755
index 5ba868c..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/close-stdout and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/close-stdout.d b/pintos/src/userprog/build/tests/userprog/close-stdout.d
deleted file mode 100644
index 9daedf5..0000000
--- a/pintos/src/userprog/build/tests/userprog/close-stdout.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/close-stdout.o: ../../tests/userprog/close-stdout.c \
- ../../lib/user/syscall.h ../../lib/stdbool.h ../../lib/debug.h \
- ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/close-stdout.o b/pintos/src/userprog/build/tests/userprog/close-stdout.o
deleted file mode 100644
index 14e2d75..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/close-stdout.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/close-twice b/pintos/src/userprog/build/tests/userprog/close-twice
deleted file mode 100755
index 2987bbf..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/close-twice and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/close-twice.d b/pintos/src/userprog/build/tests/userprog/close-twice.d
deleted file mode 100644
index d6629a5..0000000
--- a/pintos/src/userprog/build/tests/userprog/close-twice.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/close-twice.o: ../../tests/userprog/close-twice.c \
- ../../lib/user/syscall.h ../../lib/stdbool.h ../../lib/debug.h \
- ../../tests/lib.h ../../lib/stddef.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/close-twice.o b/pintos/src/userprog/build/tests/userprog/close-twice.o
deleted file mode 100644
index 3b3475a..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/close-twice.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/create-bad-ptr b/pintos/src/userprog/build/tests/userprog/create-bad-ptr
deleted file mode 100755
index 362eb66..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/create-bad-ptr and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/create-bad-ptr.d b/pintos/src/userprog/build/tests/userprog/create-bad-ptr.d
deleted file mode 100644
index 12b27c6..0000000
--- a/pintos/src/userprog/build/tests/userprog/create-bad-ptr.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/create-bad-ptr.o: ../../tests/userprog/create-bad-ptr.c \
- ../../tests/lib.h ../../lib/debug.h ../../lib/stdbool.h \
- ../../lib/stddef.h ../../lib/user/syscall.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/create-bad-ptr.o b/pintos/src/userprog/build/tests/userprog/create-bad-ptr.o
deleted file mode 100644
index 116322a..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/create-bad-ptr.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/create-bound b/pintos/src/userprog/build/tests/userprog/create-bound
deleted file mode 100755
index 28c8018..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/create-bound and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/create-bound.d b/pintos/src/userprog/build/tests/userprog/create-bound.d
deleted file mode 100644
index f5fa3af..0000000
--- a/pintos/src/userprog/build/tests/userprog/create-bound.d
+++ /dev/null
@@ -1,4 +0,0 @@
-tests/userprog/create-bound.o: ../../tests/userprog/create-bound.c \
- ../../lib/user/syscall.h ../../lib/stdbool.h ../../lib/debug.h \
- ../../tests/userprog/boundary.h ../../tests/lib.h ../../lib/stddef.h \
- ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/create-bound.o b/pintos/src/userprog/build/tests/userprog/create-bound.o
deleted file mode 100644
index 6bf2171..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/create-bound.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/create-empty b/pintos/src/userprog/build/tests/userprog/create-empty
deleted file mode 100755
index 8011cd7..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/create-empty and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/create-empty.d b/pintos/src/userprog/build/tests/userprog/create-empty.d
deleted file mode 100644
index 1fceae3..0000000
--- a/pintos/src/userprog/build/tests/userprog/create-empty.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/create-empty.o: ../../tests/userprog/create-empty.c \
- ../../tests/lib.h ../../lib/debug.h ../../lib/stdbool.h \
- ../../lib/stddef.h ../../lib/user/syscall.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/create-empty.o b/pintos/src/userprog/build/tests/userprog/create-empty.o
deleted file mode 100644
index d106ea0..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/create-empty.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/create-exists b/pintos/src/userprog/build/tests/userprog/create-exists
deleted file mode 100755
index 75e4293..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/create-exists and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/create-exists.d b/pintos/src/userprog/build/tests/userprog/create-exists.d
deleted file mode 100644
index 9936da0..0000000
--- a/pintos/src/userprog/build/tests/userprog/create-exists.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/create-exists.o: ../../tests/userprog/create-exists.c \
- ../../lib/user/syscall.h ../../lib/stdbool.h ../../lib/debug.h \
- ../../tests/lib.h ../../lib/stddef.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/create-exists.o b/pintos/src/userprog/build/tests/userprog/create-exists.o
deleted file mode 100644
index 4da2fae..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/create-exists.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/create-long b/pintos/src/userprog/build/tests/userprog/create-long
deleted file mode 100755
index b7ffe7d..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/create-long and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/create-long.d b/pintos/src/userprog/build/tests/userprog/create-long.d
deleted file mode 100644
index fd3995f..0000000
--- a/pintos/src/userprog/build/tests/userprog/create-long.d
+++ /dev/null
@@ -1,4 +0,0 @@
-tests/userprog/create-long.o: ../../tests/userprog/create-long.c \
- ../../lib/string.h ../../lib/stddef.h ../../lib/user/syscall.h \
- ../../lib/stdbool.h ../../lib/debug.h ../../tests/lib.h \
- ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/create-long.o b/pintos/src/userprog/build/tests/userprog/create-long.o
deleted file mode 100644
index c3dfe42..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/create-long.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/create-normal b/pintos/src/userprog/build/tests/userprog/create-normal
deleted file mode 100755
index 07c3926..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/create-normal and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/create-normal.d b/pintos/src/userprog/build/tests/userprog/create-normal.d
deleted file mode 100644
index b5da7ee..0000000
--- a/pintos/src/userprog/build/tests/userprog/create-normal.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/create-normal.o: ../../tests/userprog/create-normal.c \
- ../../tests/lib.h ../../lib/debug.h ../../lib/stdbool.h \
- ../../lib/stddef.h ../../lib/user/syscall.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/create-normal.o b/pintos/src/userprog/build/tests/userprog/create-normal.o
deleted file mode 100644
index 2c7ac15..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/create-normal.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/create-null b/pintos/src/userprog/build/tests/userprog/create-null
deleted file mode 100755
index fa9f4e8..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/create-null and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/create-null.d b/pintos/src/userprog/build/tests/userprog/create-null.d
deleted file mode 100644
index ea17eb2..0000000
--- a/pintos/src/userprog/build/tests/userprog/create-null.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/create-null.o: ../../tests/userprog/create-null.c \
- ../../tests/lib.h ../../lib/debug.h ../../lib/stdbool.h \
- ../../lib/stddef.h ../../lib/user/syscall.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/create-null.o b/pintos/src/userprog/build/tests/userprog/create-null.o
deleted file mode 100644
index 447f355..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/create-null.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/exec-arg b/pintos/src/userprog/build/tests/userprog/exec-arg
deleted file mode 100755
index 51af796..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/exec-arg and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/exec-arg.d b/pintos/src/userprog/build/tests/userprog/exec-arg.d
deleted file mode 100644
index 066b0c3..0000000
--- a/pintos/src/userprog/build/tests/userprog/exec-arg.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/exec-arg.o: ../../tests/userprog/exec-arg.c \
- ../../lib/user/syscall.h ../../lib/stdbool.h ../../lib/debug.h \
- ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/exec-arg.o b/pintos/src/userprog/build/tests/userprog/exec-arg.o
deleted file mode 100644
index c8621f4..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/exec-arg.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/exec-bad-ptr b/pintos/src/userprog/build/tests/userprog/exec-bad-ptr
deleted file mode 100755
index e299fb4..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/exec-bad-ptr and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/exec-bad-ptr.d b/pintos/src/userprog/build/tests/userprog/exec-bad-ptr.d
deleted file mode 100644
index b1d7580..0000000
--- a/pintos/src/userprog/build/tests/userprog/exec-bad-ptr.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/exec-bad-ptr.o: ../../tests/userprog/exec-bad-ptr.c \
- ../../lib/user/syscall.h ../../lib/stdbool.h ../../lib/debug.h \
- ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/exec-bad-ptr.o b/pintos/src/userprog/build/tests/userprog/exec-bad-ptr.o
deleted file mode 100644
index 1623f88..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/exec-bad-ptr.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/exec-missing b/pintos/src/userprog/build/tests/userprog/exec-missing
deleted file mode 100755
index bf80a73..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/exec-missing and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/exec-missing.d b/pintos/src/userprog/build/tests/userprog/exec-missing.d
deleted file mode 100644
index 8b27532..0000000
--- a/pintos/src/userprog/build/tests/userprog/exec-missing.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/exec-missing.o: ../../tests/userprog/exec-missing.c \
- ../../lib/user/syscall.h ../../lib/stdbool.h ../../lib/debug.h \
- ../../tests/lib.h ../../lib/stddef.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/exec-missing.o b/pintos/src/userprog/build/tests/userprog/exec-missing.o
deleted file mode 100644
index cb4a0af..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/exec-missing.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/exec-multiple b/pintos/src/userprog/build/tests/userprog/exec-multiple
deleted file mode 100755
index 75727ce..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/exec-multiple and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/exec-multiple.d b/pintos/src/userprog/build/tests/userprog/exec-multiple.d
deleted file mode 100644
index 1128f7c..0000000
--- a/pintos/src/userprog/build/tests/userprog/exec-multiple.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/exec-multiple.o: ../../tests/userprog/exec-multiple.c \
- ../../lib/user/syscall.h ../../lib/stdbool.h ../../lib/debug.h \
- ../../tests/lib.h ../../lib/stddef.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/exec-multiple.o b/pintos/src/userprog/build/tests/userprog/exec-multiple.o
deleted file mode 100644
index df4c97c..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/exec-multiple.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/exec-once b/pintos/src/userprog/build/tests/userprog/exec-once
deleted file mode 100755
index 39e0544..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/exec-once and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/exec-once.d b/pintos/src/userprog/build/tests/userprog/exec-once.d
deleted file mode 100644
index 782befe..0000000
--- a/pintos/src/userprog/build/tests/userprog/exec-once.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/exec-once.o: ../../tests/userprog/exec-once.c \
- ../../lib/user/syscall.h ../../lib/stdbool.h ../../lib/debug.h \
- ../../tests/lib.h ../../lib/stddef.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/exec-once.o b/pintos/src/userprog/build/tests/userprog/exec-once.o
deleted file mode 100644
index f41e741..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/exec-once.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/exit b/pintos/src/userprog/build/tests/userprog/exit
deleted file mode 100755
index 5a449fa..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/exit and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/exit.d b/pintos/src/userprog/build/tests/userprog/exit.d
deleted file mode 100644
index a4549ee..0000000
--- a/pintos/src/userprog/build/tests/userprog/exit.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/exit.o: ../../tests/userprog/exit.c ../../tests/lib.h \
- ../../lib/debug.h ../../lib/stdbool.h ../../lib/stddef.h \
- ../../lib/user/syscall.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/exit.o b/pintos/src/userprog/build/tests/userprog/exit.o
deleted file mode 100644
index 7519d34..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/exit.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/halt b/pintos/src/userprog/build/tests/userprog/halt
deleted file mode 100755
index cb3a4f9..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/halt and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/halt.d b/pintos/src/userprog/build/tests/userprog/halt.d
deleted file mode 100644
index 6294cd9..0000000
--- a/pintos/src/userprog/build/tests/userprog/halt.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/halt.o: ../../tests/userprog/halt.c ../../tests/lib.h \
- ../../lib/debug.h ../../lib/stdbool.h ../../lib/stddef.h \
- ../../lib/user/syscall.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/halt.o b/pintos/src/userprog/build/tests/userprog/halt.o
deleted file mode 100644
index b4813a1..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/halt.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/multi-child-fd b/pintos/src/userprog/build/tests/userprog/multi-child-fd
deleted file mode 100755
index 0a9cb9e..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/multi-child-fd and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/multi-child-fd.d b/pintos/src/userprog/build/tests/userprog/multi-child-fd.d
deleted file mode 100644
index 8e8fd66..0000000
--- a/pintos/src/userprog/build/tests/userprog/multi-child-fd.d
+++ /dev/null
@@ -1,5 +0,0 @@
-tests/userprog/multi-child-fd.o: ../../tests/userprog/multi-child-fd.c \
- ../../lib/stdio.h ../../lib/debug.h ../../lib/stdarg.h \
- ../../lib/stdbool.h ../../lib/stddef.h ../../lib/stdint.h \
- ../../lib/user/stdio.h ../../lib/user/syscall.h \
- ../../tests/userprog/sample.inc ../../tests/lib.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/multi-child-fd.o b/pintos/src/userprog/build/tests/userprog/multi-child-fd.o
deleted file mode 100644
index fd72dac..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/multi-child-fd.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/multi-recurse b/pintos/src/userprog/build/tests/userprog/multi-recurse
deleted file mode 100755
index 2f98bf3..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/multi-recurse and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/multi-recurse.d b/pintos/src/userprog/build/tests/userprog/multi-recurse.d
deleted file mode 100644
index 1d4d668..0000000
--- a/pintos/src/userprog/build/tests/userprog/multi-recurse.d
+++ /dev/null
@@ -1,5 +0,0 @@
-tests/userprog/multi-recurse.o: ../../tests/userprog/multi-recurse.c \
- ../../lib/debug.h ../../lib/stdlib.h ../../lib/stddef.h \
- ../../lib/stdio.h ../../lib/stdarg.h ../../lib/stdbool.h \
- ../../lib/stdint.h ../../lib/user/stdio.h ../../lib/user/syscall.h \
- ../../tests/lib.h
diff --git a/pintos/src/userprog/build/tests/userprog/multi-recurse.o b/pintos/src/userprog/build/tests/userprog/multi-recurse.o
deleted file mode 100644
index 0e92fe6..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/multi-recurse.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/no-vm/multi-oom b/pintos/src/userprog/build/tests/userprog/no-vm/multi-oom
deleted file mode 100755
index 521c750..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/no-vm/multi-oom and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/no-vm/multi-oom.d b/pintos/src/userprog/build/tests/userprog/no-vm/multi-oom.d
deleted file mode 100644
index 8c1a957..0000000
--- a/pintos/src/userprog/build/tests/userprog/no-vm/multi-oom.d
+++ /dev/null
@@ -1,5 +0,0 @@
-tests/userprog/no-vm/multi-oom.o: ../../tests/userprog/no-vm/multi-oom.c \
- ../../lib/debug.h ../../lib/stdio.h ../../lib/stdarg.h \
- ../../lib/stdbool.h ../../lib/stddef.h ../../lib/stdint.h \
- ../../lib/user/stdio.h ../../lib/string.h ../../lib/stdlib.h \
- ../../lib/user/syscall.h ../../lib/random.h ../../tests/lib.h
diff --git a/pintos/src/userprog/build/tests/userprog/no-vm/multi-oom.o b/pintos/src/userprog/build/tests/userprog/no-vm/multi-oom.o
deleted file mode 100644
index 1779527..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/no-vm/multi-oom.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/open-bad-ptr b/pintos/src/userprog/build/tests/userprog/open-bad-ptr
deleted file mode 100755
index dacf32c..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/open-bad-ptr and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/open-bad-ptr.d b/pintos/src/userprog/build/tests/userprog/open-bad-ptr.d
deleted file mode 100644
index 7359d05..0000000
--- a/pintos/src/userprog/build/tests/userprog/open-bad-ptr.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/open-bad-ptr.o: ../../tests/userprog/open-bad-ptr.c \
- ../../lib/user/syscall.h ../../lib/stdbool.h ../../lib/debug.h \
- ../../tests/lib.h ../../lib/stddef.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/open-bad-ptr.o b/pintos/src/userprog/build/tests/userprog/open-bad-ptr.o
deleted file mode 100644
index 433032c..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/open-bad-ptr.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/open-boundary b/pintos/src/userprog/build/tests/userprog/open-boundary
deleted file mode 100755
index 393df93..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/open-boundary and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/open-boundary.d b/pintos/src/userprog/build/tests/userprog/open-boundary.d
deleted file mode 100644
index 2e237a4..0000000
--- a/pintos/src/userprog/build/tests/userprog/open-boundary.d
+++ /dev/null
@@ -1,4 +0,0 @@
-tests/userprog/open-boundary.o: ../../tests/userprog/open-boundary.c \
- ../../lib/user/syscall.h ../../lib/stdbool.h ../../lib/debug.h \
- ../../tests/userprog/boundary.h ../../tests/lib.h ../../lib/stddef.h \
- ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/open-boundary.o b/pintos/src/userprog/build/tests/userprog/open-boundary.o
deleted file mode 100644
index 6ecf448..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/open-boundary.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/open-empty b/pintos/src/userprog/build/tests/userprog/open-empty
deleted file mode 100755
index a76d303..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/open-empty and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/open-empty.d b/pintos/src/userprog/build/tests/userprog/open-empty.d
deleted file mode 100644
index 8e0ab78..0000000
--- a/pintos/src/userprog/build/tests/userprog/open-empty.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/open-empty.o: ../../tests/userprog/open-empty.c \
- ../../lib/user/syscall.h ../../lib/stdbool.h ../../lib/debug.h \
- ../../tests/lib.h ../../lib/stddef.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/open-empty.o b/pintos/src/userprog/build/tests/userprog/open-empty.o
deleted file mode 100644
index 955219a..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/open-empty.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/open-missing b/pintos/src/userprog/build/tests/userprog/open-missing
deleted file mode 100755
index ec10d10..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/open-missing and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/open-missing.d b/pintos/src/userprog/build/tests/userprog/open-missing.d
deleted file mode 100644
index ded32c2..0000000
--- a/pintos/src/userprog/build/tests/userprog/open-missing.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/open-missing.o: ../../tests/userprog/open-missing.c \
- ../../lib/user/syscall.h ../../lib/stdbool.h ../../lib/debug.h \
- ../../tests/lib.h ../../lib/stddef.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/open-missing.o b/pintos/src/userprog/build/tests/userprog/open-missing.o
deleted file mode 100644
index 1bb26a6..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/open-missing.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/open-normal b/pintos/src/userprog/build/tests/userprog/open-normal
deleted file mode 100755
index b1bea73..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/open-normal and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/open-normal.d b/pintos/src/userprog/build/tests/userprog/open-normal.d
deleted file mode 100644
index fe4770d..0000000
--- a/pintos/src/userprog/build/tests/userprog/open-normal.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/open-normal.o: ../../tests/userprog/open-normal.c \
- ../../lib/user/syscall.h ../../lib/stdbool.h ../../lib/debug.h \
- ../../tests/lib.h ../../lib/stddef.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/open-normal.o b/pintos/src/userprog/build/tests/userprog/open-normal.o
deleted file mode 100644
index d871273..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/open-normal.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/open-null b/pintos/src/userprog/build/tests/userprog/open-null
deleted file mode 100755
index 6503d53..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/open-null and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/open-null.d b/pintos/src/userprog/build/tests/userprog/open-null.d
deleted file mode 100644
index 62d5338..0000000
--- a/pintos/src/userprog/build/tests/userprog/open-null.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/open-null.o: ../../tests/userprog/open-null.c \
- ../../lib/stddef.h ../../lib/user/syscall.h ../../lib/stdbool.h \
- ../../lib/debug.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/open-null.o b/pintos/src/userprog/build/tests/userprog/open-null.o
deleted file mode 100644
index 81b2dbb..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/open-null.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/open-twice b/pintos/src/userprog/build/tests/userprog/open-twice
deleted file mode 100755
index 450943d..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/open-twice and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/open-twice.d b/pintos/src/userprog/build/tests/userprog/open-twice.d
deleted file mode 100644
index 0171bd7..0000000
--- a/pintos/src/userprog/build/tests/userprog/open-twice.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/open-twice.o: ../../tests/userprog/open-twice.c \
- ../../lib/user/syscall.h ../../lib/stdbool.h ../../lib/debug.h \
- ../../tests/lib.h ../../lib/stddef.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/open-twice.o b/pintos/src/userprog/build/tests/userprog/open-twice.o
deleted file mode 100644
index 1002b63..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/open-twice.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/read-bad-fd b/pintos/src/userprog/build/tests/userprog/read-bad-fd
deleted file mode 100755
index 26a53bb..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/read-bad-fd and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/read-bad-fd.d b/pintos/src/userprog/build/tests/userprog/read-bad-fd.d
deleted file mode 100644
index 796acfa..0000000
--- a/pintos/src/userprog/build/tests/userprog/read-bad-fd.d
+++ /dev/null
@@ -1,4 +0,0 @@
-tests/userprog/read-bad-fd.o: ../../tests/userprog/read-bad-fd.c \
- ../../lib/limits.h ../../lib/user/syscall.h ../../lib/stdbool.h \
- ../../lib/debug.h ../../tests/lib.h ../../lib/stddef.h \
- ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/read-bad-fd.o b/pintos/src/userprog/build/tests/userprog/read-bad-fd.o
deleted file mode 100644
index c404520..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/read-bad-fd.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/read-bad-ptr b/pintos/src/userprog/build/tests/userprog/read-bad-ptr
deleted file mode 100755
index 5849d81..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/read-bad-ptr and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/read-bad-ptr.d b/pintos/src/userprog/build/tests/userprog/read-bad-ptr.d
deleted file mode 100644
index 27ee0e2..0000000
--- a/pintos/src/userprog/build/tests/userprog/read-bad-ptr.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/read-bad-ptr.o: ../../tests/userprog/read-bad-ptr.c \
- ../../lib/user/syscall.h ../../lib/stdbool.h ../../lib/debug.h \
- ../../tests/lib.h ../../lib/stddef.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/read-bad-ptr.o b/pintos/src/userprog/build/tests/userprog/read-bad-ptr.o
deleted file mode 100644
index afb0462..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/read-bad-ptr.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/read-boundary b/pintos/src/userprog/build/tests/userprog/read-boundary
deleted file mode 100755
index d529de4..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/read-boundary and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/read-boundary.d b/pintos/src/userprog/build/tests/userprog/read-boundary.d
deleted file mode 100644
index 8a7bdc8..0000000
--- a/pintos/src/userprog/build/tests/userprog/read-boundary.d
+++ /dev/null
@@ -1,4 +0,0 @@
-tests/userprog/read-boundary.o: ../../tests/userprog/read-boundary.c \
- ../../lib/string.h ../../lib/stddef.h ../../lib/user/syscall.h \
- ../../lib/stdbool.h ../../lib/debug.h ../../tests/userprog/boundary.h \
- ../../tests/userprog/sample.inc ../../tests/lib.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/read-boundary.o b/pintos/src/userprog/build/tests/userprog/read-boundary.o
deleted file mode 100644
index 12b150d..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/read-boundary.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/read-normal b/pintos/src/userprog/build/tests/userprog/read-normal
deleted file mode 100755
index ef27de9..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/read-normal and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/read-normal.d b/pintos/src/userprog/build/tests/userprog/read-normal.d
deleted file mode 100644
index 9ec2cdd..0000000
--- a/pintos/src/userprog/build/tests/userprog/read-normal.d
+++ /dev/null
@@ -1,4 +0,0 @@
-tests/userprog/read-normal.o: ../../tests/userprog/read-normal.c \
- ../../tests/userprog/sample.inc ../../tests/lib.h ../../lib/debug.h \
- ../../lib/stdbool.h ../../lib/stddef.h ../../lib/user/syscall.h \
- ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/read-normal.o b/pintos/src/userprog/build/tests/userprog/read-normal.o
deleted file mode 100644
index 0307ae7..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/read-normal.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/read-stdout b/pintos/src/userprog/build/tests/userprog/read-stdout
deleted file mode 100755
index 3ceca7b..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/read-stdout and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/read-stdout.d b/pintos/src/userprog/build/tests/userprog/read-stdout.d
deleted file mode 100644
index e57b915..0000000
--- a/pintos/src/userprog/build/tests/userprog/read-stdout.d
+++ /dev/null
@@ -1,4 +0,0 @@
-tests/userprog/read-stdout.o: ../../tests/userprog/read-stdout.c \
- ../../lib/stdio.h ../../lib/debug.h ../../lib/stdarg.h \
- ../../lib/stdbool.h ../../lib/stddef.h ../../lib/stdint.h \
- ../../lib/user/stdio.h ../../lib/user/syscall.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/read-stdout.o b/pintos/src/userprog/build/tests/userprog/read-stdout.o
deleted file mode 100644
index aecd3e1..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/read-stdout.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/read-zero b/pintos/src/userprog/build/tests/userprog/read-zero
deleted file mode 100755
index 53ef226..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/read-zero and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/read-zero.d b/pintos/src/userprog/build/tests/userprog/read-zero.d
deleted file mode 100644
index 67e21c4..0000000
--- a/pintos/src/userprog/build/tests/userprog/read-zero.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/read-zero.o: ../../tests/userprog/read-zero.c \
- ../../lib/user/syscall.h ../../lib/stdbool.h ../../lib/debug.h \
- ../../tests/lib.h ../../lib/stddef.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/read-zero.o b/pintos/src/userprog/build/tests/userprog/read-zero.o
deleted file mode 100644
index 4c4be5d..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/read-zero.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/rox-child b/pintos/src/userprog/build/tests/userprog/rox-child
deleted file mode 100755
index ae83e67..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/rox-child and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/rox-child.d b/pintos/src/userprog/build/tests/userprog/rox-child.d
deleted file mode 100644
index b00b44d..0000000
--- a/pintos/src/userprog/build/tests/userprog/rox-child.d
+++ /dev/null
@@ -1,4 +0,0 @@
-tests/userprog/rox-child.o: ../../tests/userprog/rox-child.c \
- ../../tests/userprog/rox-child.inc ../../lib/user/syscall.h \
- ../../lib/stdbool.h ../../lib/debug.h ../../tests/lib.h \
- ../../lib/stddef.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/rox-child.o b/pintos/src/userprog/build/tests/userprog/rox-child.o
deleted file mode 100644
index 11ab5a8..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/rox-child.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/rox-multichild b/pintos/src/userprog/build/tests/userprog/rox-multichild
deleted file mode 100755
index 43b77f4..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/rox-multichild and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/rox-multichild.d b/pintos/src/userprog/build/tests/userprog/rox-multichild.d
deleted file mode 100644
index 330578a..0000000
--- a/pintos/src/userprog/build/tests/userprog/rox-multichild.d
+++ /dev/null
@@ -1,4 +0,0 @@
-tests/userprog/rox-multichild.o: ../../tests/userprog/rox-multichild.c \
- ../../tests/userprog/rox-child.inc ../../lib/user/syscall.h \
- ../../lib/stdbool.h ../../lib/debug.h ../../tests/lib.h \
- ../../lib/stddef.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/rox-multichild.o b/pintos/src/userprog/build/tests/userprog/rox-multichild.o
deleted file mode 100644
index 87e6d84..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/rox-multichild.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/rox-simple b/pintos/src/userprog/build/tests/userprog/rox-simple
deleted file mode 100755
index f69f1e8..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/rox-simple and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/rox-simple.d b/pintos/src/userprog/build/tests/userprog/rox-simple.d
deleted file mode 100644
index 315003c..0000000
--- a/pintos/src/userprog/build/tests/userprog/rox-simple.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/rox-simple.o: ../../tests/userprog/rox-simple.c \
- ../../lib/user/syscall.h ../../lib/stdbool.h ../../lib/debug.h \
- ../../tests/lib.h ../../lib/stddef.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/rox-simple.o b/pintos/src/userprog/build/tests/userprog/rox-simple.o
deleted file mode 100644
index 4b8c31c..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/rox-simple.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/sc-bad-arg b/pintos/src/userprog/build/tests/userprog/sc-bad-arg
deleted file mode 100755
index 4a00099..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/sc-bad-arg and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/sc-bad-arg.d b/pintos/src/userprog/build/tests/userprog/sc-bad-arg.d
deleted file mode 100644
index f553a27..0000000
--- a/pintos/src/userprog/build/tests/userprog/sc-bad-arg.d
+++ /dev/null
@@ -1,4 +0,0 @@
-tests/userprog/sc-bad-arg.o: ../../tests/userprog/sc-bad-arg.c \
- ../../lib/syscall-nr.h ../../tests/lib.h ../../lib/debug.h \
- ../../lib/stdbool.h ../../lib/stddef.h ../../lib/user/syscall.h \
- ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/sc-bad-arg.o b/pintos/src/userprog/build/tests/userprog/sc-bad-arg.o
deleted file mode 100644
index 8369477..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/sc-bad-arg.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/sc-bad-sp b/pintos/src/userprog/build/tests/userprog/sc-bad-sp
deleted file mode 100755
index 7f0b5db..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/sc-bad-sp and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/sc-bad-sp.d b/pintos/src/userprog/build/tests/userprog/sc-bad-sp.d
deleted file mode 100644
index f8a62a8..0000000
--- a/pintos/src/userprog/build/tests/userprog/sc-bad-sp.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/sc-bad-sp.o: ../../tests/userprog/sc-bad-sp.c \
- ../../tests/lib.h ../../lib/debug.h ../../lib/stdbool.h \
- ../../lib/stddef.h ../../lib/user/syscall.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/sc-bad-sp.o b/pintos/src/userprog/build/tests/userprog/sc-bad-sp.o
deleted file mode 100644
index 7ca7eab..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/sc-bad-sp.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/sc-boundary b/pintos/src/userprog/build/tests/userprog/sc-boundary
deleted file mode 100755
index e6737cf..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/sc-boundary and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/sc-boundary-2 b/pintos/src/userprog/build/tests/userprog/sc-boundary-2
deleted file mode 100755
index 0857cc5..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/sc-boundary-2 and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/sc-boundary-2.d b/pintos/src/userprog/build/tests/userprog/sc-boundary-2.d
deleted file mode 100644
index 796c725..0000000
--- a/pintos/src/userprog/build/tests/userprog/sc-boundary-2.d
+++ /dev/null
@@ -1,4 +0,0 @@
-tests/userprog/sc-boundary-2.o: ../../tests/userprog/sc-boundary-2.c \
- ../../lib/syscall-nr.h ../../tests/userprog/boundary.h ../../tests/lib.h \
- ../../lib/debug.h ../../lib/stdbool.h ../../lib/stddef.h \
- ../../lib/user/syscall.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/sc-boundary-2.o b/pintos/src/userprog/build/tests/userprog/sc-boundary-2.o
deleted file mode 100644
index 5006ca1..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/sc-boundary-2.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/sc-boundary.d b/pintos/src/userprog/build/tests/userprog/sc-boundary.d
deleted file mode 100644
index dce051d..0000000
--- a/pintos/src/userprog/build/tests/userprog/sc-boundary.d
+++ /dev/null
@@ -1,4 +0,0 @@
-tests/userprog/sc-boundary.o: ../../tests/userprog/sc-boundary.c \
- ../../lib/syscall-nr.h ../../tests/userprog/boundary.h ../../tests/lib.h \
- ../../lib/debug.h ../../lib/stdbool.h ../../lib/stddef.h \
- ../../lib/user/syscall.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/sc-boundary.o b/pintos/src/userprog/build/tests/userprog/sc-boundary.o
deleted file mode 100644
index e223992..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/sc-boundary.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/wait-bad-pid b/pintos/src/userprog/build/tests/userprog/wait-bad-pid
deleted file mode 100755
index 0914b87..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/wait-bad-pid and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/wait-bad-pid.d b/pintos/src/userprog/build/tests/userprog/wait-bad-pid.d
deleted file mode 100644
index 5dd84c4..0000000
--- a/pintos/src/userprog/build/tests/userprog/wait-bad-pid.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/wait-bad-pid.o: ../../tests/userprog/wait-bad-pid.c \
- ../../lib/user/syscall.h ../../lib/stdbool.h ../../lib/debug.h \
- ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/wait-bad-pid.o b/pintos/src/userprog/build/tests/userprog/wait-bad-pid.o
deleted file mode 100644
index a5937d6..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/wait-bad-pid.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/wait-killed b/pintos/src/userprog/build/tests/userprog/wait-killed
deleted file mode 100755
index 70926d3..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/wait-killed and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/wait-killed.d b/pintos/src/userprog/build/tests/userprog/wait-killed.d
deleted file mode 100644
index 30cbeee..0000000
--- a/pintos/src/userprog/build/tests/userprog/wait-killed.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/wait-killed.o: ../../tests/userprog/wait-killed.c \
- ../../lib/user/syscall.h ../../lib/stdbool.h ../../lib/debug.h \
- ../../tests/lib.h ../../lib/stddef.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/wait-killed.o b/pintos/src/userprog/build/tests/userprog/wait-killed.o
deleted file mode 100644
index 36e3a1f..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/wait-killed.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/wait-simple b/pintos/src/userprog/build/tests/userprog/wait-simple
deleted file mode 100755
index 4691dfe..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/wait-simple and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/wait-simple.d b/pintos/src/userprog/build/tests/userprog/wait-simple.d
deleted file mode 100644
index d3bf2f4..0000000
--- a/pintos/src/userprog/build/tests/userprog/wait-simple.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/wait-simple.o: ../../tests/userprog/wait-simple.c \
- ../../lib/user/syscall.h ../../lib/stdbool.h ../../lib/debug.h \
- ../../tests/lib.h ../../lib/stddef.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/wait-simple.o b/pintos/src/userprog/build/tests/userprog/wait-simple.o
deleted file mode 100644
index c4c8e7e..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/wait-simple.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/wait-twice b/pintos/src/userprog/build/tests/userprog/wait-twice
deleted file mode 100755
index 8d4bd1a..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/wait-twice and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/wait-twice.d b/pintos/src/userprog/build/tests/userprog/wait-twice.d
deleted file mode 100644
index 0db96f0..0000000
--- a/pintos/src/userprog/build/tests/userprog/wait-twice.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/wait-twice.o: ../../tests/userprog/wait-twice.c \
- ../../lib/user/syscall.h ../../lib/stdbool.h ../../lib/debug.h \
- ../../tests/lib.h ../../lib/stddef.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/wait-twice.o b/pintos/src/userprog/build/tests/userprog/wait-twice.o
deleted file mode 100644
index 63d24ff..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/wait-twice.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/write-bad-fd b/pintos/src/userprog/build/tests/userprog/write-bad-fd
deleted file mode 100755
index e71aebe..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/write-bad-fd and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/write-bad-fd.d b/pintos/src/userprog/build/tests/userprog/write-bad-fd.d
deleted file mode 100644
index 34628a5..0000000
--- a/pintos/src/userprog/build/tests/userprog/write-bad-fd.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/write-bad-fd.o: ../../tests/userprog/write-bad-fd.c \
- ../../lib/limits.h ../../lib/user/syscall.h ../../lib/stdbool.h \
- ../../lib/debug.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/write-bad-fd.o b/pintos/src/userprog/build/tests/userprog/write-bad-fd.o
deleted file mode 100644
index fc7f60d..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/write-bad-fd.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/write-bad-ptr b/pintos/src/userprog/build/tests/userprog/write-bad-ptr
deleted file mode 100755
index 0ce814f..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/write-bad-ptr and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/write-bad-ptr.d b/pintos/src/userprog/build/tests/userprog/write-bad-ptr.d
deleted file mode 100644
index 8acb3b0..0000000
--- a/pintos/src/userprog/build/tests/userprog/write-bad-ptr.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/write-bad-ptr.o: ../../tests/userprog/write-bad-ptr.c \
- ../../lib/user/syscall.h ../../lib/stdbool.h ../../lib/debug.h \
- ../../tests/lib.h ../../lib/stddef.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/write-bad-ptr.o b/pintos/src/userprog/build/tests/userprog/write-bad-ptr.o
deleted file mode 100644
index c1339e4..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/write-bad-ptr.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/write-boundary b/pintos/src/userprog/build/tests/userprog/write-boundary
deleted file mode 100755
index 0aebaf2..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/write-boundary and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/write-boundary.d b/pintos/src/userprog/build/tests/userprog/write-boundary.d
deleted file mode 100644
index 4c379ce..0000000
--- a/pintos/src/userprog/build/tests/userprog/write-boundary.d
+++ /dev/null
@@ -1,4 +0,0 @@
-tests/userprog/write-boundary.o: ../../tests/userprog/write-boundary.c \
- ../../lib/string.h ../../lib/stddef.h ../../lib/user/syscall.h \
- ../../lib/stdbool.h ../../lib/debug.h ../../tests/userprog/boundary.h \
- ../../tests/userprog/sample.inc ../../tests/lib.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/write-boundary.o b/pintos/src/userprog/build/tests/userprog/write-boundary.o
deleted file mode 100644
index 615675e..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/write-boundary.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/write-normal b/pintos/src/userprog/build/tests/userprog/write-normal
deleted file mode 100755
index 4feabf1..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/write-normal and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/write-normal.d b/pintos/src/userprog/build/tests/userprog/write-normal.d
deleted file mode 100644
index 3c037ce..0000000
--- a/pintos/src/userprog/build/tests/userprog/write-normal.d
+++ /dev/null
@@ -1,4 +0,0 @@
-tests/userprog/write-normal.o: ../../tests/userprog/write-normal.c \
- ../../lib/user/syscall.h ../../lib/stdbool.h ../../lib/debug.h \
- ../../tests/userprog/sample.inc ../../tests/lib.h ../../lib/stddef.h \
- ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/write-normal.o b/pintos/src/userprog/build/tests/userprog/write-normal.o
deleted file mode 100644
index e78a391..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/write-normal.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/write-stdin b/pintos/src/userprog/build/tests/userprog/write-stdin
deleted file mode 100755
index 19d9927..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/write-stdin and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/write-stdin.d b/pintos/src/userprog/build/tests/userprog/write-stdin.d
deleted file mode 100644
index 159bcd9..0000000
--- a/pintos/src/userprog/build/tests/userprog/write-stdin.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/write-stdin.o: ../../tests/userprog/write-stdin.c \
- ../../lib/user/syscall.h ../../lib/stdbool.h ../../lib/debug.h \
- ../../tests/lib.h ../../lib/stddef.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/write-stdin.o b/pintos/src/userprog/build/tests/userprog/write-stdin.o
deleted file mode 100644
index a846aa2..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/write-stdin.o and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/write-zero b/pintos/src/userprog/build/tests/userprog/write-zero
deleted file mode 100755
index 7bb3dae..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/write-zero and /dev/null differ
diff --git a/pintos/src/userprog/build/tests/userprog/write-zero.d b/pintos/src/userprog/build/tests/userprog/write-zero.d
deleted file mode 100644
index d4b02f2..0000000
--- a/pintos/src/userprog/build/tests/userprog/write-zero.d
+++ /dev/null
@@ -1,3 +0,0 @@
-tests/userprog/write-zero.o: ../../tests/userprog/write-zero.c \
- ../../lib/user/syscall.h ../../lib/stdbool.h ../../lib/debug.h \
- ../../tests/lib.h ../../lib/stddef.h ../../tests/main.h
diff --git a/pintos/src/userprog/build/tests/userprog/write-zero.o b/pintos/src/userprog/build/tests/userprog/write-zero.o
deleted file mode 100644
index c9d5bac..0000000
Binary files a/pintos/src/userprog/build/tests/userprog/write-zero.o and /dev/null differ
diff --git a/pintos/src/userprog/build/threads/init.d b/pintos/src/userprog/build/threads/init.d
deleted file mode 100644
index 33e7830..0000000
--- a/pintos/src/userprog/build/threads/init.d
+++ /dev/null
@@ -1,17 +0,0 @@
-threads/init.o: ../../threads/init.c ../../threads/init.h \
- ../../lib/debug.h ../../lib/stdbool.h ../../lib/stddef.h \
- ../../lib/stdint.h ../../lib/kernel/console.h ../../lib/inttypes.h \
- ../../lib/limits.h ../../lib/random.h ../../lib/stdio.h \
- ../../lib/stdarg.h ../../lib/kernel/stdio.h ../../lib/stdlib.h \
- ../../lib/string.h ../../devices/kbd.h ../../devices/input.h \
- ../../devices/serial.h ../../devices/shutdown.h ../../devices/timer.h \
- ../../lib/round.h ../../devices/vga.h ../../devices/rtc.h \
- ../../threads/interrupt.h ../../threads/io.h ../../threads/loader.h \
- ../../threads/malloc.h ../../threads/palloc.h ../../threads/pte.h \
- ../../threads/vaddr.h ../../threads/thread.h ../../lib/kernel/list.h \
- ../../threads/fixed-point.h ../../lib/kernel/hash.h \
- ../../lib/kernel/list.h ../../threads/synch.h ../../userprog/process.h \
- ../../filesys/file.h ../../filesys/off_t.h ../../userprog/exception.h \
- ../../userprog/gdt.h ../../userprog/syscall.h ../../userprog/tss.h \
- ../../devices/block.h ../../devices/ide.h ../../filesys/filesys.h \
- ../../filesys/fsutil.h
diff --git a/pintos/src/userprog/build/threads/init.o b/pintos/src/userprog/build/threads/init.o
deleted file mode 100644
index a9bf28a..0000000
Binary files a/pintos/src/userprog/build/threads/init.o and /dev/null differ
diff --git a/pintos/src/userprog/build/threads/interrupt.d b/pintos/src/userprog/build/threads/interrupt.d
deleted file mode 100644
index 2d63361..0000000
--- a/pintos/src/userprog/build/threads/interrupt.d
+++ /dev/null
@@ -1,9 +0,0 @@
-threads/interrupt.o: ../../threads/interrupt.c ../../threads/interrupt.h \
- ../../lib/stdbool.h ../../lib/stdint.h ../../lib/debug.h \
- ../../lib/inttypes.h ../../lib/stdio.h ../../lib/stdarg.h \
- ../../lib/stddef.h ../../lib/kernel/stdio.h ../../threads/flags.h \
- ../../threads/intr-stubs.h ../../threads/io.h ../../threads/thread.h \
- ../../lib/kernel/list.h ../../threads/fixed-point.h \
- ../../threads/vaddr.h ../../threads/loader.h ../../lib/kernel/hash.h \
- ../../lib/kernel/list.h ../../threads/synch.h ../../devices/timer.h \
- ../../lib/round.h
diff --git a/pintos/src/userprog/build/threads/interrupt.o b/pintos/src/userprog/build/threads/interrupt.o
deleted file mode 100644
index c006e88..0000000
Binary files a/pintos/src/userprog/build/threads/interrupt.o and /dev/null differ
diff --git a/pintos/src/userprog/build/threads/intr-stubs.d b/pintos/src/userprog/build/threads/intr-stubs.d
deleted file mode 100644
index 3137c71..0000000
--- a/pintos/src/userprog/build/threads/intr-stubs.d
+++ /dev/null
@@ -1 +0,0 @@
-threads/intr-stubs.o: ../../threads/intr-stubs.S ../../threads/loader.h
diff --git a/pintos/src/userprog/build/threads/intr-stubs.o b/pintos/src/userprog/build/threads/intr-stubs.o
deleted file mode 100644
index f7a8066..0000000
Binary files a/pintos/src/userprog/build/threads/intr-stubs.o and /dev/null differ
diff --git a/pintos/src/userprog/build/threads/kernel.lds.s b/pintos/src/userprog/build/threads/kernel.lds.s
deleted file mode 100644
index 778681d..0000000
--- a/pintos/src/userprog/build/threads/kernel.lds.s
+++ /dev/null
@@ -1,19 +0,0 @@
-OUTPUT_FORMAT("elf32-i386")
-OUTPUT_ARCH("i386")
-ENTRY(start)
-SECTIONS
-{
-  _start = 0xc0000000 + 0x20000;
-  . = _start + SIZEOF_HEADERS;
-  .text : { *(.start) *(.text) } = 0x90
-  .rodata : { *(.rodata) *(.rodata.*)
-       . = ALIGN(0x1000);
-       _end_kernel_text = .; }
-  .data : { *(.data)
-     _signature = .; LONG(0xaa55aa55) }
-  _start_bss = .;
-  .bss : { *(.bss) }
-  _end_bss = .;
-  _end = .;
-  ASSERT (_end - _start <= 512K, "Kernel image is too big.")
-}
diff --git a/pintos/src/userprog/build/threads/loader.o b/pintos/src/userprog/build/threads/loader.o
deleted file mode 100644
index a5cc450..0000000
Binary files a/pintos/src/userprog/build/threads/loader.o and /dev/null differ
diff --git a/pintos/src/userprog/build/threads/malloc.d b/pintos/src/userprog/build/threads/malloc.d
deleted file mode 100644
index f717c3e..0000000
--- a/pintos/src/userprog/build/threads/malloc.d
+++ /dev/null
@@ -1,6 +0,0 @@
-threads/malloc.o: ../../threads/malloc.c ../../threads/malloc.h \
- ../../lib/debug.h ../../lib/stddef.h ../../lib/kernel/list.h \
- ../../lib/stdbool.h ../../lib/stdint.h ../../lib/round.h \
- ../../lib/stdio.h ../../lib/stdarg.h ../../lib/kernel/stdio.h \
- ../../lib/string.h ../../threads/palloc.h ../../threads/synch.h \
- ../../threads/vaddr.h ../../threads/loader.h
diff --git a/pintos/src/userprog/build/threads/malloc.o b/pintos/src/userprog/build/threads/malloc.o
deleted file mode 100644
index 3eea03c..0000000
Binary files a/pintos/src/userprog/build/threads/malloc.o and /dev/null differ
diff --git a/pintos/src/userprog/build/threads/palloc.d b/pintos/src/userprog/build/threads/palloc.d
deleted file mode 100644
index 13faec6..0000000
--- a/pintos/src/userprog/build/threads/palloc.d
+++ /dev/null
@@ -1,6 +0,0 @@
-threads/palloc.o: ../../threads/palloc.c ../../threads/palloc.h \
- ../../lib/stddef.h ../../lib/kernel/bitmap.h ../../lib/stdbool.h \
- ../../lib/inttypes.h ../../lib/stdint.h ../../lib/debug.h \
- ../../lib/round.h ../../lib/stdio.h ../../lib/stdarg.h \
- ../../lib/kernel/stdio.h ../../lib/string.h ../../threads/loader.h \
- ../../threads/synch.h ../../lib/kernel/list.h ../../threads/vaddr.h
diff --git a/pintos/src/userprog/build/threads/palloc.o b/pintos/src/userprog/build/threads/palloc.o
deleted file mode 100644
index f8d2295..0000000
Binary files a/pintos/src/userprog/build/threads/palloc.o and /dev/null differ
diff --git a/pintos/src/userprog/build/threads/start.d b/pintos/src/userprog/build/threads/start.d
deleted file mode 100644
index 80ba309..0000000
--- a/pintos/src/userprog/build/threads/start.d
+++ /dev/null
@@ -1 +0,0 @@
-threads/start.o: ../../threads/start.S ../../threads/loader.h
diff --git a/pintos/src/userprog/build/threads/start.o b/pintos/src/userprog/build/threads/start.o
deleted file mode 100644
index 587dee3..0000000
Binary files a/pintos/src/userprog/build/threads/start.o and /dev/null differ
diff --git a/pintos/src/userprog/build/threads/switch.d b/pintos/src/userprog/build/threads/switch.d
deleted file mode 100644
index 5c98388..0000000
--- a/pintos/src/userprog/build/threads/switch.d
+++ /dev/null
@@ -1 +0,0 @@
-threads/switch.o: ../../threads/switch.S ../../threads/switch.h
diff --git a/pintos/src/userprog/build/threads/switch.o b/pintos/src/userprog/build/threads/switch.o
deleted file mode 100644
index de19ec2..0000000
Binary files a/pintos/src/userprog/build/threads/switch.o and /dev/null differ
diff --git a/pintos/src/userprog/build/threads/synch.d b/pintos/src/userprog/build/threads/synch.d
deleted file mode 100644
index 3cadd04..0000000
--- a/pintos/src/userprog/build/threads/synch.d
+++ /dev/null
@@ -1,7 +0,0 @@
-threads/synch.o: ../../threads/synch.c ../../threads/synch.h \
- ../../lib/kernel/list.h ../../lib/stdbool.h ../../lib/stddef.h \
- ../../lib/stdint.h ../../lib/stdio.h ../../lib/debug.h \
- ../../lib/stdarg.h ../../lib/kernel/stdio.h ../../lib/string.h \
- ../../threads/interrupt.h ../../threads/thread.h \
- ../../threads/fixed-point.h ../../threads/vaddr.h ../../threads/loader.h \
- ../../lib/kernel/hash.h ../../lib/kernel/list.h
diff --git a/pintos/src/userprog/build/threads/synch.o b/pintos/src/userprog/build/threads/synch.o
deleted file mode 100644
index b82d9c4..0000000
Binary files a/pintos/src/userprog/build/threads/synch.o and /dev/null differ
diff --git a/pintos/src/userprog/build/threads/thread.d b/pintos/src/userprog/build/threads/thread.d
deleted file mode 100644
index 8c636e4..0000000
--- a/pintos/src/userprog/build/threads/thread.d
+++ /dev/null
@@ -1,10 +0,0 @@
-threads/thread.o: ../../threads/thread.c ../../threads/thread.h \
- ../../lib/debug.h ../../lib/kernel/list.h ../../lib/stdbool.h \
- ../../lib/stddef.h ../../lib/stdint.h ../../threads/fixed-point.h \
- ../../threads/vaddr.h ../../threads/loader.h ../../lib/kernel/hash.h \
- ../../lib/kernel/list.h ../../threads/synch.h ../../lib/random.h \
- ../../lib/stdio.h ../../lib/stdarg.h ../../lib/kernel/stdio.h \
- ../../lib/string.h ../../devices/timer.h ../../lib/round.h \
- ../../threads/flags.h ../../threads/interrupt.h \
- ../../threads/intr-stubs.h ../../threads/palloc.h ../../threads/switch.h \
- ../../userprog/process.h ../../filesys/file.h ../../filesys/off_t.h
diff --git a/pintos/src/userprog/build/threads/thread.o b/pintos/src/userprog/build/threads/thread.o
deleted file mode 100644
index 1369f10..0000000
Binary files a/pintos/src/userprog/build/threads/thread.o and /dev/null differ
diff --git a/pintos/src/userprog/build/userprog/exception.d b/pintos/src/userprog/build/userprog/exception.d
deleted file mode 100644
index ed933d7..0000000
--- a/pintos/src/userprog/build/userprog/exception.d
+++ /dev/null
@@ -1,8 +0,0 @@
-userprog/exception.o: ../../userprog/exception.c \
- ../../userprog/exception.h ../../lib/inttypes.h ../../lib/stdint.h \
- ../../lib/stdio.h ../../lib/debug.h ../../lib/stdarg.h \
- ../../lib/stdbool.h ../../lib/stddef.h ../../lib/kernel/stdio.h \
- ../../userprog/gdt.h ../../threads/loader.h ../../threads/interrupt.h \
- ../../threads/thread.h ../../lib/kernel/list.h \
- ../../threads/fixed-point.h ../../threads/vaddr.h \
- ../../lib/kernel/hash.h ../../lib/kernel/list.h ../../threads/synch.h
diff --git a/pintos/src/userprog/build/userprog/exception.o b/pintos/src/userprog/build/userprog/exception.o
deleted file mode 100644
index 56cba48..0000000
Binary files a/pintos/src/userprog/build/userprog/exception.o and /dev/null differ
diff --git a/pintos/src/userprog/build/userprog/gdt.d b/pintos/src/userprog/build/userprog/gdt.d
deleted file mode 100644
index 4541e22..0000000
--- a/pintos/src/userprog/build/userprog/gdt.d
+++ /dev/null
@@ -1,4 +0,0 @@
-userprog/gdt.o: ../../userprog/gdt.c ../../userprog/gdt.h \
- ../../threads/loader.h ../../lib/stdint.h ../../lib/debug.h \
- ../../userprog/tss.h ../../threads/palloc.h ../../lib/stddef.h \
- ../../threads/vaddr.h ../../lib/stdbool.h
diff --git a/pintos/src/userprog/build/userprog/gdt.o b/pintos/src/userprog/build/userprog/gdt.o
deleted file mode 100644
index f6360e9..0000000
Binary files a/pintos/src/userprog/build/userprog/gdt.o and /dev/null differ
diff --git a/pintos/src/userprog/build/userprog/pagedir.d b/pintos/src/userprog/build/userprog/pagedir.d
deleted file mode 100644
index 04876dc..0000000
--- a/pintos/src/userprog/build/userprog/pagedir.d
+++ /dev/null
@@ -1,5 +0,0 @@
-userprog/pagedir.o: ../../userprog/pagedir.c ../../userprog/pagedir.h \
- ../../lib/stdbool.h ../../lib/stdint.h ../../lib/stddef.h \
- ../../lib/string.h ../../threads/init.h ../../lib/debug.h \
- ../../threads/pte.h ../../threads/vaddr.h ../../threads/loader.h \
- ../../threads/palloc.h
diff --git a/pintos/src/userprog/build/userprog/pagedir.o b/pintos/src/userprog/build/userprog/pagedir.o
deleted file mode 100644
index a13bffa..0000000
Binary files a/pintos/src/userprog/build/userprog/pagedir.o and /dev/null differ
diff --git a/pintos/src/userprog/build/userprog/process.d b/pintos/src/userprog/build/userprog/process.d
deleted file mode 100644
index 5e95ae5..0000000
--- a/pintos/src/userprog/build/userprog/process.d
+++ /dev/null
@@ -1,12 +0,0 @@
-userprog/process.o: ../../userprog/process.c ../../userprog/process.h \
- ../../filesys/file.h ../../filesys/off_t.h ../../lib/stdint.h \
- ../../threads/thread.h ../../lib/debug.h ../../lib/kernel/list.h \
- ../../lib/stdbool.h ../../lib/stddef.h ../../threads/fixed-point.h \
- ../../threads/vaddr.h ../../threads/loader.h ../../lib/kernel/hash.h \
- ../../lib/kernel/list.h ../../threads/synch.h ../../lib/inttypes.h \
- ../../lib/round.h ../../lib/stdio.h ../../lib/stdarg.h \
- ../../lib/kernel/stdio.h ../../lib/stdlib.h ../../lib/string.h \
- ../../userprog/gdt.h ../../userprog/pagedir.h ../../userprog/tss.h \
- ../../filesys/directory.h ../../devices/block.h ../../filesys/filesys.h \
- ../../threads/flags.h ../../threads/init.h ../../threads/interrupt.h \
- ../../threads/malloc.h ../../threads/palloc.h ../../devices/timer.h
diff --git a/pintos/src/userprog/build/userprog/process.o b/pintos/src/userprog/build/userprog/process.o
deleted file mode 100644
index d735799..0000000
Binary files a/pintos/src/userprog/build/userprog/process.o and /dev/null differ
diff --git a/pintos/src/userprog/build/userprog/syscall.d b/pintos/src/userprog/build/userprog/syscall.d
deleted file mode 100644
index df55d89..0000000
--- a/pintos/src/userprog/build/userprog/syscall.d
+++ /dev/null
@@ -1,10 +0,0 @@
-userprog/syscall.o: ../../userprog/syscall.c ../../userprog/syscall.h \
- ../../lib/stdint.h ../../userprog/process.h ../../filesys/file.h \
- ../../filesys/off_t.h ../../threads/thread.h ../../lib/debug.h \
- ../../lib/kernel/list.h ../../lib/stdbool.h ../../lib/stddef.h \
- ../../threads/fixed-point.h ../../threads/vaddr.h ../../threads/loader.h \
- ../../lib/kernel/hash.h ../../lib/kernel/list.h ../../threads/synch.h \
- ../../lib/stdio.h ../../lib/stdarg.h ../../lib/kernel/stdio.h \
- ../../lib/syscall-nr.h ../../threads/interrupt.h ../../threads/malloc.h \
- ../../filesys/filesys.h ../../devices/input.h ../../devices/shutdown.h \
- ../../lib/kernel/hash.h
diff --git a/pintos/src/userprog/build/userprog/syscall.o b/pintos/src/userprog/build/userprog/syscall.o
deleted file mode 100644
index cb50aa3..0000000
Binary files a/pintos/src/userprog/build/userprog/syscall.o and /dev/null differ
diff --git a/pintos/src/userprog/build/userprog/tss.d b/pintos/src/userprog/build/userprog/tss.d
deleted file mode 100644
index 033c2b5..0000000
--- a/pintos/src/userprog/build/userprog/tss.d
+++ /dev/null
@@ -1,6 +0,0 @@
-userprog/tss.o: ../../userprog/tss.c ../../userprog/tss.h \
- ../../lib/stdint.h ../../lib/debug.h ../../lib/stddef.h \
- ../../userprog/gdt.h ../../threads/loader.h ../../threads/thread.h \
- ../../lib/kernel/list.h ../../lib/stdbool.h ../../threads/fixed-point.h \
- ../../threads/vaddr.h ../../lib/kernel/hash.h ../../lib/kernel/list.h \
- ../../threads/synch.h ../../threads/palloc.h
diff --git a/pintos/src/userprog/build/userprog/tss.o b/pintos/src/userprog/build/userprog/tss.o
deleted file mode 100644
index 9eea4d2..0000000
Binary files a/pintos/src/userprog/build/userprog/tss.o and /dev/null differ
diff --git a/pintos/src/userprog/designdoc.txt b/pintos/src/userprog/designdoc.txt
deleted file mode 100644
index db15299..0000000
--- a/pintos/src/userprog/designdoc.txt
+++ /dev/null
@@ -1,142 +0,0 @@
-             +--------------------------+
-             |          CS 140          |
-             | PROJECT 2: USER PROGRAMS |
-             |     DESIGN DOCUMENT      |
-             +--------------------------+
-
----- GROUP ----
-
->> Fill in the names and email addresses of your group members.
-
-Elizabeth Davis     <eseraiah@stanford.edu>
-Jay       Evans     <  jevans@stanford.edu>
-Nicholas  Chee-awai
-
----- PRELIMINARIES ----
-
->> If you have any preliminary comments on your submission, notes for the
->> TAs, or extra credit, please give them here.
-
->> Please cite any offline or online sources you consulted while
->> preparing your submission, other than the Pintos documentation, course
->> text, lecture notes, and course staff.
-
-               ARGUMENT PASSING
-               ================
-
----- DATA STRUCTURES ----
-
->> A1: Copy here the declaration of each new or changed `struct' or
->> `struct' member, global or static variable, `typedef', or
->> enumeration.  Identify the purpose of each in 25 words or less.
-
----- ALGORITHMS ----
-
->> A2: Briefly describe how you implemented argument parsing.  How do
->> you arrange for the elements of argv[] to be in the right order?
->> How do you avoid overflowing the stack page?
-
----- RATIONALE ----
-
->> A3: Why does Pintos implement strtok_r() but not strtok()?
-
-   Unlike strtok(), strtok_r() is thread-safe.
-
-
->> A4: In Pintos, the kernel separates commands into a executable name
->> and arguments.  In Unix-like systems, the shell does this
->> separation.  Identify at least two advantages of the Unix approach.
-
-                 SYSTEM CALLS
-                 ============
-
----- DATA STRUCTURES ----
-
->> B1: Copy here the declaration of each new or changed `struct' or
->> `struct' member, global or static variable, `typedef', or
->> enumeration.  Identify the purpose of each in 25 words or less.
-
->> B2: Describe how file descriptors are associated with open files.
->> Are file descriptors unique within the entire OS or just within a
->> single process?
-
-   File descriptors each map to one open file. File descriptors are
-   are unique within a single process.
-
----- ALGORITHMS ----
-
->> B3: Describe your code for reading and writing user data from the
->> kernel.
-
->> B4: Suppose a system call causes a full page (4,096 bytes) of data
->> to be copied from user space into the kernel.  What is the least
->> and the greatest possible number of inspections of the page table
->> (e.g. calls to pagedir_get_page()) that might result?  What about
->> for a system call that only copies 2 bytes of data?  Is there room
->> for improvement in these numbers, and how much?
-
->> B5: Briefly describe your implementation of the "wait" system call
->> and how it interacts with process termination.
-
->> B6: Any access to user program memory at a user-specified address
->> can fail due to a bad pointer value.  Such accesses must cause the
->> process to be terminated.  System calls are fraught with such
->> accesses, e.g. a "write" system call requires reading the system
->> call number from the user stack, then each of the call's three
->> arguments, then an arbitrary amount of user memory, and any of
->> these can fail at any point.  This poses a design and
->> error-handling problem: how do you best avoid obscuring the primary
->> function of code in a morass of error-handling?  Furthermore, when
->> an error is detected, how do you ensure that all temporarily
->> allocated resources (locks, buffers, etc.) are freed?  In a few
->> paragraphs, describe the strategy or strategies you adopted for
->> managing these issues.  Give an example.
-
----- SYNCHRONIZATION ----
-
->> B7: The "exec" system call returns -1 if loading the new executable
->> fails, so it cannot return before the new executable has completed
->> loading.  How does your code ensure this?  How is the load
->> success/failure status passed back to the thread that calls "exec"?
-
->> B8: Consider parent process P with child process C.  How do you
->> ensure proper synchronization and avoid race conditions when P
->> calls wait(C) before C exits?  After C exits?  How do you ensure
->> that all resources are freed in each case?  How about when P
->> terminates without waiting, before C exits?  After C exits?  Are
->> there any special cases?
-
----- RATIONALE ----
-
->> B9: Why did you choose to implement access to user memory from the
->> kernel in the way that you did?
-
->> B10: What advantages or disadvantages can you see to your design
->> for file descriptors?
-
->> B11: The default tid_t to pid_t mapping is the identity mapping.
->> If you changed it, what advantages are there to your approach?
-
-               SURVEY QUESTIONS
-               ================
-
-Answering these questions is optional, but it will help us improve the
-course in future quarters.  Feel free to tell us anything you
-want--these questions are just to spur your thoughts.  You may also
-choose to respond anonymously in the course evaluations at the end of
-the quarter.
-
->> In your opinion, was this assignment, or any one of the three problems
->> in it, too easy or too hard?  Did it take too long or too little time?
-
->> Did you find that working on a particular part of the assignment gave
->> you greater insight into some aspect of OS design?
-
->> Is there some particular fact or hint we should give students in
->> future quarters to help them solve the problems?  Conversely, did you
->> find any of our guidance to be misleading?
-
->> Do you have any suggestions for the TAs to more effectively assist
->> students, either for future quarters or the remaining projects?
-
->> Any other comments?
diff --git a/pintos/src/userprog/filesys.dsk b/pintos/src/userprog/filesys.dsk
index 8dde6f1..91a1123 100644
Binary files a/pintos/src/userprog/filesys.dsk and b/pintos/src/userprog/filesys.dsk differ
diff --git a/pintos/src/userprog/process.c b/pintos/src/userprog/process.c
index d77c246..7a7d5c1 100644
--- a/pintos/src/userprog/process.c
+++ b/pintos/src/userprog/process.c
@@ -146,7 +146,6 @@ process_wait (tid_t child_tid)
   struct thread *t = thread_current ();
   struct relation *rel;
   struct list_elem *rel_iter;
-  bool success;
   /* Find the relation with the tid. */ 
   for (rel_iter = list_begin (&t->children_in_r);
        rel_iter != list_end (&t->children_in_r);
@@ -173,12 +172,8 @@ process_wait (tid_t child_tid)
     }
   int status = rel->w_status;
   list_remove (&rel->elem);
-<<<<<<< HEAD
-  //free (rel);
-=======
   lock_release (rel->status_lock);
   free (rel);
->>>>>>> 163b3fb508b9671f40e31dd633f81709dc0e20a7
   return status;
 }
 
@@ -214,29 +209,6 @@ process_exit (void)
         lock_release (cur->parent_in_r->status_lock);
     }
 
-<<<<<<< HEAD
-   /* Orphan all children. */
-   rel_iter = list_begin (&cur->children_in_r);
-   while (rel_iter != list_end (&cur->children_in_r))
-     {
-       rel = list_entry (rel_iter, struct relation, elem);
-       while (!lock_try_acquire (&rel->status_lock))
-         thread_yield ();
-       if (rel->c_status == P_EXITED)
-         {
-           rel_iter = list_next (rel_iter);
-           list_remove (&rel->elem);
-           free (rel);
-         }
-       else
-         {
-           rel->p_status = P_EXITED;
-           rel_iter = list_next (rel_iter);
-           list_remove (&rel->elem);
-           lock_release (&rel->status_lock);
-         }
-     }
-=======
   /* Orphan all children. */
   struct list_elem *rel_iter;
   while (!list_empty (&cur->children_in_r))
@@ -254,7 +226,6 @@ process_exit (void)
       else
         lock_release (rel->status_lock);
     }
->>>>>>> 163b3fb508b9671f40e31dd633f81709dc0e20a7
  
   /* Close all fds in fdt. */
   lock_acquire (&fs_lock);
