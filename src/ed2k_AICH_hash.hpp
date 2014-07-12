
#ifndef ED2K_AICH_HASH_
#define ED2K_AICH_HASH_

#include "digest_common.hxx"
#include "md4.hxx"


class Ed2kAICH_Digester : public Digester
{
public:
    virtual void Initialize();
    virtual void Update(const void* p, size_t len);
    virtual void Finish();
    virtual size_t GetDigestLength();
    virtual bool GetDigest(void* p, size_t& len);
    virtual DigesterFactory* GetFactory();

    virtual ~Ed2kAICH_Digester();

public:
    static Ed2kAICH_Digester* CreateDigester()
    {
        return new Ed2kAICH_Digester();
    }

    static const char* const NAME;

    enum { ED2K_CHUNK_BYTES = 9728000, ED2K_AICH_BLOCK_BYTES = 180*1024 };

private:

    Ed2kAICH_Digester() : 
            m_chunkHash(NULL), 
            m_rootHash(NULL), 
            m_fileOffset(0),
            m_chunkStart(0)
    {
        Initialize();
    }

    void OnChunkComplete();

    MD4_Digester* m_chunkHash;
    MD4_Digester* m_rootHash;
    ulong64       m_fileOffset;     // Current offset in file
    ulong64       m_chunkStart;     // start offset of current chunk hash
    uint8         m_result[MD4_Digester::MD4_DIGEST_BYTES];
};


class Ed2kAICH_Factory: public DigesterFactory
{
public:
    virtual Digester* CreateDigester()
    {
        return Ed2kAICH_Digester::CreateDigester();
    }
    virtual const char* GetDigestName()
    {
        return Ed2kAICH_Digester::NAME;
    }

protected:
    Ed2kAICH_Factory()
    {}

private:
    static Ed2kAICH_Factory instance;
};


#endif /* ED2K_AICH_HASH_ */
