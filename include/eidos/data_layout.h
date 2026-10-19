#ifndef _EIDOS_DATA_LAYOUT_H
#define _EIDOS_DATA_LAYOUT_H

#include "eidos/alloc.h"
#include "eidos/string_arena.h"

#define EIDOS_INVALID_INDEX ((1ul << 31ul) - 1ul)

struct eidos_alias_type_t;
struct eidos_message_type_t;
struct eidos_variant_type_t;
struct eidos_integer_type_t;
struct eidos_intvalue_type_t;

struct eidos_message_field_t;
struct eidos_variant_option_t;

struct eidos_type_t;
struct eidos_definition_t;

struct eidos_context_t;

struct eidos_context_t *eidos_context_init(struct eidos_alloc_conf_t const *alloc);
void eidos_context_destroy(struct eidos_context_t *ctx);

uint32_t eidos_make_integer_type(struct eidos_context_t *ctx, struct eidos_string_ref_t name_ref,
                                 uint32_t size_bytes);

struct eidos_message_builder_t;
struct eidos_message_builder_t *eidos_begin_message_type(struct eidos_context_t *ctx);
uint32_t eidos_append_message_field(struct eidos_message_builder_t *,
                                    struct eidos_string_ref_t name_ref, uint32_t type_idx);
void eidos_destroy_message_type(struct eidos_message_builder_t *);
uint32_t eidos_end_message_type(struct eidos_message_builder_t const *bldr,
                                struct eidos_context_t *ctx, struct eidos_string_ref_t name_ref);

uint32_t eidos_make_variant_type(struct eidos_context_t *ctx, struct eidos_string_ref_t name_ref,
                                 uint32_t size_bytes);

#endif // !_EIDOS_DATA_LAYOUT_H
