
#ifndef DIGEST_COMMON_
#define DIGEST_COMMON_

#include "types_def.hpp"
#include <stdlib.h>
#include <string.h>
#include <string>


void dbg_printf(const char* fmt, ... );

class DigestResult
{
private:
    uint8* m_digest;
    size_t m_length;

public:
    DigestResult() : m_digest(NULL), m_length(0)
    {}

    DigestResult(const DigestResult& res) : m_digest(NULL), m_length(0)
    {
        assign(res.m_digest, res.m_length);
    }

    ~DigestResult();

    void assign(const uint8* val, size_t len);

    bool hasResult()
    {
        return (m_digest != NULL && m_length != 0);
    }

    static void ByteToHexString(uint8 by, char* outStr)
    {
        static char HEX_CHAR[17] = "0123456789ABCDEF";

        outStr[0] = HEX_CHAR[ (by >> 4) & 0x0F ];
        outStr[1] = HEX_CHAR[ by & 0x0F ];
        outStr[2] = '\0';
    }

    void toString(std::string& outStr)
    {
        ASSERT(hasResult());

        outStr = "";
        outStr.reserve(m_length * 2);
        for (uint8* p = m_digest; p < m_digest + m_length; ++p)
        {
            char hexStr[4];
            ByteToHexString( *p, hexStr );

            outStr += hexStr;
        }
    }
};


class Digester
{
public:
    virtual void Initialize() = 0;
    virtual void Update(const u8* p, size_t len) = 0;
    virtual void Finish() = 0;
    virtual void GetDigest(DigestResult& result) = 0;

    virtual ~Digester()
    {}

    const char* GetName() const
    {
        return m_name;
    }

    size_t GetDigestLength() const
    {
        return m_digestLength;
    }

protected:

    Digester(const char* name,
            size_t digLen): m_name(name), m_digestLength(digLen)
    {}

private:
    const char* const m_name;
    size_t m_digestLength;
};


class DigesterFactory
{
private:
    static DigesterFactory* g_head;  // Global linked list head
    DigesterFactory* m_next;

public:
    virtual Digester* CreateDigester() = 0;
    virtual const char* GetDigestName() = 0;

protected:
    void doRegister()
    {
        this->m_next = g_head;
        g_head = this;
    }

    DigesterFactory() : m_next(NULL)
    {
        doRegister();
    }

public:
    static DigesterFactory* Find(const char* name)
    {
        DigesterFactory* p = g_head;
        while (p != NULL)
        {
            const char* n = p->GetDigestName();
            if (strcmp(n, name) == 0)
                return p;

            p = p->m_next;
        }
        return NULL;
    }

    static Digester* CreateDigesterByName(const char* name)
    {
        DigesterFactory* factory = Find(name);
        if (factory == NULL)
        {
            return NULL;
        }
        return factory->CreateDigester();
    }
};


class DigestSafe
{
public:
    DigestSafe(Digester* dig) : 
                    m_initialized(false), 
                    m_finished(false),
                    m_digest(dig)
    {}

    void Initialize()
    {
        m_digest->Initialize();
        m_initialized = true;
        m_finished = false;
    }

    void Update(const u8* p, size_t len)
    {
        ASSERT(m_initialized);
        ASSERT(!m_finished);
        m_digest->Update(p, len);
    }

    void Finish()
    {
        ASSERT(m_initialized);
        ASSERT(!m_finished);
        m_digest->Finish();
        m_finished = true;
    }

    size_t GetDigestLength() const
    {
        return m_digest->GetDigestLength();
    } 

    void GetDigest(DigestResult& result)
    {
        ASSERT(m_initialized);
        ASSERT(m_finished);
        return m_digest->GetDigest(result);
    }

protected:
    bool m_initialized;
    bool m_finished;
    Digester* m_digest;
};


#endif /* DIGEST_COMMON_ */

