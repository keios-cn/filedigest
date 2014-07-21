
#ifndef DIGEST_COMMON_
#define DIGEST_COMMON_


class Digester
{
public:
    virtual void Initialize() = 0;
    virtual void Update(const void* p, size_t len) = 0;
    virtual void Finish() = 0;
    virtual size_t GetDigestLength() = 0;
    virtual bool GetDigest(void* p, size_t& len) = 0;

    virtual ~Digester()
    {}

    const char* GetName()
    {
        return m_name;
    }

protected:

    Digester(): m_name("")
    {}

    const char* m_name;
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
    DigestSafe(DigestInterface* dig) : 
                    m_initilized(false), 
                    m_finished(false),
                    m_digest(dig)
    {}

    void Initialize()
    {
        m_digest->Initialize();
        m_initilized = true;
    }

    void Update(const void* p, size_t len)
    {
        ASSERT(m_initilized);
        m_digest->Update(p, len);
    }

    void Finish()
    {
        ASSERT(m_initilized);
        m_digest->Finish();
        m_finished = true;
    }

    size_t GetDigestLength()
    {
        return m_digest->GetDigestLength();
    } 

    bool GetDigest(void* p, size_t& len)
    {
        ASSERT(m_initilized);
        ASSERT(m_finished);
        return m_digest->GetDigest(p, len);
    }

protected:
    bool m_initilized;
    bool m_finished;
    DigestInterface* m_digest;
};


#endif /* DIGEST_COMMON_ */

