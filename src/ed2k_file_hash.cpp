
#include "ed2k_file_hash.hpp"


Ed2kFile_Factory Ed2kFile_Factory::instance;

const char* const Ed2kFile_Digester::NAME = "ed2k_file_hash";


Ed2kFile_Digester::~Ed2kFile_Digester()
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

    size_t hLen = sizeof(m_lastChunkHash);
    m_chunkHash->GetDigest(m_lastChunkHash, hLen);

    // TODO: Record all chunk hash, into a map <m_chunkStart, chunkHash>
    //       to support Hashset link: 
    //    ed2k://|file|name|12043984|6744FC42EDA527B27F0B2F2538728B3E|p=264E6F6B587985D87EB0157A2A7BAF40:17B9A4D1DCE0E4C2B672DF257145E98A|/
    //    ed2k://|file|name|12043984|<File Hash>|p=<Hash of 1st part>:<Hash of 2st part>|/

    m_rootHash->Update(m_lastChunkHash, hLen);

    // init for next chunk
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

    if (m_fileOffset < ED2K_CHUNK_BYTES)
    {
        m_chunkHash->Finish();
        m_chunkHash->GetDigest(m_result, hLen);
    }
    else if (m_fileOffset == ED2K_CHUNK_BYTES)
    {
        memcpy(m_result, m_lastChunkHash, sizeof(m_result));
    }
    else
    {
        ASSERT(m_fileOffset > ED2K_CHUNK_BYTES);

        OnChunkComplete();
        m_rootHash->Finish();
        m_rootHash->GetDigest(m_result, hLen);
    }
}


void Ed2kFile_Digester::GetDigest(DigestResult& result)
{
    result.Assign(m_result, sizeof(m_result));
}

