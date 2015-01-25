
#include "sha384.hpp"
#include "sha/sha.h"


// Factory instance to register
SHA384_Factory SHA384_Factory::instance;


const char* const SHA384_Digester::NAME = "SHA384";


void SHA384_Digester::Initialize()
{
    SHA384_Init(&m_ctx);
}

void SHA384_Digester::Update(const u8* p, size_t len)
{
    SHA384_Update(&m_ctx, p, len);
}

void SHA384_Digester::Finish()
{
    SHA384_Final(m_result, &m_ctx);
}

STATIC_ASSERT(STATIC_ASSERT_SHA384_DIGEST_LENGTH, 
            (SHA384_Digester::DIGEST_BYTES == SHA384_DIGEST_LENGTH) );

void SHA384_Digester::GetDigest(DigestResult& result)
{
    result.assign(m_result, DIGEST_BYTES);
}
