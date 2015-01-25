
#include "md4.hpp"
#include "md4/md4.h"


// Factory instance to register
MD4_Factory MD4_Factory::instance;


const char* const MD4_Digester::NAME = "MD4";


void MD4_Digester::Initialize()
{
    MD4_Init(&m_ctx);
}

void MD4_Digester::Update(const u8* p, size_t len)
{
    MD4_Update(&m_ctx, p, len);
}

void MD4_Digester::Finish()
{
    MD4_Final(m_result, &m_ctx);
}

STATIC_ASSERT(STATIC_ASSERT_MD4_DIGEST_LENGTH, 
            (MD4_Digester::DIGEST_BYTES == MD4_DIGEST_LENGTH) );

void MD4_Digester::GetDigest(DigestResult& result)
{
    result.assign(m_result, DIGEST_BYTES);
}

void MD4_Digester::GetDigest(u8* p, size_t len)
{
    ASSERT(len >= DIGEST_BYTES);

    memcpy(p, m_result, DIGEST_BYTES);
}

