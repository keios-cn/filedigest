
#ifndef DIGEST_COMMON_
#define DIGEST_COMMON_

#include "types_def.hpp"


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
        Assign(res.m_digest, res.m_length);
    }

    ~DigestResult();

    void Assign(uint8* val, size_t len);
};


class Digester
{
public:
    virtual void Initialize() = 0;
    virtual void Update(const void* p, size_t len) = 0;
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
public:
    virtual Digester* CreateDigester() = 0;
    virtual const char* GetDigestName() = 0;

protected:
    DigesterFactory() : m_next(NULL)
    {
        register();
    }

    void register()
    {
        this->m_next = g_head;
        g_head = this;
    }

private:
    static DigesterFactory* g_head;
    DigesterFactory* m_next;

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

    void Update(const void* p, size_t len)
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

