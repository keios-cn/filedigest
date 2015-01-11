

#ifndef MAIN_
#define MAIN_

#include "types_def.hpp"
#include "digest_common.hpp"
#include <vector>
#include <string>

class SimpleLock
{
public:
    virtual void Lock() = 0;
    virtual void Unlock() = 0;
};

SimpleLock* CreateSimpleLock();

class SimpleCondition
{
public:
    virtual void Wait(SimpleLock*) = 0;
    virtual void Signal() = 0;
    virtual void Broadcast() = 0;
};

SimpleCondition* CreateSimpleCondition();


#endif  /* MAIN_ */

