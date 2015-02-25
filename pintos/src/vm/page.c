#include "vm/page.h"
#include "vm/frame.h"

struct page_table 
  {
    struct hash pt_hash;
    struct lock pt_lock;
  };

static unsigned page_hash (const struct hash_elem *e, void *aux UNUSED);
static bool page_cmp (const struct hash_elem *a,
                      const struct hash_elem *b,
                      void *aux);

static struct page_table sp_table;

void
page_table_init ()
{
  hash_init (&sp_table.pt_hash, page_hash, page_cmp, NULL);
  lock_init (&sp_table.pt_lock);
}

void
page_table_alloc (size_t index, enum cache_flag flag)
{
  lock_acquire (&sp_table.pt_lock);
  
  struct spt_entry *spte = malloc (sizeof (struct spt_entry));
  spte->location = flag;
  spte->ft_idx = index;
  hash_insert (&sp_table.pt_hash, &spte->elem);
  lock_release (&sp_table.pt_lock); 
}

static unsigned
page_hash (const struct hash_elem *e, void *aux UNUSED)
{
  struct spt_entry *spte = hash_entry (e, struct spt_entry, elem);
  return  hash_int ((uint32_t) frame_get (spte->ft_idx));
}

static bool
page_cmp (const struct hash_elem *a,
          const struct hash_elem *b,
          void *aux UNUSED)
{
  struct spt_entry *spte_a = hash_entry (a, struct spt_entry, elem);
  struct spt_entry *spte_b = hash_entry (b, struct spt_entry, elem);
  return frame_get (spte_a->ft_idx) < frame_get (spte_b->ft_idx);

}




