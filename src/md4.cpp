
#include "md4.hxx"
#include "md4/md4.h"


MD4_Factory MD4_Factory::instance();


const char* const MD4_Digester::NAME = "md4";


void MD4_Digester::Initialize()
{
    MD4_Init(&m_ctx);
}

void MD4_Digester::Update(const void* p, size_t len)
{
    MD4_Update(&m_ctx, p, len);
}

void MD4_Digester::Finish()
{
    MD4_Final(m_result, &m_ctx);
}

bool MD4_Digester::GetDigest(void* p, size_t& len)
{
    ASSERT(MD4_DIGEST_BYTES == MD4_DIGEST_LENGTH);

    if (len < MD4_DIGEST_BYTES)
        return false;

    memcpy(p, m_result, MD4_DIGEST_BYTES);
    len = MD4_DIGEST_BYTES;
    return true;
}
