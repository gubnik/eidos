#ifndef _EIDOS_STRING_VIEW_H
#define _EIDOS_STRING_VIEW_H

#include <eidos/integers.h>

struct eidos_string_view_t
{
    char *data;
    uint32_t size;
};

#define EIDOS_SV(strlit)                                                                           \
    (struct eidos_string_view_t)                                                                   \
    {                                                                                              \
        .data = (strlit), .size = sizeof((strlit))                                                 \
    }

#endif // !_EIDOS_STRING_VIEW_H
