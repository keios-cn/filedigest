
#ifndef SHA224_HPP_
#define SHA224_HPP_

#include "digest_common.hpp"
#include "sha/sha.h"


class SHA224_Digester : public Digester
{
public:
    enum { DIGEST_BYTES = (224/8) };

    virtual void Initialize();
    virtual void Update(const u8* p, size_t len);
    virtual void Finish();
    virtual void GetDigest(DigestResult& result);

public:
    static SHA224_Digester* CreateDigester()
    {
        return new SHA224_Digester();
    }

    static const char* const NAME;

private:
    SHA224_Digester() : Digester(NAME, DIGEST_BYTES)
    {
        Initialize();
    }

    SHA256_CTX m_ctx;
    uint8 m_result[DIGEST_BYTES];
};


class SHA224_Factory: public DigesterFactory
{
    friend class SHA224_Digester;

public:
    virtual Digester* CreateDigester()
    {
        return SHA224_Digester::CreateDigester();
    }
    virtual const char* GetDigestName()
    {
        return SHA224_Digester::NAME;
    }

protected:
    SHA224_Factory()
    {}

private:
    static SHA224_Factory instance;
};


#endif /* SHA224_HPP_ */
