#include "malloc.h"


void *my_malloc(size_t size)
{
    if (size == 0)
        return NULL;
    if (starting_point == NULL)
    {
        starting_point = bucket_new(size);
    }
    struct bucket *cstst = starting_point;
    void *to_ret = bucket_get_free(cstst, size);

    return to_ret;
}

void *my_calloc(size_t elmcount, size_t elmsize)
{
    if (elmcount == 0 || elmsize == 0)
        return NULL;

    int r = 0;
    if (__builtin_mul_overflow(elmcount, elmsize, &r) == 1)
    {
        errno = ENOMEM;
        return NULL;
    }
    void *new = my_malloc(elmcount * elmsize);
    if (new == NULL)
        return NULL;
    unsigned char *ha = new;
    for (size_t ind = 0; ind < elmcount * elmsize; ind++)
    {
        ha[ind] = 0;
    }
    return new;
}

void my_free(void *ptr)
{
    if (ptr == NULL)
        return;
    struct bucket *bucket = get_begin(ptr);
    bucket_free_block(bucket, ptr);
    return;
}

void *my_realloc(void *ptr, size_t size)
{
    if (ptr == NULL)
    {
        void *res = my_malloc(size);
        return res;
    }
    else
    {
        if (size == 0)
        {
            my_free(ptr);
            return ptr;
        }
    }
    struct bucket *a = get_begin(ptr);
    if (a->block_size >= size)
        return ptr;

    void *new = my_malloc(size);
    if (new == NULL)
        return NULL;
    unsigned char *ha = new;
    unsigned char *hb = ptr;
    for (size_t ind = 0; ind < a->block_size; ind++)
    {
        ha[ind] = hb[ind];
    }
    my_free(hb);
    return ha;
}


/*
 * Functions that will replace the system malloc library's.
 * Explicit visibility as the library will be compiled with -fvisibility=hidden 
 */
__attribute__((visibility("default"))) void *malloc(size_t size)
{
    return my_malloc(size);
}

__attribute__((visibility("default"))) void free(void *ptr)
{
    my_free(ptr);
    return;
}

__attribute__((visibility("default"))) void *realloc(void *ptr, size_t size)
{
    return my_realloc(ptr, size);
}

__attribute__((visibility("default"))) void *calloc(size_t nmemb, size_t size)
{
    return my_calloc(nmemb, size);
}

