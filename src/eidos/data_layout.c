#include "eidos/alloc.h"
#include "eidos/dynamic_array.h"
#include "eidos/string_arena.h"
#include <assert.h>
#include <eidos/data_layout.h>
#include <stdint.h>

#define EIDOS_FIELD_INTEGER_MASK (1ull << 63ull)

struct eidos_integer_expr_t
{
    union
    {
        uint64_t field_def_idx;
        uint64_t literal;
        uint64_t _raw;
    } as;
};

/**
 * ```
 * define %type-ident: alias = %type-ident
 * ```
 */
#define EIDOS_ALIAS_TYPE 0x00

/**
 * ```
 * define %type-ident: message = { %message-body }
 * ```
 */
#define EIDOS_MESSAGE_TYPE 0x01

/**
 * ```
 * define %type-ident: variant by %int-type-ident = { %variant-body }
 * ```
 */
#define EIDOS_VARIANT_TYPE 0x02
#define EIDOS_INTEGER_TYPE 0x03

struct eidos_message_field_t
{
    uint32_t type_idx;
};

struct eidos_variant_option_t
{
    struct eidos_integer_expr_t int_expr;
    uint32_t type_idx;
};

#define EIDOS_INTEGER_SIZE_MASK 0x3F
#define EIDOS_INTEGER_BIG_ENDIAN_MASK 0x40
#define EIDOS_INTEGER_SIGNED_MASK 0x80

struct eidos_integer_type_t
{
    uint8_t size_bytes;
};

struct eidos_message_type_t
{
    uint32_t fields_begin;
    uint32_t fields_count;
};

struct eidos_variant_proto_type_t
{
    uint32_t integer_type_idx;
    uint32_t options_begin;
    uint32_t options_count;
};

struct eidos_variant_type_t
{
    uint32_t prototype_idx;
    uint32_t field_idx;
};

struct eidos_type_t
{
    uint32_t value_index;
    uint8_t kind;
};

#define EIDOS_TYPE_DEFINITION 0x00
#define EIDOS_FIELD_DEFINITION 0x01
#define EIDOS_VARIANT_PROTO_DEFINITION 0x02
#define EIDOS_CONSTANT_DEFINITION 0x02

struct eidos_definition_t
{
    struct eidos_string_ref_t name_ref;
    uint32_t value_idx;
    uint32_t kind;
};

MAKE_ARRAY_STRUCT(_types_t, struct eidos_type_t);
MAKE_ARRAY_STRUCT(_defintions_t, struct eidos_definition_t);

MAKE_ARRAY_STRUCT(_integer_types_t, struct eidos_integer_type_t);
MAKE_ARRAY_STRUCT(_message_types_t, struct eidos_message_type_t);
MAKE_ARRAY_STRUCT(_variant_proto_types_t, struct eidos_variant_proto_type_t);
MAKE_ARRAY_STRUCT(_variant_types_t, struct eidos_variant_type_t);

MAKE_ARRAY_STRUCT(_message_fields_t, struct eidos_message_field_t);
MAKE_ARRAY_STRUCT(_variant_options_t, struct eidos_variant_option_t);

struct eidos_context_t
{
    struct _types_t types;
    struct _defintions_t defintions;

    struct _integer_types_t integer_types;
    struct _message_types_t message_types;
    struct _variant_proto_types_t variant_proto_types;
    struct _variant_types_t variant_types;

    struct _message_fields_t message_fields;
    struct _variant_options_t variant_options;

    struct eidos_alloc_conf_t alloc;
    struct eidos_string_arena_t *arena;
};

