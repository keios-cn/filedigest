
#include "digest_common.hpp"


void
DigestResult::Assign(uint8* val, size_t len)
{
    if (m_digest != NULL)
        delete[] m_digest;
    m_digest = new uint8[len];
    m_length = len;
}

