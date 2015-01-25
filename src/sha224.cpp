
#include "sha224.hpp"
#include "sha/sha.h"


// Factory instance to register
SHA224_Factory SHA224_Factory::instance;


const char* const SHA224_Digester::NAME = "SHA224";


void SHA224_Digester::Initialize()
{
    SHA224_Init(&m_ctx);
}

void SHA224_Digester::Update(const u8* p, size_t len)
{
    SHA224_Update(&m_ctx, p, len);
}

void SHA224_Digester::Finish()
{
    SHA224_Final(m_result, &m_ctx);
}

STATIC_ASSERT(STATIC_ASSERT_SHA224_DIGEST_LENGTH, 
            (SHA224_Digester::DIGEST_BYTES == SHA224_DIGEST_LENGTH) );

void SHA224_Digester::GetDigest(DigestResult& result)
{
    result.assign(m_result, DIGEST_BYTES);
}
