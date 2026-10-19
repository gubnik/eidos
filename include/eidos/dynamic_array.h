#ifndef _EIDOS_ZOZIN_ARRAY_H
#define _EIDOS_ZOZIN_ARRAY_H

#include <eidos/integers.h>
#include <eidos/memcpy.h>

#define MAKE_ARRAY_STRUCT(name_ident, type)                                                        \
    struct name_ident                                                                              \
    {                                                                                              \
        uint32_t capacity;                                                                         \
        uint32_t size;                                                                             \
        type *data;                                                                                \
    }

#define da_append(array, value, alloc)                                                             \
    do                                                                                             \
    {                                                                                              \
        if ((array).size >= (array).capacity)                                                      \
        {                                                                                          \
            if ((array).capacity == 0)                                                             \
            {                                                                                      \
                (array).capacity = 256;                                                            \
            }                                                                                      \
            else                                                                                   \
            {                                                                                      \
                (array).capacity *= 2;                                                             \
                (array).data =                                                                     \
                    (alloc)->realloc((array).data, (array).capacity * sizeof(*(array).data));      \
            }                                                                                      \
            (array).data[(array).size++] = value;                                                  \
        }                                                                                          \
    } while (0)

#define da_append_range(array, source, count, alloc)                                               \
    do                                                                                             \
    {                                                                                              \
        uint32_t _count = (uint32_t)(count);                                                       \
        if (_count != 0)                                                                           \
        {                                                                                          \
            uint32_t _needed = (array).size + _count;                                              \
            if (_needed > (array).capacity)                                                        \
            {                                                                                      \
                uint32_t _new_capacity = (array).capacity ? (array).capacity : 256;                \
                while (_new_capacity < _needed)                                                    \
                    _new_capacity *= 2;                                                            \
                (array).data =                                                                     \
                    (alloc)->realloc((array).data, _new_capacity * sizeof(*(array).data));         \
                (array).capacity = _new_capacity;                                                  \
            }                                                                                      \
            eidos_memcpy((array).data + (array).size, (source), _count * sizeof(*(array).data));   \
            (array).size = _needed;                                                                \
        }                                                                                          \
    } while (0)

#endif // !_EIDOS_ZOZIN_ARRAY_H
