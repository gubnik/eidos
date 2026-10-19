#ifndef _EIDOS_STRING_ARENA_H
#define _EIDOS_STRING_ARENA_H

#include <eidos/alloc.h>
#include <eidos/integers.h>
#include <eidos/string_view.h>

struct eidos_string_arena_t;

struct eidos_string_ref_t
{
    uint32_t offset;
    uint32_t length;
};

struct eidos_string_arena_t *eidos_string_arena_init(struct eidos_alloc_conf_t const *alloc);

void eidos_string_arena_destroy(struct eidos_string_arena_t *arena_ptr);

struct eidos_string_ref_t eidos_string_arena_append(struct eidos_string_arena_t *restrict arena_ptr,
                                                    struct eidos_string_view_t sv);

struct eidos_string_view_t eidos_string_arena_view(struct eidos_string_arena_t *restrict arena_ptr,
                                                   struct eidos_string_ref_t ref);

void eidos_string_arena_merge(struct eidos_string_arena_t *restrict arena_ptr,
                              struct eidos_string_arena_t *restrict dest,
                              struct eidos_string_arena_t const *restrict src);

#endif // !_EIDOS_STRING_ARENA_H
