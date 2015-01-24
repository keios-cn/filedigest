
#ifndef MD4_HPP_
#define MD4_HPP_

#include "digest_common.hpp"
#include "md4/md4.h"


class MD4_Digester : public Digester
{
public:
    enum { MD4_DIGEST_BYTES = 16 };

    virtual void Initialize();
    virtual void Update(const u8* p, size_t len);
    virtual void Finish();
    virtual void GetDigest(DigestResult& result);

public:
    static MD4_Digester* CreateDigester()
    {
        return new MD4_Digester();
    }

    static const char* const NAME;

    void GetDigest(u8* p, size_t len);

private:
    MD4_Digester() : Digester(NAME, MD4_DIGEST_BYTES)
    {
        Initialize();
    }

    MD4_CTX m_ctx;
    uint8 m_result[MD4_DIGEST_BYTES];
};


class MD4_Factory: public DigesterFactory
{
    friend class MD4_Digester;

public:
    virtual Digester* CreateDigester()
    {
        return MD4_Digester::CreateDigester();
    }
    virtual const char* GetDigestName()
    {
        return MD4_Digester::NAME;
    }

protected:
    MD4_Factory()
    {}

private:
    static MD4_Factory instance;
};


#endif /* MD4_HPP_ */
