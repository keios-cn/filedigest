
#include "md4.hxx"
#include "md4/md4.h"


MD4_Factory MD4_Factory::instance();

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

size_t MD4_Digester::GetDigestLength()
{
	return MD4_DIGEST_LENGTH;
}

bool MD4_Digester::GetDigest(void* p, size_t& len)
{
	if (len < MD4_DIGEST_LENGTH)
		return false;
	memcpy(p, m_result, MD4_DIGEST_LENGTH);
	len = MD4_DIGEST_LENGTH;
	return true;
}

DigesterFactory* MD4_Digester::GetFactory()
{
	return &MD4Factory::instance;
}

