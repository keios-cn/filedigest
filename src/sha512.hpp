
#ifndef SHA512_HPP_
#define SHA512_HPP_

#include "digest_common.hpp"
#include "sha/sha.h"


class SHA512_Digester : public Digester
{
public:
    enum { DIGEST_BYTES = (512/8) };

    virtual void Initialize();
    virtual void Update(const u8* p, size_t len);
    virtual void Finish();
    virtual void GetDigest(DigestResult& result);

public:
    static SHA512_Digester* CreateDigester()
    {
        return new SHA512_Digester();
    }

    static const char* const NAME;

private:
    SHA512_Digester() : Digester(NAME, DIGEST_BYTES)
    {
        Initialize();
    }

    SHA512_CTX m_ctx;
    uint8 m_result[DIGEST_BYTES];
};


class SHA512_Factory: public DigesterFactory
{
    friend class SHA512_Digester;

public:
    virtual Digester* CreateDigester()
    {
        return SHA512_Digester::CreateDigester();
    }
    virtual const char* GetDigestName()
    {
        return SHA512_Digester::NAME;
    }

protected:
    SHA512_Factory()
    {}

private:
    static SHA512_Factory instance;
};


#endif /* SHA512_HPP_ */