struct eidos_context_t *eidos_context_init (struct eidos_alloc_conf_t const *alloc)
{
    struct eidos_context_t *ctx = alloc->malloc(sizeof(struct eidos_context_t));
    if (ctx == EIDOS_NULL)
    {
        return EIDOS_NULL;
    }
    ctx->arena = eidos_string_arena_init(alloc);
    if (ctx->arena == EIDOS_NULL)
    {
        alloc->free(ctx);
        return EIDOS_NULL;
    }
    ctx->alloc = *alloc;
    ctx->types = (struct _types_t){0};
    ctx->defintions = (struct _defintions_t){0};
    ctx->integer_types = (struct _integer_types_t){0};
    ctx->message_types = (struct _message_types_t){0};
    ctx->variant_proto_types = (struct _variant_proto_types_t){0};
    ctx->variant_types = (struct _variant_types_t){0};
    ctx->message_fields = (struct _message_fields_t){0};
    ctx->variant_options = (struct _variant_options_t){0};
    return ctx;
}

void eidos_context_destroy (struct eidos_context_t *ctx)
{
    if (ctx == EIDOS_NULL)
    {
        return;
    }
    if (ctx->arena != EIDOS_NULL)
    {
        eidos_string_arena_destroy(ctx->arena);
    }
    ctx->alloc.free(ctx);
}

uint32_t eidos_define_integer_type (struct eidos_context_t *ctx, struct eidos_string_ref_t name_ref,
                                    uint32_t size_bytes, uint8_t properties_mask)
{
    if (ctx == EIDOS_NULL)
    {
        return EIDOS_INVALID_INDEX;
    }
    if (size_bytes > EIDOS_INTEGER_BIG_ENDIAN_MASK)
    {
        return EIDOS_INVALID_INDEX;
    }
    uint32_t const integer_type_idx = ctx->integer_types.size;
    struct eidos_integer_type_t int_type = {.size_bytes = size_bytes};
    if (properties_mask & EIDOS_INTEGER_SIGNED_MASK)
    {
        int_type.size_bytes |= EIDOS_INTEGER_SIGNED_MASK;
    }
    if (properties_mask & EIDOS_INTEGER_BIG_ENDIAN_MASK)
    {
        int_type.size_bytes |= EIDOS_INTEGER_BIG_ENDIAN_MASK;
    }
    da_append(ctx->integer_types, int_type, &ctx->alloc);
    if (integer_type_idx == ctx->integer_types.size)
    {
        return EIDOS_INVALID_INDEX;
    }
    uint32_t const type_type_idx = ctx->types.size;
    da_append(ctx->types,
              ((struct eidos_type_t){.value_index = type_type_idx, .kind = EIDOS_INTEGER_TYPE}),
              &ctx->alloc);
    if (type_type_idx == ctx->types.size)
    {
        ctx->integer_types.size = integer_type_idx;
        return EIDOS_INVALID_INDEX;
    }
    uint32_t const intty_def_idx = ctx->defintions.size;
    struct eidos_definition_t const def = {
        .name_ref = name_ref,
        .value_idx = type_type_idx,
        .kind = EIDOS_TYPE_DEFINITION,
    };
    da_append(ctx->defintions, def, &ctx->alloc);
    if (intty_def_idx == ctx->defintions.size)
    {
        ctx->types.size = type_type_idx;
        ctx->integer_types.size = integer_type_idx;
        return EIDOS_INVALID_INDEX;
    }
    return intty_def_idx;
}

uint8_t eidos_integer_big_endian (struct eidos_context_t const *ctx, uint32_t int_type_def_idx)
{
    if (int_type_def_idx >= ctx->integer_types.size)
    {
        return EIDOS_NULL;
    }
    struct eidos_integer_type_t const int_type = ctx->integer_types.data[int_type_def_idx];
    return (int_type.size_bytes & EIDOS_INTEGER_BIG_ENDIAN_MASK);
}

uint8_t eidos_integer_signed (struct eidos_context_t const *ctx, uint32_t int_type_def_idx)
{
    if (int_type_def_idx >= ctx->integer_types.size)
    {
        return EIDOS_NULL;
    }
    struct eidos_integer_type_t const int_type = ctx->integer_types.data[int_type_def_idx];
    return (int_type.size_bytes & EIDOS_INTEGER_SIGNED_MASK);
}

uint8_t eidos_integer_size (struct eidos_context_t const *ctx, uint32_t int_type_def_idx)
{
    if (int_type_def_idx >= ctx->integer_types.size)
    {
        return EIDOS_NULL;
    }
    struct eidos_integer_type_t const int_type = ctx->integer_types.data[int_type_def_idx];
    return (int_type.size_bytes & ~EIDOS_INTEGER_SIZE_MASK);
}

