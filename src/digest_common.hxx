
#ifndef DIGEST_COMMON_
#define DIGEST_COMMON_

class DigestInterface
{
public:
    virtual void CTX_Initialize() = 0;
    virtual void CTX_Update(void* p, size_t len) = 0;
    virtual void CTX_Final() = 0;
    virtual size_t DigestLength() = 0;
    virtual bool GetDigest(void* p, size_t len) = 0;
};


#endif /* DIGEST_COMMON_ */

