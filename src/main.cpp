
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

SimpleCondition* CreateSimpleCondition()
{
    return new PthreadCondition();
}


SimpleLock* g_bufferMutex = NULL;

SimpleCondition* g_hashCond = NULL;

SimpleCondition* g_ioCond = NULL;

FileInputBuffer* g_currentIOBuffer;
FileInputBuffer* g_currentHashBuffer;


std::vector<FileInfo*> g_fileArray;

std::vector<FileInfo*>::iterator g_ioFileIter;

bool g_allReadDone = false;


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
        if (!file->read(g_currentIOBuffer))
        {
            // handle error
            printf("Read file error\n");
            return;
        }

        g_bufferMutex->Lock();

        g_currentIOBuffer->setReadDone();
        g_currentIOBuffer = g_currentIOBuffer->nextBuf();

        if (file->hasReachedEOF())
        {
            g_ioFileIter++;

            dbg_printf("Move to next file\n");

            if (g_ioFileIter == g_fileArray.end())
            {
                allDone = true;
            }
        }

        // wake up hash thread
        g_hashCond->Broadcast();
    }

    g_allReadDone = true;
    g_bufferMutex->Unlock();

    dbg_printf("IO thread exit...\n");

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
        dbg_printf("1. get index %d on buffer %d \n", index, buffer->getIndex());
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
    ASSERT(fileInfo != NULL);

    if (g_currentHashBuffer->getFileInfo() != fileInfo)
    {
        // next to current buffer is a new file
        ASSERT(g_currentHashBuffer->hasReachEOF());

        if (buffer->getFirstRunnableJob(index))
        {
            dbg_printf("2. get index %d on buffer %d \n", index, buffer->getIndex());
            return true;
        }
        return false;
    }
    else
    {
        // same file
        ASSERT(fileInfo->getHashCount() == g_currentHashBuffer->getHashCount());
        ASSERT(fileInfo->getHashCount() == buffer->getHashCount());
        for (index = 0; index < fileInfo->getHashCount(); ++index)
        {
            if (g_currentHashBuffer->getHashStatusByIndex(index)->hasDone() &&
                    buffer->getHashStatusByIndex(index)->hasNotRun())
            {
                dbg_printf("3. get index %d on buffer %d \n", index, buffer->getIndex());
                return true;
            }
        }
        return false;
    }
}


void
HashThread(int selfNumber)
{
    int hashIndex;
    FileInputBuffer* buffer;

    g_bufferMutex->Lock();

    while (true)
    {
        while (!CheckAvailableHashJob(buffer, hashIndex))
        {
            if (g_allReadDone)
            {
                // all file read finished, and no more hash job
                // hash thread should exit now
                goto _ExitThread;
            }
            g_hashCond->Wait(g_bufferMutex);
        }

        dbg_printf("HashThread[%d] Start hashIndex %d on buffer %d \n", 
                selfNumber, hashIndex, buffer->getIndex());

        buffer->getHashStatusByIndex(hashIndex)->setRunning();
        g_bufferMutex->Unlock();

        buffer->doHash(hashIndex);

        g_bufferMutex->Lock();

        dbg_printf("HashThread[%d] Finish hashIndex %d on buffer %d \n", 
                selfNumber, hashIndex, buffer->getIndex());

        buffer->getHashStatusByIndex(hashIndex)->setDone();
        if (buffer->hasReachEOF())
        {
            if (buffer->checkAllDone() && buffer->getFileInfo()->checkFinish())
            {
                buffer->getFileInfo()->reportResult();
                dbg_printf("HashThread[%d] Print result done.\n", selfNumber);
            }
        }

        if (buffer->checkAllDone())  // can move next
        {
            ASSERT(buffer == g_currentHashBuffer);

            g_currentHashBuffer = buffer->nextBuf();

            dbg_printf("HashThread[%d] Current hash buffer move to %d \n", 
                    selfNumber, g_currentHashBuffer->getIndex());

            buffer->resetForRead();
            g_ioCond->Broadcast();
        }
    }

_ExitThread:
    dbg_printf("HashThread[%d] exit.\n", selfNumber);

    g_bufferMutex->Unlock();

    return;
}


void *
HashThread_Wrapper(void* ptr)
{
    HashThread( (int) ptr );
    return NULL;
}

void*
IOThread_Wrapper(void* ptr)
{
    IOThread();
    return NULL;
}

#define MAX_THREADS 8
pthread_t g_ioThread = 0;
pthread_t g_hashThread[MAX_THREADS] = {0,};

void
InitializeThreads()
{
    g_bufferMutex = CreateSimpleLock();
    g_hashCond = CreateSimpleCondition();
    g_ioCond   = CreateSimpleCondition();

    int ret = pthread_create(&g_ioThread, NULL, IOThread_Wrapper, NULL);
    if (ret)
    { 
        fprintf(stderr, "Error - pthread_create() return code: %d\n", ret); 
        exit(1); 
    } 

    for (int i = 0; i < 4; ++i)
    {
        ret = pthread_create(&g_hashThread[i], NULL, HashThread_Wrapper, (void*)i);
        if (ret)
        { 
            fprintf(stderr, "Error - pthread_create() return code: %d\n", ret); 
            exit(1); 
        } 
    }

}


int
main(int argc, char* argv[])
{
    std::vector<std::string> hashNames;
    hashNames.push_back("MD4");
    hashNames.push_back("ed2k_file_hash");
    hashNames.push_back("MD5");
    hashNames.push_back("SHA1");
    //hashNames.push_back("SHA224");
    //hashNames.push_back("SHA256");
    //hashNames.push_back("SHA384");
    //hashNames.push_back("SHA512");

    FileInfo* f1 = new FileInfo(argv[1]);
    if (! f1->InitializeHashers(hashNames) )
    {
        printf("InitializeHashers failed! \n");
        return 1;
    }

    g_fileArray.push_back(f1);

    FileInputBuffer::Initialize(hashNames.size(), 128 * 1024, 256);
    g_currentIOBuffer = FileInputBuffer::g_inputRingBufferArr[0];
    g_currentHashBuffer = g_currentIOBuffer;

    g_ioFileIter = g_fileArray.begin();

    InitializeThreads();

    pthread_join(g_ioThread, NULL);
    for (int i = 0; i < MAX_THREADS; ++i)
    {
        if (g_hashThread[i] != 0)
        {
            pthread_join(g_hashThread[i], NULL);
        }
    }

    return 0;
}


