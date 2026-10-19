#include "eidos/alloc.h"
#include "eidos/dynamic_array.h"
#include "eidos/string_arena.h"
#include <assert.h>
#include <eidos/data_layout.h>

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

struct eidos_message_field_t
{
    uint32_t type_idx;
};

struct eidos_variant_option_t
{
    uint64_t literal_value;
    uint32_t type_idx;
};

struct eidos_integer_type_t
{
    uint8_t size_bytes;
};

struct eidos_message_type_t
{
    uint32_t fields_begin;
    uint32_t fields_count;
};

struct eidos_varian_type_t
{
    uint32_t options_begin;
    uint32_t options_length;
};

struct eidos_type_t
{
    uint32_t index;
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
MAKE_ARRAY_STRUCT(_variant_types_t, struct eidos_variant_type_t);

MAKE_ARRAY_STRUCT(_message_fields_t, struct eidos_message_field_t);
MAKE_ARRAY_STRUCT(_variant_options_t, struct eidos_variant_option_t);

struct eidos_context_t
{
    struct _types_t types;
    struct _defintions_t defintions;

    struct _integer_types_t integer_types;
    struct _message_types_t message_types;
    struct _variant_types_t variant_types;

    struct _message_fields_t message_fields;
    struct _variant_types_t variant_options;

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

struct eidos_message_builder_t
{
    struct _defintions_t defintions;
    struct _message_fields_t message_fields;

    struct eidos_alloc_conf_t alloc;
};

struct eidos_message_builder_t *eidos_begin_message_type (struct eidos_context_t *ctx)
{
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
    if (bldr == EIDOS_NULL)
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
        (struct eidos_type_t){.index = msg_type_idx, .kind = EIDOS_MESSAGE_TYPE};
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
    return msg_type_idx;
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
