
#ifndef TYPES_DEF_
#define TYPES_DEF_


#include <assert.h>

#ifdef DEBUG_ALL
#define ASSERT(x) assert(x)
#else
#define ASSERT(x)
#endif /* DEBUG_ALL */

typedef char            int8;
typedef unsigned char  uint8;
typedef short           int16;
typedef unsigned short uint16;

typedef int             int32;
typedef unsigned int   uint32;

typedef long long       int64;
typedef unsigned long long uint64;


#endif /* TYPES_DEF_ */

