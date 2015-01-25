
#ifndef TYPES_DEF_
#define TYPES_DEF_


#include <assert.h>

#ifdef DEBUG_ALL
#define ASSERT(x) assert(x)
#else
#define ASSERT(x)
#endif /* DEBUG_ALL */


#if ((defined(__GNUC__) && __GNUC__ == 4 && __GNUC_MINOR__ >= 6)) && !defined(__cplusplus)
#define STATIC_ASSERT(description, condition) \
    _Static_assert(condition, #description);
#else
#define STATIC_ASSERT(description, condition) \
    typedef char __VIOLATED_ASSERTION_THAT__##description[1-2*!(condition)];
#endif



typedef char            int8;
typedef unsigned char  uint8;
typedef unsigned char     u8;
typedef short           int16;
typedef unsigned short uint16;

typedef int             int32;
typedef unsigned int   uint32;

typedef long long       int64;
typedef unsigned long long uint64;
typedef unsigned long long ulong64;


#endif /* TYPES_DEF_ */

