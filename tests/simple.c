#include <stdio.h>
#include <stdlib.h>


int main(void)
{
    printf("TEST: Simple");
    void *a = malloc(5000);
    void *b = realloc(a, 3000);
    if (b == NULL)
        return 0;
    void *c = realloc(b, 9000);
    free(c);
}
