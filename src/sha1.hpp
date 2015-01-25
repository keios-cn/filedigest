
#ifndef SHA1_HPP_
#define SHA1_HPP_

#include "digest_common.hpp"
#include "sha/sha.h"


class SHA1_Digester : public Digester
{
public:
    enum { SHA1_DIGEST_BYTES = 20 };

    virtual void Initialize();
    virtual void Update(const u8* p, size_t len);
    virtual void Finish();
    virtual void GetDigest(DigestResult& result);

public:
    static SHA1_Digester* CreateDigester()
    {
        return new SHA1_Digester();
    }

    static const char* const NAME;

private:
    SHA1_Digester() : Digester(NAME, SHA1_DIGEST_BYTES)
    {
        Initialize();
    }

    SHA_CTX m_ctx;
    uint8 m_result[SHA1_DIGEST_BYTES];
};


class SHA1_Factory: public DigesterFactory
{
    friend class SHA1_Digester;

public:
    virtual Digester* CreateDigester()
    {
        return SHA1_Digester::CreateDigester();
    }
    virtual const char* GetDigestName()
    {
        return SHA1_Digester::NAME;
    }

protected:
    SHA1_Factory()
    {}

private:
    static SHA1_Factory instance;
};


#endif /* SHA1_HPP_ */
