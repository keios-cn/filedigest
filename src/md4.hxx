
#ifndef MD4_HXX_
#define MD4_HXX_

#include "digest_common.hxx"


class MD4_Digester : public Digester
{
public:
    virtual void Initialize();
    virtual void Update(const void* p, size_t len);
    virtual void Finish();
    virtual size_t GetDigestLength();
    virtual bool GetDigest(void* p, size_t& len);
    virtual DigesterFactory* GetFactory();

public:
    static MD4_Digester* CreateDigester()
    {
        return new MD4_Digester();
    }
    
    static const char* const name;

private:
	MD4_Digester()
	{
		Initialize();
	}

	MD4_CTX m_ctx;
	unsigned char m_result[16];
};


class MD4_Factory: public DigesterFactory
{
	friend class MD4_Digester;
public:
    virtual Digester* CreateDigester()
    {
    	return MD4_Digester::CreateDigester();
    }
    virtual const char* GetDigestName()
    {
    	return MD4_Digester::name;
    }

protected:
	MD4_Factory()
	{}

private:
	static MD4_Factory instance;
};


#endif /* MD4_HXX_ */
