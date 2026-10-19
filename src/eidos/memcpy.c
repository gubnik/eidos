#include <eidos/memcpy.h>

#if !defined(EIDOS_KILL_LIBC)
#include <string.h>
#endif

void *eidos_memcpy (void *restrict dest, void const *restrict src, uint32_t size)
{
#if !defined(EIDOS_KILL_LIBC)
    return memcpy(dest, src, size);
#else
    uint8_t const *s = src;
    uint8_t *d = dest;
    for (uint32_t i = 0; i < size; i++)
    {
        d[i] = s[i];
    }
    return d;
#endif
}

void *eidos_memmove (void *dest, void const *src, uint32_t size)
{
#if !defined(EIDOS_KILL_LIBC)
    return memmove(dest, src, size);
#else
    uint8_t const *s = src;
    uint8_t *d = dest;
    if (d < s)
    {
        for (uint32_t i = 0; i < size; i++)
        {
            d[i] = s[i];
        }
    }
    else
    {
        for (uint32_t i = size; i > 0; i--)
        {
            d[i - 1] = s[i - 1];
        }
    }
    return dest;
#endif
}
