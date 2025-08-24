#ifndef BUCKET_H
#define BUCKET_H

#include <stdint.h>
#include <stddef.h>
#include <errno.h>
#include <sys/mman.h>
#include <unistd.h>

#include "../utils/utils.h"


// As buckets are chained all together, the first element of the chained free_list
// must be saved here
extern void *starting_point;

/*
 * A bucket is a memory space with a given size.
 */
struct bucket
{
    // Next and previous buckets
    struct bucket *prev;
    struct bucket *next;
    // Size of the bucket     
    size_t total_size;
    // Remaining block 
    size_t remain_block;
    // Size of a block size
    size_t block_size;
    // Starting point of the data.  
    void *data;
};

// Freelist to keep track of memory space so it can be easier to free it.
struct free_list
{
    struct free_list *next;
};

// Functions
struct bucket *bucket_new(size_t block_size);
void *bucket_pop(struct bucket *blka);
void *bucket_get_free(struct bucket *blka, size_t block_size);
void bucket_free_block(struct bucket *blka, struct free_list *fl);
struct bucket *get_begin(void *ptr);

#endif /* ! BUCKET_H */
