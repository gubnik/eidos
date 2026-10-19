#ifndef _EIDOS_INTEGERS_H
#define _EIDOS_INTEGERS_H

#if !defined(EIDOS_KILL_LIBC)
#include <stdint.h>
#else

typedef unsigned long uint64_t;
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

#endif

#endif // !_EIDOS_INTEGERS_H
