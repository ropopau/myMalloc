#include "utils.h"


/*
 * @brief Return the size that is aligned with a given alignment
 * 
 * @param size (size_t) Size that must be aligned
 * @param alignment (size_t) Alignment to use
 * @return size_t Aligned size
 */
size_t align(size_t size, size_t alignment)
{
    size_t factor = (size % alignment == 0) ? size / alignment : size / alignment + 1;

    size_t result = 0;
    if (__builtin_mul_overflow(alignment, factor, &result))
        return 0;

    return result;
}

