
#include <vector>
#include <string>
#include <pthread.h>

#include "main.hpp"
#include "file_io.hpp"


class PthreadCondition;

class PthreadLock : public SimpleLock
{
    friend PthreadCondition;

private:
    pthread_mutex_t m_mutex;

public:
    PthreadLock()
    {
        pthread_mutex_init(&m_mutex, NULL);
    }
    ~PthreadLock()
    {
        pthread_mutex_destroy(&m_mutex);
    }

    virtual void Lock()
    {
        pthread_mutex_lock(&m_mutex);
    }
    virtual void Unlock()
    {
        pthread_mutex_unlock(&m_mutex);
    }
};


SimpleLock*
CreateSimpleLock()
{
    return new PthreadLock();
}


class PthreadCondition : public SimpleCondition
{
    friend PthreadLock;

private:
    pthread_cond_t m_cond;

public:
    PthreadCondition()
    {
        pthread_cond_init (&m_cond, NULL);
    }
    ~PthreadCondition()
    {
        pthread_cond_destroy (&m_cond);
    }

    virtual void Wait(SimpleLock* lock)
    {
        PthreadLock* mutex = (PthreadLock*) lock;
        pthread_cond_wait(&m_cond, &mutex->m_mutex);
    }

    virtual void Signal()
    {
        pthread_cond_signal(&m_cond);
    }

    virtual void Broadcast()
    {
        pthread_cond_broadcast(&m_cond);
    }

};


SimpleLock* g_bufferMutex = NULL;

SimpleCondition* g_hashCond = NULL;

SimpleCondition* g_ioCond = NULL;

FileInputBuffer* g_currentIOBuffer;
FileInputBuffer* g_currentHashBuffer;


std::vector<FileInfo*> g_fileArray;

std::vector<FileInfo*>::iterator g_ioFileIter;


void
WaitForAvailableIOJob(FileInfo*& file)
{
    bool ok = false;
    // g_bufferMutex->Lock();

    while (!ok)
    {
        file = *g_ioFileIter;
        if (g_currentIOBuffer->availableForRead(file))
        {
            // get file and buffer
            ok = true;
        }
        else
        {
            // current IO buffer is not available, wait...
            g_ioCond->Wait(g_bufferMutex);
        }
    }

    // g_bufferMutex->Unlock();
}

void
IOThread()
{
    bool allDone = false;
    FileInfo* file;

    g_bufferMutex->Lock();

    while (!allDone)
    {
        WaitForAvailableIOJob(file);

        g_bufferMutex->Unlock();

        // read file
        file->Read(g_currentIOBuffer);

        g_bufferMutex->Lock();

        g_currentIOBuffer->doneReading();
        g_currentIOBuffer = g_currentIOBuffer->nextBuf();

        if (file->hasReachedEOF())
        {
            g_ioFileIter++;
            if (g_ioFileIter == g_fileArray.end())
            {
                allDone = true;
            }
        }

        // wake up hash thread
        g_hashCond->Broadcast();
    }

    g_bufferMutex->Unlock();

    return;
}


bool
CheckAvailableHashJob(FileInputBuffer*& buffer, int& index)
{
    if (!g_currentHashBuffer->availableForHash())
        return false;

    if (g_currentHashBuffer->getFirstRunnableJob(index))
    {
        buffer = g_currentHashBuffer;
        return true;
    }

    // no available job in current buffer,
    // let's see next buffer
    buffer = g_currentHashBuffer->nextBuf();

    if (!buffer->availableForHash())
    {
        return false;
    }

    FileInfo* fileInfo = buffer->getFileInfo();
    if (g_currentHashBuffer->getFileInfo() != fileInfo)
    {
        // next buffer is a new file
        ASSERT(g_currentHashBuffer->hasReachEOF());

        if (buffer->getFirstRunnableJob(index))
        {
            return true;
        }
        return false;
    }
    else
    {
        // same file
        for (index = 0; index < fileInfo->getHashCount(); ++index)
        {
            if (g_currentHashBuffer->getHashStatusByIndex(index)->hasDone() &&
                    buffer->getHashStatusByIndex(index)->hasNotRun())
            {
                return true;
            }
        }
        return false;
    }
}


void
HashThread()
{
    bool allDone = false;

    int hashIndex;
    FileInputBuffer* buffer;

    g_bufferMutex->Lock();

    while (!allDone)
    {
        while (!CheckAvailableHashJob(buffer, hashIndex))
        {
            g_hashCond->Wait(g_bufferMutex);
        }

        buffer->getHashStatusByIndex(hashIndex)->setRunning();
        g_bufferMutex->Unlock();

        buffer->doHash(hashIndex);

        g_bufferMutex->Lock();

        buffer->getHashStatusByIndex(hashIndex)->setDone();
        if (buffer->hasReachEOF())
        {
            if (buffer->checkAllDone() && buffer->getFileInfo()->checkFinish())
            {
                buffer->getFileInfo()->reportResult();
            }
        }

        if (buffer->checkAllDone())  // can move next
        {
            ASSERT(buffer == g_currentHashBuffer);

            g_currentHashBuffer = buffer->nextBuf();

            buffer->resetForRead();
            g_ioCond->Broadcast();
        }
    }

    g_bufferMutex->Unlock();
    return;
}


void
InitializeThreads()
{
    g_bufferMutex = CreateSimpleLock();
    g_hashCond = CreateSimpleCondition();
    g_ioCond   = CreateSimpleCondition();
}


int
main()
{
    std::vector<std::string> hashNames;
    hashNames.push_back("md4");
    hashNames.push_back("ed2k_file_hash");

    FileInfo* f1 = new FileInfo("test.dat");
    f1->InitializeHashers(hashNames);
    g_fileArray.push_back(f1);


    FileInputBuffer::Initialize(hashNames.size());
    g_currentIOBuffer = FileInputBuffer::g_inputRingBufferArr[0];
    g_currentHashBuffer = g_currentIOBuffer;

    g_ioFileIter = g_fileArray.begin();

    return 0;
}