uint8_t eidos_make_literal_integer_expr (struct eidos_integer_expr_t *int_expr,
                                         uint64_t literal_value)
{
    if (literal_value & EIDOS_FIELD_INTEGER_MASK)
    {
        return EIDOS_NULL;
    }
    int_expr->as.literal = literal_value;
    return 1;
}

uint8_t eidos_make_field_integer_expr (struct eidos_context_t const *ctx,
                                       struct eidos_integer_expr_t *int_expr,
                                       uint32_t field_def_idx)
{
    if (field_def_idx >= ctx->defintions.size)
    {
        return EIDOS_NULL;
    }
    struct eidos_definition_t const def_view = ctx->defintions.data[field_def_idx];
    if (def_view.kind != EIDOS_FIELD_DEFINITION)
    {
        return EIDOS_NULL;
    }
    int_expr->as.field_def_idx = field_def_idx;
    int_expr->as.field_def_idx |= EIDOS_FIELD_INTEGER_MASK;
    return 1;
}

uint8_t eidos_is_integer_field (struct eidos_integer_expr_t const *int_expr)
{
    return ((EIDOS_FIELD_INTEGER_MASK & (int_expr->as._raw)) == 0ull);
}

uint32_t eidos_get_integer_field_idx (struct eidos_integer_expr_t const *int_expr)
{
    return (int_expr->as.field_def_idx & ~EIDOS_FIELD_INTEGER_MASK);
}

struct eidos_message_builder_t
{
    struct _defintions_t defintions;
    struct _message_fields_t message_fields;

    struct eidos_alloc_conf_t alloc;
};

struct eidos_message_builder_t *eidos_begin_message_type (struct eidos_context_t *ctx)
{
    if (ctx == EIDOS_NULL)
    {
        return EIDOS_NULL;
    }
    struct eidos_message_builder_t *bldr =
        ctx->alloc.malloc(sizeof(struct eidos_message_builder_t));
    if (bldr == EIDOS_NULL)
    {
        return EIDOS_NULL;
    }
    struct _defintions_t defintions = {0};
    struct _message_fields_t message_fields = {0};
    bldr->defintions = defintions;
    bldr->message_fields = message_fields;
    bldr->alloc = ctx->alloc;
    return bldr;
}

uint32_t eidos_append_message_field (struct eidos_message_builder_t *bldr,
                                     struct eidos_string_ref_t name_ref, uint32_t type_idx)
{
    if (bldr == EIDOS_NULL)
    {
        return EIDOS_INVALID_INDEX;
    }
    uint32_t const field_idx = bldr->message_fields.size;
    da_append(bldr->message_fields, (struct eidos_message_field_t){.type_idx = type_idx},
              &bldr->alloc);
    if (field_idx == bldr->message_fields.size)
    {
        return EIDOS_INVALID_INDEX;
    }
    uint32_t const definition_idx = bldr->defintions.size;
    {
        struct eidos_definition_t def = (struct eidos_definition_t){
            .name_ref = name_ref, .value_idx = field_idx, .kind = EIDOS_FIELD_DEFINITION};
        da_append(bldr->defintions, def, &bldr->alloc);
    }
    if (definition_idx == bldr->defintions.size)
    {
        bldr->message_fields.size = field_idx;
        return EIDOS_INVALID_INDEX;
    }
    return definition_idx;
}

uint32_t eidos_end_message_type (struct eidos_message_builder_t const *bldr,
                                 struct eidos_context_t *ctx, struct eidos_string_ref_t name_ref)
{
    if (bldr == EIDOS_NULL || ctx == EIDOS_NULL)
    {
        return EIDOS_INVALID_INDEX;
    }
    assert(bldr->message_fields.size == bldr->defintions.size);
    uint32_t const orig_ctx_msgflds_size = ctx->message_fields.size;
    uint32_t const orig_ctx_def_size = ctx->defintions.size;

