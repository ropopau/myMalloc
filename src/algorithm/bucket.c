#include "bucket.h"


void *starting_point = NULL;


struct bucket *get_begin(void *ptr)
{
    size_t newptr = (size_t)ptr;

    uintptr_t ps = sysconf(_SC_PAGESIZE) - 1;
    uintptr_t buck = (uintptr_t)newptr & ~ps;
    // On retrouve le debut de la page actuelle
    struct bucket *a = (struct bucket *)buck;
    return a;
}

// BEGIN BUCKET
struct bucket *bucket_new(size_t block_size)
{
    // Taille finale de la page de mempore
    size_t new_size =
        align(block_size + sizeof(struct bucket), sysconf(_SC_PAGESIZE));
    struct bucket *to_add = mmap(NULL, new_size, PROT_READ | PROT_WRITE,
                                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (to_add == MAP_FAILED)
        return NULL;

    // Size of bucket's metadata
    size_t meta_size = align(sizeof(struct bucket), sizeof(long double));
    // On inscrit les metadonnees
    to_add->next = NULL;
    to_add->prev = NULL;
    to_add->total_size = new_size - meta_size;
    to_add->block_size = align(block_size, sizeof(long double));

    to_add->remain_block = to_add->total_size / to_add->block_size;

    // Premier bloc
    void *actu = (unsigned char *)to_add + meta_size;
    // On l'ajoute au champs data
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

// On ajoute dans la free list
void bucket_append(struct bucket *blka, struct free_list *fl)
{
    // On sauvegarde la premiere adresse libre
    struct free_list *top = blka->data;
    // le suivant de la free a insere est le top au dessus
    fl->next = top;
    // Le top est la nouvelle free
    blka->data = fl;
    // On incremente de 1 le nombre de bloc restant
    blka->remain_block++;
    // Si tout est libre apres le free.
    //
    if (blka->remain_block == blka->total_size / blka->block_size)
    {
        // On recupere le size de la page
        size_t tot_size = align(blka->block_size + sizeof(struct bucket),
                                sysconf(_SC_PAGESIZE));

        // On recupere le next
        struct bucket *next = blka->next;
        struct bucket *prev = blka->prev;
        // Si le bucket actuelle est en plein milieu
        if (next != NULL && prev != NULL)
        {
            // le suivant du precedent devient le suivant de l'actuel
            prev->next = next;
            // le precedent du suivant deviens le precedent de l'actuel
            next->prev = prev;
        }
        // Si le bucket est a la fin
        else if (next == NULL && prev != NULL)
        {
            prev->next = NULL;
        }
        // Si le bucket est au debut
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

// Renvoie le premier bloc adapte
void *bucket_get_free(struct bucket *blka, size_t block_size)
{
    struct bucket *actu = blka;
    // On cherche
    while (actu != NULL)
    {
        if (align(block_size, sizeof(long double)) == actu->block_size
            && actu->remain_block > 0)
        {
            void *to_ret = bucket_pop(actu);
            return to_ret;
        }
        actu = actu->next;
    }
    // On creer un nouveau bloc et on le renvoie et on le rattache au bucket
    // initial

    // Nouveau bloc
    struct bucket *new = bucket_new(block_size);
    if (new == NULL)
        return NULL;
    // On le prepend au premier bucket
    struct bucket *top = starting_point;
    top->prev = new;
    new->next = top;
    starting_point = new;
    // On renvoie le premier libre de ce nouveau bloc
    void *a = bucket_pop(new);

    return a;
}

