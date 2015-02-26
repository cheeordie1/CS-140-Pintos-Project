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

/* Initialize Supplementary Page Table. */
void
page_table_init ()
{
  hash_init (&sp_table.pt_hash, page_hash, page_cmp, NULL);
  lock_init (&sp_table.pt_lock);
}

/* Alloc a supplementary page table entry 
   and insert it into the table. */
struct sp_entry *
page_supp_alloc (struct thread *t, uint8_t *upage)
{
  struct spt_entry *spe = malloc (sizeof (struct spt_entry));
  spe->fd = -1;
  spe->idx = SIZE_MAX;
  spe->location = UNMAPPED;
  spe->bytes = 0;
  spe->writable = true;
  spe->upage = upage;
  spe->t = t;
  lock_acquire (&sp_table.pt_lock);
  hash_insert (&sp_table.pt_hash, &spe->elem);
  lock_release (&sp_table.pt_lock); 
}

void
page_supp_delete (struct sp_entry *spe)
{
  hash_delete (&sp_table.pt_hash, &spe->elem);
  switch (spe->location)
    {
      // TODO Implement deletion of pages through supp page table
      case UNMAPPED:
      case FRAMED:
      case FILESYSTEM:
      case SWAP:
      break
      default: NOT_USED ();
    }
  delete (spe);
}

/* Hash function to insert elements into 
   supplementary page table. */
static unsigned
page_hash (const struct hash_elem *e, void *aux UNUSED)
{
  struct spt_entry *spte = hash_entry (e, struct spt_entry, elem);
  return  hash_int ((uint32_t) frame_get (spte->ft_idx));
}

/* Comparison function to search supplementary 
   page table hash. Two entries are equivalent if 
   they have the same page virtual address and the 
   same thread owner tid. */
static bool
page_cmp (const struct hash_elem *a,
          const struct hash_elem *b,
          void *aux UNUSED)
{
  struct spt_entry *spe_a = hash_entry (a, struct sp_entry, elem);
  struct spt_entry *spe_b = hash_entry (b, struct sp_entry, elem);
	  return spe_a->upage < spe_b->upage && spe_a->t->tid != spe_b->t->tid;
}
