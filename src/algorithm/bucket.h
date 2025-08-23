#ifndef BUCKET_H
#define BUCKET_H

#include <stdint.h>
#include <stddef.h>
#include <errno.h>
#include <sys/mman.h>
#include <unistd.h>

#include "../utils/utils.h"

extern void *starting_point;


/*
 * Define the structure of the bucket.
 * 
 *
 *
 *
 *
 */
struct bucket
{
    struct bucket *prev;
    struct bucket *next;
    size_t total_size;
    size_t remain_block;
    size_t block_size;
    void *data;
};

// Liste chainees qui pointe vers les emplacement libre
struct free_list
{
    struct free_list *next;
};

// FONCTIONS

// Creer un nouveau bucket et le renvoie.
struct bucket *bucket_new(size_t block_size);

// Pop et renvoie le premier element libre
// et le pop de la free list
void *bucket_pop(struct bucket *blka);

// Cherche la bucket avec la taille necessaire et de l'espace libre,
// si il n'existe pas, il en creer un avant de le renvoyer.
void *bucket_get_free(struct bucket *blka, size_t block_size);

void bucket_append(struct bucket *blka, struct free_list *fl);

struct bucket *get_begin(void *ptr);

#endif /* ! BUCKET_H */
