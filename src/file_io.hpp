

#ifndef FILE_IO_
#define FILE_IO_

#include "types_def.hpp"
#include "digest_common.hpp"
#include <vector>
#include <string>
#include <stdio.h>


// default 64M
#define DEFAULT_BUFFER_SIZE  (8 * 1024)
#define DEFAULT_BUFFER_COUNT (8 * 1024)



class FileInputBuffer;

class FileInfo
{
private:
    std::string m_fileName;   // includes path
    uint64 m_fileLength;

    std::vector<Digester*> m_hashers;
    std::vector<DigestResult> m_results;
    int m_hashCount;

    bool m_allFinish;

    // IO related members
    bool m_reachEOF;   // File reach EOF or not

    // for file read
    FILE* m_fileHandle;
    uint64 m_currentOffset;
    int m_ferror;

public:
    FileInfo(const char* fileName)
        : m_fileName(fileName), m_hashCount(0), m_allFinish(false),
            m_reachEOF(false), 
            m_fileHandle(NULL), m_currentOffset(0), m_ferror(0)
    {}
    ~FileInfo();

    bool InitializeHashers(const std::vector<std::string>& hashNames);

    int getHashCount()
    {
        return m_hashCount;
    }

    bool hasReachedEOF()
    {
        return m_reachEOF;
    }

    bool read(FileInputBuffer* buffer);

    Digester* getDigesterByIndex(int index)
    {
        return m_hashers[index];
    }

    DigestResult& getResultByIndex(int index)
    {
        return m_results[index];
    }

    bool checkFinish()
    {
        for (int i = 0; i < m_hashCount; ++i)
        {
            if (!m_results[i].hasResult())
                return false;
        }
        m_allFinish = true;
        return true;
    }

    void reportResult()
    {
        //TODO : output all result for this file
        printf( "File: %s \n", m_fileName.c_str());

        for (int i = 0; i < m_hashCount; ++i)
        {
            Digester* dig = m_hashers[i];

            // result
            std::string res;
            m_results[i].toString(res);

            printf( "%s: %s \n", dig->GetName(), res.c_str());
        }
    }
};


class BufferHashStatus
{
private:
    enum { NOT_RUN, RUNNING, DONE } m_status;

    // Lock ?
    // pthread_mutex m_mutex;
public:
    BufferHashStatus();

    void reset()
    {
        m_status = NOT_RUN;
    }

    bool hasNotRun()
    {
        return (m_status == NOT_RUN);
    }

    bool hasDone()
    {
        return (m_status == DONE);
    }

    void setRunning()
    {
        m_status = RUNNING;
    }

    void setDone()
    {
        m_status = DONE;
    }
};


class FileInputBuffer
{
private:
    int m_myIndex;    // Global index of this buffer

    u8* m_buffer;         // buffer head pointer
    size_t m_bufferSize;  // size of buffer

    FileInfo* m_file;  // set to NULL for unused buffer
    uint64 m_offset;   // offset in file
    size_t m_length;   // length of data in buffer, will be same as buffer size, 
                        // unless last buffer on file
                        // or 0 for empty buffer
    bool m_reachEOF;   // Reach EOF or not
    bool m_reading;    // it is currently in reading or not

    BufferHashStatus* m_hashStatusArr;
    int m_hashCount;

    FileInputBuffer* m_prev;
    FileInputBuffer* m_next;

public:
    FileInputBuffer(int index, size_t bufferSize, int hashCount);
    ~FileInputBuffer();

    int getIndex()
    {
        return m_myIndex;
    }

    FileInputBuffer* nextBuf()
    {
        return m_next;
    }

    FileInfo* getFileInfo()
    {
        return m_file;
    }

    u8* getBuffer()
    {
        return m_buffer;
    }

    size_t getBufferSize()
    {
        return m_bufferSize;
    }

    void setReadLength(size_t len)
    {
        m_length = len;
    }

    bool hasReachEOF()
    {
        return m_reachEOF;
    }

    void setReadEOF()
    {
        m_reachEOF = true;
    }

    void setReadOffset(uint64 off)
    {
        m_offset = off;
    }

    bool availableForRead(FileInfo* newF)
    {
        if (m_reading)
            return false;
        if (m_file != NULL)
            return false;

        m_file = newF;
        m_reading = true;
        return true;
    }

    void setReadDone()
    {
        m_reading = false;
    }

    bool getReading()
    {
        return m_reading;
    }

    bool availableForHash()
    {
        if (m_file == NULL)
            return false;
        if (m_reading)
            return false;
        return true;
    }

    bool getFirstRunnableJob(int& index)
    {
        for (index = 0; index < m_hashCount; ++index)
        {
            if (m_hashStatusArr[index].hasNotRun())
            {
                return true;
            }
        }
        return false;
    }

    BufferHashStatus* getHashStatusByIndex(int index)
    {
        ASSERT(index < m_hashCount);
        return m_hashStatusArr + index;
    }

    int getHashCount()
    {
        return m_hashCount;
    }

    void doHash(int index)
    {
        Digester* digester = m_file->getDigesterByIndex(index);

        digester->Update(m_buffer, m_length);

        if (m_reachEOF)
        {
            digester->Finish();
            digester->GetDigest(m_file->getResultByIndex(index));
        }
    }

    bool checkAllDone()
    {
        for (int i = 0; i < m_hashCount; ++i)
        {
            if (!m_hashStatusArr[i].hasDone())
                return false;
        }
        return true;
    }

    void resetForRead()
    {
        m_file = NULL;
        m_reading = false;
        m_length = 0;
        m_offset = 0;
        for (int i = 0; i < m_hashCount; ++i)
        {
            m_hashStatusArr[i].reset();
        }
    }

    static void Initialize(int hashCount,
                        size_t bufferSize = DEFAULT_BUFFER_SIZE,
                        size_t bufferCount = DEFAULT_BUFFER_COUNT);
    static void Uninitialize();

    static FileInputBuffer** g_inputRingBufferArr;
    static size_t            g_ringBufferCount;
};


#endif  /* FILE_IO_ */

