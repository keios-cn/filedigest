
#include "sha256.hpp"
#include "sha/sha.h"


// Factory instance to register
SHA256_Factory SHA256_Factory::instance;


const char* const SHA256_Digester::NAME = "SHA256";


void SHA256_Digester::Initialize()
{
    SHA256_Init(&m_ctx);
}

void SHA256_Digester::Update(const u8* p, size_t len)
{
    SHA256_Update(&m_ctx, p, len);
}

void SHA256_Digester::Finish()
{
    SHA256_Final(m_result, &m_ctx);
}

STATIC_ASSERT(STATIC_ASSERT_SHA256_DIGEST_LENGTH, 
            (SHA256_Digester::DIGEST_BYTES == SHA256_DIGEST_LENGTH) );

void SHA256_Digester::GetDigest(DigestResult& result)
{
    result.assign(m_result, DIGEST_BYTES);
}
