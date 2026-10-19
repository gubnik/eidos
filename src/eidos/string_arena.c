#include "eidos/alloc.h"
#include "eidos/dynamic_array.h"
#include "eidos/string_view.h"
#include <eidos/string_arena.h>

MAKE_ARRAY_STRUCT(eidos_string_arena_impl_t, char);

struct eidos_string_arena_t
{
    struct eidos_string_arena_impl_t impl;
    struct eidos_alloc_conf_t alloc;
};

struct eidos_string_arena_t *eidos_string_arena_init (struct eidos_alloc_conf_t const *alloc)
{
    struct eidos_string_arena_t *arena = alloc->malloc(sizeof(struct eidos_string_arena_t));
    if (arena == EIDOS_NULL)
    {
        return EIDOS_NULL;
    }
    struct eidos_string_arena_impl_t impl = {0};
    arena->impl = impl;
    arena->alloc = *alloc;
    return arena;
}

void eidos_string_arena_destroy (struct eidos_string_arena_t *arena_ptr)
{
    arena_ptr->alloc.free(arena_ptr);
}

struct eidos_string_ref_t
eidos_string_arena_append (struct eidos_string_arena_t *restrict arena_ptr,
                           struct eidos_string_view_t sv)
{
    uint32_t const begin = arena_ptr->impl.size;
    da_append_range(arena_ptr->impl, sv.data, sv.size, &arena_ptr->alloc);
    uint32_t const end = arena_ptr->impl.size;
    return (struct eidos_string_ref_t){.offset = begin, .length = (end - begin)};
}

struct eidos_string_view_t eidos_string_arena_view (struct eidos_string_arena_t *restrict arena_ptr,
                                                    struct eidos_string_ref_t ref)
{
    return (struct eidos_string_view_t){.data = arena_ptr->impl.data + ref.offset,
                                        .size = ref.length};
}

void eidos_string_arena_merge (struct eidos_string_arena_t *restrict arena_ptr,
                               struct eidos_string_arena_t *restrict dest,
                               struct eidos_string_arena_t const *restrict src)
{
}
