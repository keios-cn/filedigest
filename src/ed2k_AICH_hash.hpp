
#ifndef ED2K_AICH_HASH_
#define ED2K_AICH_HASH_

#include "digest_common.hpp"
#include "md4.hpp"


/*
Reference: [1] http://www.emule-project.net/home/perl/help.cgi?l=1&topic_id=589&rm=show_topic
           [2] http://en.wikipedia.org/wiki/Ed2k_URI_scheme

* File Hash and ICH - Intelligent Corruption Handling

** File Hash, Part Hashes & Hashset
For every file shared in the network an unique identification value is created 
using the mathematical crypto algorithm MD4. This value is called file hash and 
is contained in every standard eD2k link, e.g.

ed2k://|file|name|12043984|6744FC42EDA527B27F0B2F2538728B3E|/

where 6744FC42EDA527B27F0B2F2538728B3E is the file hash making this file uniquely 
identified all over the network.
This File Hash is calculated by dividing the entire file into parts of 9.28 MB. 
For each of the parts a Part Hash is calculated using the same MD4 algorithm. 
These Part Hashes, called Hashset, are then used to calculate the final File Hash. 
For example a 600 MB file would be divided into 65 parts each with its own Part 
Hash which are then used to create the final File Hash.
To make sure that eMule always receives the correct Hashset a special link can 
be created containing this, e.g.

ed2k://|file|name|12043984|6744FC42EDA527B27F0B2F2538728B3E|p=264E6F6B587985D87EB0157A2A7BAF40:17B9A4D1DCE0E4C2B672DF257145E98A|/

where the p= value denotes the Hashset. Each Part Hash is divided by a ":". 
This file has a size of 12043984 Bytes (=11.49 MB) which means it has one full 
9.28 part and the rest left to 11.49 MB resulting in two Part Hashes.

** ICH Intelligent Corruption Handling
Whenever eMule finishes such a part it will be checked if the downloaded data 
matches the Part Hash value for this finished part. If yes, this part is offered 
for upload to help spreading it.
If no, a corruption has occurred and the part has to be downloaded again. 
To avoid downloading the full 9.28 MB, ICH redownloads 180 KB from the beginning 
of the part and then check the whole part again to see if the Part Hash is now 
correct. If not, the next 180 KB are downloaded, check again etc. until the part 
hash is correct again. In the best case eMule has to download only 180 KB again 
if the corruption is right at the beginning of the part. In the worst case the 
entire part has to be downloaded again if the corruption is somewhere near the 
end of the part. On average ICH saves 50% of redownloading in case of part 
corruptions.

*/


class Ed2kAICH_Digester : public Digester
{
public:
    virtual void Initialize();
    virtual void Update(const void* p, size_t len);
    virtual void Finish();
    virtual void GetDigest(DigestResult& result);

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
