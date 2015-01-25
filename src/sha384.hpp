
#ifndef SHA384_HPP_
#define SHA384_HPP_

#include "digest_common.hpp"
#include "sha/sha.h"


class SHA384_Digester : public Digester
{
public:
    enum { DIGEST_BYTES = (384/8) };

    virtual void Initialize();
    virtual void Update(const u8* p, size_t len);
    virtual void Finish();
    virtual void GetDigest(DigestResult& result);

public:
    static SHA384_Digester* CreateDigester()
    {
        return new SHA384_Digester();
    }

    static const char* const NAME;

private:
    SHA384_Digester() : Digester(NAME, DIGEST_BYTES)
    {
        Initialize();
    }

    SHA512_CTX m_ctx;
    uint8 m_result[DIGEST_BYTES];
};


class SHA384_Factory: public DigesterFactory
{
    friend class SHA384_Digester;

public:
    virtual Digester* CreateDigester()
    {
        return SHA384_Digester::CreateDigester();
    }
    virtual const char* GetDigestName()
    {
        return SHA384_Digester::NAME;
    }

protected:
    SHA384_Factory()
    {}

private:
    static SHA384_Factory instance;
};


#endif /* SHA384_HPP_ */
