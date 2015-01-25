
#ifndef MD5_HPP_
#define MD5_HPP_

#include "digest_common.hpp"
#include "md5/md5.h"


class MD5_Digester : public Digester
{
public:
    enum { DIGEST_BYTES = 16 };

    virtual void Initialize();
    virtual void Update(const u8* p, size_t len);
    virtual void Finish();
    virtual void GetDigest(DigestResult& result);

public:
    static MD5_Digester* CreateDigester()
    {
        return new MD5_Digester();
    }

    static const char* const NAME;

    void GetDigest(u8* p, size_t len);

private:
    MD5_Digester() : Digester(NAME, DIGEST_BYTES)
    {
        Initialize();
    }

    MD5_CTX m_ctx;
    uint8 m_result[DIGEST_BYTES];
};


class MD5_Factory: public DigesterFactory
{
    friend class MD5_Digester;

public:
    virtual Digester* CreateDigester()
    {
        return MD5_Digester::CreateDigester();
    }
    virtual const char* GetDigestName()
    {
        return MD5_Digester::NAME;
    }

protected:
    MD5_Factory()
    {}

private:
    static MD5_Factory instance;
};


#endif /* MD5_HPP_ */
