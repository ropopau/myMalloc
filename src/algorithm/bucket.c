#include "bucket.h"


void *starting_point = NULL;


/*
 * Get the bucket given a memory address 
 */
struct bucket *get_begin(void *ptr)
{

    uintptr_t ps = sysconf(_SC_PAGESIZE) - 1;
    uintptr_t bucket_start_ptr = (uintptr_t)ptr & ~ps;
    return (struct bucket *)bucket_start_ptr;
}

/*
 * Return the size that is aligned with a given alignment
 * 
 */
struct bucket *bucket_new(size_t block_size)
{
    // computing final size of the bucket
    // example:
    // SC_PAGESIZE is usually 4096 bytes so this Align here will most of the time be 4096
    size_t new_size =
        align(block_size + sizeof(struct bucket), sysconf(_SC_PAGESIZE));
    // initialize the bucket to add 
    struct bucket *to_add = mmap(NULL, new_size, PROT_READ | PROT_WRITE,
                                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
     
    if (to_add == MAP_FAILED)
        return NULL;

    // The medatada are align to the block size. Not really efficient when big block size. 
    size_t meta_size = align(sizeof(struct bucket), sizeof(long double));
    // On inscrit les metadonnees
    to_add->next = NULL;
    to_add->prev = NULL;
    to_add->total_size = new_size - meta_size;
    to_add->block_size = align(block_size, sizeof(long double));

    to_add->remain_block = to_add->total_size / to_add->block_size;

    // First usable block are after the metadatas and is the field data of the structure
    void *actu = (unsigned char *)to_add + meta_size;
    
    // From the first block, 
    to_add->data = actu;
    for (size_t ind = 1; ind < (to_add->total_size / to_add->block_size); ind++)
    {
        // On recupere le prochain
        void *nxt = (unsigned char *)actu + to_add->block_size;
        // On l'ajoute a l'actuel
        struct free_list *act = actu;
        act->next = nxt;
        actu = nxt;
    }
    return to_add;
}

/*
 *  Return the size that is aligned with a given alignment
 */
void *bucket_pop(struct bucket *blka)
{
    if (blka == NULL)
        return NULL;
    if (blka->data == NULL)
        return NULL;
    struct free_list *ret = blka->data;
    void *rr = ret->next;
    blka->data = rr;
    blka->remain_block--;
    return ret;
}

/*
 * Add a new block in the freelist
 * If every block are free after, it will unmap the given bucket
 */
void bucket_free_block(struct bucket *blka, struct free_list *fl)
{
    // Save the first block of the freelist
    struct free_list *top = blka->data;
    
    // Prepending the new block to the existing freelist
    fl->next = top;
    blka->data = fl;
   
    // Incrementing remaining block by 1 
    blka->remain_block++;

    // If every blocks are free,
    // we are deleting the bucket
    if (blka->remain_block == blka->total_size / blka->block_size)
    {
        size_t tot_size = align(blka->block_size + sizeof(struct bucket),
                                sysconf(_SC_PAGESIZE));

        struct bucket *next = blka->next;
        struct bucket *prev = blka->prev;
        
        if (next != NULL && prev != NULL)
        {
            prev->next = next;
            next->prev = prev;
        }
        else if (next == NULL && prev != NULL)
        {
            prev->next = NULL;
        }
        else if (next != NULL && prev == NULL)
        {
            starting_point = next;
            next->prev = NULL;
        }
        else if (next == NULL && prev == NULL)
        {
            starting_point = NULL;
        }
        munmap(blka, tot_size);
    }
    return;
}

/*
 * Return the size that is aligned with a given alignment
 */
void *bucket_get_free(struct bucket *blka, size_t block_size)
{
    struct bucket *actu = blka;
    while (actu != NULL)
    {
        // If the block_size matches and there is one ore more remaining blocks  
        if (align(block_size, sizeof(long double)) == actu->block_size
            && actu->remain_block > 0)
        {
            void *to_ret = bucket_pop(actu);
            return to_ret;
        }
        actu = actu->next;
    }

    struct bucket *new = bucket_new(block_size);
    if (new == NULL)
        return NULL;
    struct bucket *top = starting_point;
    top->prev = new;
    new->next = top;
    starting_point = new;
    void *a = bucket_pop(new);

    return a;
}

