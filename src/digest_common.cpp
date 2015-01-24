
#include "digest_common.hpp"


#include <stdio.h>
#include <stdarg.h>


void dbg_printf(const char* fmt, ... )
{
#ifdef DEBUG_ALL
    va_list args;
    va_start (args, fmt);
    vprintf (fmt, args);
    va_end (args);
#endif
}


void
DigestResult::assign(const uint8* val, size_t len)
{
    if (val == NULL || len == 0)
    {
        if (m_digest != NULL)
        {
            delete[] m_digest;
            m_digest = NULL;
        }
        m_length = 0;
        return;
    }

    if (m_digest != NULL && len > m_length)
    {
        delete[] m_digest;
        m_digest = NULL;
    }

    if (m_digest == NULL)
    {
        m_digest = new uint8[len];
    }

    memcpy(m_digest, val, len);
    m_length = len;
}

DigestResult::~DigestResult()
{
    if (m_digest != NULL)
        delete[] m_digest;
}


DigesterFactory* DigesterFactory::g_head = NULL;

