#include <eidos/alloc.h>
#include <stdint.h>
#include <stdlib.h>

void *_eidos_libc_malloc (uint32_t size)
{
    return malloc((size_t)size);
}

void _eidos_libc_free (void *mem)
{
    return free(mem);
}

void *_eidos_libc_realloc (void *mem, uint32_t size)
{
    return realloc(mem, (size_t)size);
}

void eidos_stdalloc (struct eidos_alloc_conf_t *out_alloc)
{
    out_alloc->malloc = _eidos_libc_malloc;
    out_alloc->free = _eidos_libc_free;
    out_alloc->realloc = _eidos_libc_realloc;
}
