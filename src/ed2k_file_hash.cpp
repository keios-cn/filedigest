
#include "ed2k_file_hash.hxx"


Ed2kFile_Factory Ed2kFile_Factory::instance();

const char* const Ed2kFile_Digester::NAME = "ed2k";

~Ed2kFile_Digester()
{
    if (m_chunkHash != NULL)
    {
        delete m_chunkHash;
    }
    if (m_rootHash != NULL)
    {
        delete m_rootHash;
    }
}


void Ed2kFile_Digester::Initialize()
{
    if (m_chunkHash == NULL)
    {
        m_chunkHash = MD4_Digester::CreateDigester();
    }

    if (m_rootHash == NULL)
    {
        m_rootHash = MD4_Digester::CreateDigester();
    }

    ASSERT(m_chunkHash != NULL && m_rootHash != NULL)
    m_chunkHash->Initialize();
    m_rootHash->Initialize();
    memset(m_result, 0, sizeof(m_result));
    m_fileOffset = 0;
}

void Ed2kFile_Digester::Update(const void* p, size_t len)
{
    ulong64 chunkOff = m_fileOffset % ED2K_CHUNK_SIZE;
    if (chunkOff + len < ED2K_CHUNK_SIZE)
    {
        m_chunkHash->Update(p, len);
    }
    else
    {
        size_t chunkRemain = ED2K_CHUNK_SIZE - chunkOff;
        m_chunkHash->Update(p, chunkRemain);
        m_chunkHash->Finish();

        uint8 chunkHash[MD4_Digester::MD4_DIGEST_BYTES];
        size_t hLen = sizeof(chunkHash);
        m_chunkHash->GetDigest(chunkHash, hLen);

        m_rootHash->Update(chunkHash, hLen);

        // hash remaining data in a new chunk
        ASSERT(len >= chunkRemain);
        m_chunkHash->Initialize();
        m_chunkHash->Update(p + chunkRemain, len - chunkRemain);
    }

    m_fileOffset += len;
}

void Ed2kFile_Digester::Finish()
{
    // TODO
    if (m_fileOffset <= ED2K_CHUNK_SIZE)
    {

    }
    MD4_Final(m_result, &m_ctx);
}

size_t Ed2kFile_Digester::GetDigestLength()
{
    return MD4_Digester::MD4_DIGEST_BYTES;
}

bool Ed2kFile_Digester::GetDigest(void* p, size_t& len)
{
    if (len < MD4_Digester::MD4_DIGEST_BYTES)
        return false;

    memcpy(p, m_result, MD4_Digester::MD4_DIGEST_BYTES);
    len = MD4_Digester::MD4_DIGEST_BYTES;
    return true;
}

DigesterFactory* Ed2kFile_Digester::GetFactory()
{
    return &Ed2kFile_Factory::instance;
}

