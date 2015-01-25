
#include "sha512.hpp"
#include "sha/sha.h"


// Factory instance to register
SHA512_Factory SHA512_Factory::instance;


const char* const SHA512_Digester::NAME = "SHA512";


void SHA512_Digester::Initialize()
{
    SHA512_Init(&m_ctx);
}

void SHA512_Digester::Update(const u8* p, size_t len)
{
    SHA512_Update(&m_ctx, p, len);
}

void SHA512_Digester::Finish()
{
    SHA512_Final(m_result, &m_ctx);
}

STATIC_ASSERT(STATIC_ASSERT_SHA512_DIGEST_LENGTH, 
            (SHA512_Digester::DIGEST_BYTES == SHA512_DIGEST_LENGTH) );

void SHA512_Digester::GetDigest(DigestResult& result)
{
    result.assign(m_result, DIGEST_BYTES);
}
