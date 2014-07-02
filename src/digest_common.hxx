
#ifndef DIGEST_COMMON_
#define DIGEST_COMMON_


class Digester
{
public:
    virtual void CTX_Initialize() = 0;
    virtual void CTX_Update(const void* p, size_t len) = 0;
    virtual void CTX_Finish() = 0;
    virtual size_t GetDigestLength() = 0;
    virtual bool GetDigest(void* p, size_t& len) = 0;

    virtual DigesterFactory* GetFactory();
    const char* GetDigesterName();
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
};


class DigestCommon : public DigestInterface
{
public:
    DigestCommon() : m_initilized(false), m_finished(false)
    {}

    virtual ~DigestCommon()
    {}



protected:
    bool m_initilized;
    bool m_finished;

};


#endif /* DIGEST_COMMON_ */

