#ifndef _EIDOS_ALLOC_H
#define _EIDOS_ALLOC_H

#include <eidos/integers.h>

#define EIDOS_NULL 0

typedef void *(*eidos_malloc_fn)(uint32_t);
typedef void (*eidos_free_fn)(void *);
typedef void *(*eidos_realloc_fn)(void *, uint32_t);

struct eidos_alloc_conf_t
{
    eidos_malloc_fn malloc;
    eidos_free_fn free;
    eidos_realloc_fn realloc;
};

#endif // !_EIDOS_ALLOC_H
