
#include "sha1.hpp"
#include "sha/sha.h"


// Factory instance to register
SHA1_Factory SHA1_Factory::instance;


const char* const SHA1_Digester::NAME = "sha1";


void SHA1_Digester::Initialize()
{
    SHA1_Init(&m_ctx);
}

void SHA1_Digester::Update(const u8* p, size_t len)
{
    SHA1_Update(&m_ctx, p, len);
}

void SHA1_Digester::Finish()
{
    SHA1_Final(m_result, &m_ctx);
}

STATIC_ASSERT(STATIC_ASSERT_SHA1_DIGEST_LENGTH, 
            (SHA1_Digester::SHA1_DIGEST_BYTES == SHA_DIGEST_LENGTH) );

void SHA1_Digester::GetDigest(DigestResult& result)
{
    result.assign(m_result, SHA1_DIGEST_BYTES);
}
