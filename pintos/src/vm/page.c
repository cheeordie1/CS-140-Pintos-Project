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
  struct sp_entry *spe = malloc (sizeof (struct sp_entry));
  spe->fp = NULL;
  spe->idx = SIZE_MAX;
  spe->location = UNMAPPED;
  spe->read_bytes = 0;
  spe->zero_bytes = 0;
  spe->ofs = 0;
  spe->writable = true;
  spe->upage = upage;
  spe->t = t;
  lock_acquire (&sp_table.pt_lock);
  hash_insert (&sp_table.pt_hash, &spe->h_elem);
  list_push_back (&t->spe_list, &spe->l_elem);
  lock_release (&sp_table.pt_lock); 
  return spe;
}

void
page_supp_destroy (struct thread *t)
{
  while (!list_empty (&t->spe_list)) 
  {
    struct sp_entry *spe = list_entry (list_pop_front (&t->spe_list), 
                                       struct sp_entry, l_elem);
    page_supp_delete (spe);
  }
}


void
page_supp_delete (struct sp_entry *spe)
{
  list_remove (&spe->l_elem);
  lock_acquire (&sp_table.pt_lock); 
  hash_delete (&sp_table.pt_hash, &spe->h_elem);
  lock_release (&sp_table.pt_lock); 
  switch (spe->location)
    {
      case FRAMED:
        frame_delete (spe);
        break;
      case FILESYSTEM:
        file_close (spe->fp);
        break;
      case SWAPPED:
        // TODO: implement SWAPPED case
        break;
      break;
      default:
        break;
    }
  free (spe);
}

/* Hash function to insert elements into 
   supplementary page table. */
static unsigned
page_hash (const struct hash_elem *e, void *aux UNUSED)
{
  struct sp_entry *spe = hash_entry (e, struct sp_entry, h_elem);
  return  hash_int ((uint32_t) frame_get (spe->idx));
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
	  return spe_a->upage < spe_b->upage && spe_a->t->tid != spe_b->t->tid;
}
