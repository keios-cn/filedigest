
#include "digest_common.hpp"


void
DigestResult::Assign(uint8* val, size_t len)
{
    if (m_digest != NULL)
    {
        delete[] m_digest;
        m_length = 0;
    }

    if (val != NULL && len > 0)
    {
        m_digest = new uint8[len];
        m_length = len;
        memcpy(m_digest, val, len);
    }
}

DigestResult::~DigestResult()
{
    if (m_digest != NULL)
        delete[] m_digest;
}


DigesterFactory* DigesterFactory::g_head = NULL;

