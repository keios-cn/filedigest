
#ifndef DIGEST_COMMON_
#define DIGEST_COMMON_

#include "types_def.hpp"

class Digester
{
public:
    virtual void Initialize() = 0;
    virtual void Update(const void* p, size_t len) = 0;
    virtual void Finish() = 0;
    virtual bool GetDigest(void* p, size_t& len) = 0;

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
        this->m_next = head;
        head = this;
    }

private:
    static DigesterFactory* head;
    DigesterFactory* m_next;

public:
    static DigesterFactory* Find(const char* name)
    {
        DigesterFactory* p = head;
        while (p != NULL)
        {
            const char* n = p->GetDigestName();
            if (strcmp(n, name) == 0)
                return p;
        }
        return NULL;
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

    bool GetDigest(void* p, size_t& len)
    {
        ASSERT(m_initialized);
        ASSERT(m_finished);
        return m_digest->GetDigest(p, len);
    }

protected:
    bool m_initialized;
    bool m_finished;
    Digester* m_digest;
};


#endif /* DIGEST_COMMON_ */

