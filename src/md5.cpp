
#include "md5.hpp"
#include "md5/md5.h"


// Factory instance to register
MD5_Factory MD5_Factory::instance;


const char* const MD5_Digester::NAME = "MD5";


void MD5_Digester::Initialize()
{
    MD5_Init(&m_ctx);
}

void MD5_Digester::Update(const u8* p, size_t len)
{
    MD5_Update(&m_ctx, p, len);
}

void MD5_Digester::Finish()
{
    MD5_Final(m_result, &m_ctx);
}

STATIC_ASSERT(STATIC_ASSERT_MD5_DIGEST_LENGTH, 
            (MD5_Digester::DIGEST_BYTES == MD5_DIGEST_LENGTH) );

void MD5_Digester::GetDigest(DigestResult& result)
{
    result.assign(m_result, DIGEST_BYTES);
}

void MD5_Digester::GetDigest(u8* p, size_t len)
{
    ASSERT(len >= DIGEST_BYTES);

    memcpy(p, m_result, DIGEST_BYTES);
}

