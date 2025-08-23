#include "utils.h"


size_t align(size_t size, size_t alignment)
{
    size_t factor = (size % alignment == 0) ? size / alignment : size / alignment + 1;

    size_t result = 0;
    if (__builtin_mul_overflow(alignment, factor, &result))
        return 0;

    return result;
}