    uint32_t const fields_count = bldr->message_fields.size;
    uint32_t const fields_begin = ctx->message_fields.size;
    for (uint32_t i = 0; i < bldr->message_fields.size; i++)
    {
        uint32_t const new_field_idx = ctx->message_fields.size;
        da_append(ctx->message_fields, bldr->message_fields.data[i], &ctx->alloc);
        if (new_field_idx == ctx->message_fields.size)
        {
            ctx->message_fields.size = orig_ctx_msgflds_size;
            ctx->defintions.size = orig_ctx_def_size;
            return EIDOS_INVALID_INDEX;
        }
        struct eidos_definition_t new_def = bldr->defintions.data[i];
        new_def.value_idx = new_field_idx;
        uint32_t const new_defintion_idx = ctx->defintions.size;
        da_append(ctx->defintions, new_def, &ctx->alloc);
        if (new_defintion_idx == ctx->defintions.size)
        {
            ctx->message_fields.size = orig_ctx_msgflds_size;
            ctx->defintions.size = orig_ctx_def_size;
            return EIDOS_INVALID_INDEX;
        }
    }

    assert(ctx->message_fields.size == fields_count + fields_begin);
    struct eidos_message_type_t msg_type = {0};
    msg_type.fields_begin = fields_begin;
    msg_type.fields_count = fields_count;
    uint32_t const msg_type_idx = ctx->message_types.size;
    da_append(ctx->message_types, msg_type, &ctx->alloc);
    if (msg_type_idx == ctx->message_types.size)
    {
        ctx->message_fields.size = orig_ctx_msgflds_size;
        ctx->defintions.size = orig_ctx_def_size;
        return EIDOS_INVALID_INDEX;
    }
    uint32_t const msg_type_type_idx = ctx->types.size;
    struct eidos_type_t const msg_type_type =
        (struct eidos_type_t){.value_index = msg_type_idx, .kind = EIDOS_MESSAGE_TYPE};
    da_append(ctx->types, msg_type_type, &ctx->alloc);
    if (msg_type_type_idx == ctx->types.size)
    {
        ctx->message_types.size = msg_type_idx;
        ctx->message_fields.size = orig_ctx_msgflds_size;
        ctx->defintions.size = orig_ctx_def_size;
        return EIDOS_INVALID_INDEX;
    }
    struct eidos_definition_t msg_def = {
        .name_ref = name_ref,
        .value_idx = msg_type_idx,
        .kind = EIDOS_TYPE_DEFINITION,
    };
    uint32_t const msg_type_def_idx = ctx->defintions.size;
    da_append(ctx->defintions, msg_def, &ctx->alloc);
    if (msg_type_def_idx == ctx->defintions.size)
    {
        ctx->types.size = msg_type_type_idx;
        ctx->message_types.size = msg_type_idx;
        ctx->message_fields.size = orig_ctx_msgflds_size;
        ctx->defintions.size = orig_ctx_def_size;
        return EIDOS_INVALID_INDEX;
    }
    return msg_type_def_idx;
}

void eidos_destroy_message_type (struct eidos_message_builder_t *bldr)
{
    if (bldr == EIDOS_NULL)
    {
        return;
    }
    bldr->alloc.free(bldr->message_fields.data);
    bldr->alloc.free(bldr->defintions.data);
    bldr->alloc.free(bldr);
}

struct eidos_proto_variant_builder_t
{
    struct _variant_options_t variant_options;
    struct eidos_alloc_conf_t alloc;
    struct eidos_context_t const *ctx_view;
    uint32_t int_type_idx;
};

