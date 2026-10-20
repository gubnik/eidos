#ifndef _EIDOS_DATA_LAYOUT_H
#define _EIDOS_DATA_LAYOUT_H

#include "eidos/alloc.h"
#include "eidos/string_arena.h"

#define EIDOS_INVALID_INDEX ((1ul << 31ul) - 1ul)

struct eidos_integer_expr_t;

struct eidos_alias_type_t;
struct eidos_message_type_t;

struct eidos_variant_proto_type_t;
struct eidos_variant_type_t;

struct eidos_integer_type_t;
struct eidos_intvalue_type_t;

struct eidos_message_field_t;
struct eidos_variant_option_t;

struct eidos_type_t;
struct eidos_definition_t;

struct eidos_context_t;

/// BEGIN CONTEXT

struct eidos_context_t *eidos_context_init(struct eidos_alloc_conf_t const *alloc);
void eidos_context_destroy(struct eidos_context_t *ctx);

/// END CONTEXT

/// BEGIN INTEGER TYPE

uint32_t eidos_define_integer_type(struct eidos_context_t *ctx, struct eidos_string_ref_t name_ref,
                                   uint32_t size_bytes, uint8_t properties_mask);
uint8_t eidos_integer_big_endian(struct eidos_context_t const *ctx, uint32_t int_type_def_idx);
uint8_t eidos_integer_signed(struct eidos_context_t const *ctx, uint32_t int_type_def_idx);
uint8_t eidos_integer_size(struct eidos_context_t const *ctx, uint32_t int_type_def_idx);

/// END INTEGER TYPE

/// BEGIN INTEGER EXPR

uint8_t eidos_make_literal_integer_expr(struct eidos_integer_expr_t *int_expr,
                                        uint64_t literal_value);
uint8_t eidos_make_field_integer_expr(struct eidos_context_t const *restrict ctx,
                                      struct eidos_integer_expr_t *restrict int_expr,
                                      uint32_t field_def_idx);
uint8_t eidos_is_integer_field(struct eidos_integer_expr_t const *int_expr);
uint32_t eidos_get_integer_field_idx(struct eidos_integer_expr_t const *int_expr);

/// END INTEGER EXPR

/// BEGIN MESSAGE TYPE

struct eidos_message_builder_t;
struct eidos_message_builder_t *eidos_begin_message_type(struct eidos_context_t *ctx);
uint32_t eidos_append_message_field(struct eidos_message_builder_t *restrict bldr,
                                    struct eidos_string_ref_t name_ref, uint32_t type_idx);
void eidos_destroy_message_type(struct eidos_message_builder_t *restrict bldr);
uint32_t eidos_end_message_type(struct eidos_message_builder_t const *restrict bldr,
                                struct eidos_context_t *restrict ctx,
                                struct eidos_string_ref_t name_ref);

/// END MESSAGE TYPE

/// BEGIN VARIANT PROTO TYPE

struct eidos_proto_variant_builder_t;

struct eidos_proto_variant_builder_t *
eidos_begin_variant_proto_type(struct eidos_context_t *restrict ctx, uint32_t int_type_def_idx);
uint32_t eidos_append_variant_proto_option(struct eidos_proto_variant_builder_t *restrict bldr,
                                           struct eidos_integer_expr_t const *restrict int_expr,
                                           uint32_t type_def_idx);
uint32_t eidos_end_variant_proto_type(struct eidos_proto_variant_builder_t const *restrict bldr,
                                      struct eidos_context_t *restrict ctx,
                                      struct eidos_string_ref_t name_ref);
void eidos_destroy_variant_proto_type(struct eidos_proto_variant_builder_t *bldr);

/// END VARIANT PROTO TYPE

#endif // !_EIDOS_DATA_LAYOUT_H
