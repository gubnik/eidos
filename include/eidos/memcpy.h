#ifndef _EIDOS_MEMCPY_H
#define _EIDOS_MEMCPY_H

#include <stdint.h>

void *eidos_memcpy(void *restrict dest, void const *restrict src, uint32_t size);
void *eidos_memmove(void *dest, void const *src, uint32_t size);

#endif // !_EIDOS_MEMCPY_H
