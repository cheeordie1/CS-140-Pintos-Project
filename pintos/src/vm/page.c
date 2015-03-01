#include "vm/page.h"
#include "vm/frame.h"
#include "vm/swap.h"

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
page_supp_init ()
{
  hash_init (&sp_table.pt_hash, page_hash, page_cmp, NULL);
  lock_init (&sp_table.pt_lock);
}

/* Alloc a supplementary page table entry 
   and insert it into the table. */
struct sp_entry *
page_supp_alloc (struct thread *t, uint8_t *upage)
{
  struct sp_entry *spe = malloc (sizeof (struct sp_entry));
  spe->fp = NULL;
  spe->idx = SIZE_MAX;
  spe->sector = SIZE_MAX;
  spe->location = UNMAPPED;
  spe->read_bytes = 0;
  spe->zero_bytes = 0;
  spe->ofs = 0;
  spe->writable = true;
  spe->upage = upage;
  spe->pagedir = t->pagedir;
  spe->tid = t->tid;
  lock_acquire (&sp_table.pt_lock);
  list_push_back (&t->spe_list, &spe->l_elem);
  hash_insert (&sp_table.pt_hash, &spe->h_elem);
  lock_release (&sp_table.pt_lock); 
  return spe;
}

/* Delete all entries in the supplementary
   page table. */
void
page_supp_destroy (struct thread *t)
{
  while (!list_empty (&t->spe_list)) 
  {
    lock_acquire (&sp_table.pt_lock); 
    struct sp_entry *spe = list_entry (list_pop_front (&t->spe_list), 
                                       struct sp_entry, l_elem);
    list_remove (&spe->l_elem);
    hash_delete (&sp_table.pt_hash, &spe->h_elem);
    lock_release (&sp_table.pt_lock); 
    page_supp_delete (spe);
  }
}

/* Delete supplementary page entry. */
void
page_supp_delete (struct sp_entry *spe)
{
  switch (spe->location)
    {
      case FRAMED:
        frame_delete (spe);
        break;
      case FILESYSTEM:
        break;
      case SWAPPED:
        swap_delete (spe);
        break;
      default:
        barrier ();
        break;
    }
  free (spe);
}

struct sp_entry *page_find (struct thread *t, void *vaddr) 
{
  struct sp_entry singleton;
  singleton.tid = t->tid;
  singleton.upage = (uint8_t *) vaddr;

  struct hash_elem *spe_elem = hash_find (&sp_table.pt_hash, 
                                          &singleton.h_elem);
  if (spe_elem == NULL) 
    {
      return NULL;
    }
  return hash_entry (spe_elem, struct sp_entry, h_elem);
}

/* Hash function to insert elements into 
   supplementary page table. */
static unsigned
page_hash (const struct hash_elem *e, void *aux UNUSED)
{
  struct sp_entry *spe = hash_entry (e, struct sp_entry, h_elem);
  return  hash_int ((uint32_t) spe->upage);
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
  struct sp_entry *spe_a = hash_entry (a, struct sp_entry, h_elem);
  struct sp_entry *spe_b = hash_entry (b, struct sp_entry, h_elem);
	  return spe_a->upage < spe_b->upage && spe_a->tid < spe_b->tid;
}