struct eidos_proto_variant_builder_t *eidos_begin_variant_proto_type (struct eidos_context_t *ctx,
                                                                      uint32_t int_type_def_idx)
{
    if (ctx == EIDOS_NULL)
    {
        return EIDOS_NULL;
    }
    struct eidos_proto_variant_builder_t *bldr =
        ctx->alloc.malloc(sizeof(struct eidos_proto_variant_builder_t));
    if (bldr == EIDOS_NULL)
    {
        return EIDOS_NULL;
    }
    bldr->variant_options = (struct _variant_options_t){0};
    bldr->alloc = ctx->alloc;

    struct eidos_definition_t const def = ctx->defintions.data[int_type_def_idx];
    if (def.kind != EIDOS_TYPE_DEFINITION)
    {
        eidos_destroy_variant_proto_type(bldr);
        return EIDOS_NULL;
    }
    uint32_t const type_idx = def.value_idx;
    struct eidos_type_t const type = ctx->types.data[type_idx];
    if (type.kind != EIDOS_INTEGER_TYPE)
    {
        eidos_destroy_variant_proto_type(bldr);
        return EIDOS_NULL;
    }
    bldr->ctx_view = ctx;
    bldr->int_type_idx = type.value_index;
    return bldr;
}

uint32_t eidos_append_variant_proto_option (struct eidos_proto_variant_builder_t *bldr,
                                            struct eidos_integer_expr_t const *int_expr,
                                            uint32_t type_idx)
{
    if (bldr == EIDOS_NULL)
    {
        return EIDOS_INVALID_INDEX;
    }

    uint8_t const int_bytes = eidos_integer_size(bldr->ctx_view, bldr->int_type_idx);
    uint64_t const max_value = ((1ull << (int_bytes * 8)) - 1ull);
    if (int_bytes != 0)
    {
        if (!(int_expr->as._raw & EIDOS_FIELD_INTEGER_MASK) && int_expr->as.literal > max_value)
        {
            return EIDOS_INVALID_INDEX;
        }
    }

    uint32_t const option_idx = bldr->variant_options.size;
    da_append(bldr->variant_options,
              ((struct eidos_variant_option_t){.int_expr = *int_expr, .type_idx = type_idx}),
              &bldr->alloc);
    if (option_idx == bldr->variant_options.size)
    {
        return EIDOS_INVALID_INDEX;
    }
    return option_idx;
}

uint32_t eidos_end_variant_proto_type (struct eidos_proto_variant_builder_t const *bldr,
                                       struct eidos_context_t *ctx,
                                       struct eidos_string_ref_t name_ref)
{
    if (bldr == EIDOS_NULL || ctx == EIDOS_NULL)
    {
        return EIDOS_INVALID_INDEX;
    }
    uint32_t const orig_ctx_varopt_size = ctx->variant_options.size;

    uint32_t const options_begin = bldr->variant_options.size;
    uint32_t const options_count = ctx->variant_options.size;
    for (uint32_t i = 0; i < bldr->variant_options.size; i++)
    {
        uint32_t const new_field_idx = ctx->variant_options.size;
        da_append(ctx->variant_options, bldr->variant_options.data[i], &ctx->alloc);
        if (new_field_idx == ctx->variant_options.size)
        {
            ctx->variant_options.size = orig_ctx_varopt_size;
            return EIDOS_INVALID_INDEX;
        }
    }
    assert(ctx->variant_options.size == options_begin + options_count);
    struct eidos_variant_proto_type_t proto = {
        .integer_type_idx = bldr->int_type_idx,
        .options_begin = options_begin,
        .options_count = options_count,
    };
    uint32_t const proto_idx = ctx->variant_proto_types.size;
    da_append(ctx->variant_proto_types, proto, &ctx->alloc);
    if (proto_idx == ctx->variant_proto_types.size)
    {
        ctx->variant_options.size = orig_ctx_varopt_size;
        return EIDOS_INVALID_INDEX;
    }
    struct eidos_definition_t const def = {
        .name_ref = name_ref,
        .value_idx = proto_idx,
        .kind = EIDOS_VARIANT_PROTO_DEFINITION,
    };
    uint32_t const proto_def_idx = ctx->variant_proto_types.size;
    da_append(ctx->defintions, def, &ctx->alloc);
    if (proto_def_idx == ctx->defintions.size)
    {
        ctx->variant_proto_types.size = proto_idx;
        ctx->variant_options.size = orig_ctx_varopt_size;
        return EIDOS_INVALID_INDEX;
    }
    return proto_def_idx;
}
