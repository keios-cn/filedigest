
#ifndef SHA256_HPP_
#define SHA256_HPP_

#include "digest_common.hpp"
#include "sha/sha.h"


class SHA256_Digester : public Digester
{
public:
    enum { DIGEST_BYTES = (256/8) };

    virtual void Initialize();
    virtual void Update(const u8* p, size_t len);
    virtual void Finish();
    virtual void GetDigest(DigestResult& result);

public:
    static SHA256_Digester* CreateDigester()
    {
        return new SHA256_Digester();
    }

    static const char* const NAME;

private:
    SHA256_Digester() : Digester(NAME, DIGEST_BYTES)
    {
        Initialize();
    }

    SHA256_CTX m_ctx;
    uint8 m_result[DIGEST_BYTES];
};


class SHA256_Factory: public DigesterFactory
{
    friend class SHA256_Digester;

public:
    virtual Digester* CreateDigester()
    {
        return SHA256_Digester::CreateDigester();
    }
    virtual const char* GetDigestName()
    {
        return SHA256_Digester::NAME;
    }

protected:
    SHA256_Factory()
    {}

private:
    static SHA256_Factory instance;
};


#endif /* SHA256_HPP_ */
