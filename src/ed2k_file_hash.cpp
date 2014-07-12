
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
    m_chunkStart = 0;
}

void Ed2kFile_Digester::OnChunkComplete()
{
    m_chunkHash->Finish();

    uint8 chunkHash[MD4_Digester::MD4_DIGEST_BYTES];
    size_t hLen = sizeof(chunkHash);
    m_chunkHash->GetDigest(chunkHash, hLen);

    // TODO: Record all chunk hash, <m_chunkStart, chunkHash>

    m_rootHash->Update(chunkHash, hLen);

    // init a new chunk
    m_chunkHash->Initialize();
}

void Ed2kFile_Digester::Update(const void* p, size_t len)
{
    ulong64 chunkOff = m_fileOffset % ED2K_CHUNK_BYTES;

    if (chunkOff + len < ED2K_CHUNK_BYTES)
    {
        m_chunkHash->Update(p, len);
    }
    else
    {
        size_t off = 0;
        size_t chunkRemain = ED2K_CHUNK_BYTES - chunkOff;
        m_chunkHash->Update(p, chunkRemain);
        OnChunkComplete();

        off += chunkRemain;
        m_chunkStart = m_fileOffset + off;

        ASSERT(len >= chunkRemain);

        while (len - off >= ED2K_CHUNK_BYTES)
        {
            m_chunkHash->Update(p + off, ED2K_CHUNK_BYTES);
            OnChunkComplete();

            off += ED2K_CHUNK_BYTES;
            m_chunkStart = m_fileOffset + off;
        }

        // last less than chunk size
        if (len - off > 0)
        {
            m_chunkHash->Update(p + off, len - off);
        }
    }

    m_fileOffset += len;
}

void Ed2kFile_Digester::Finish()
{
    size_t hLen = sizeof(m_result);

    if (m_fileOffset <= ED2K_CHUNK_BYTES)
    {
        // FIXME! bug if m_fileOffset == ED2K_CHUNK_BYTES
        m_chunkHash->Finish();
        m_chunkHash->GetDigest(m_result, hLen);
    }
    else
    {
        OnChunkComplete();
        m_rootHash->Finish();
        m_rootHash->GetDigest(m_result, hLen);
    }
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

