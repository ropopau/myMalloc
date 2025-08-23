#include "malloc.h"

void *my_malloc(size_t size)
{
    if (size == 0)
        return NULL;
    // Si pas d'allocator initialise...
    if (starting_point == NULL)
    {
        starting_point = bucket_new(size);
    }
    // Sinon On parcours toute les pages pour verifier les memoires disponibles
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
    // Retourner le bucket dans lequel ptr est presen
    struct bucket *a = get_begin(ptr);
    // On l'ajoute dans la free list
    bucket_append(a, ptr);
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
    // Retourner le bucket dans lequel ptr est presen
    struct bucket *a = get_begin(ptr);
    if (a->block_size >= size)
        return ptr;

    // On trouve une nouvelle zone memoire de nouvelle taille.
    void *new = my_malloc(size);
    if (new == NULL)
        return NULL;
    // Nouvelle adresse
    unsigned char *ha = new;
    // Ancienne adresse
    unsigned char *hb = ptr;
    // On copie tout les donnée de l'ancienne vers la nouvelle
    for (size_t ind = 0; ind < a->block_size; ind++)
    {
        ha[ind] = hb[ind];
    }
    my_free(hb);
    return ha;
}


/*
 * Functions that will replace the system malloc library's.
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

