
#include "main.hpp"
#include <vector>
#include <string>
#include <pthread.h>


class PthreadLock : public SimpleLock
{
private:
    pthread_mutex_t m_mutex;

public:
    PthreadLock()
    {
        pthread_mutex_init(&m_mutex);
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


bool
WaitForAvailableIOJob(FileInfo*& file)
{
    bool ok = false;
    g_bufferMutex->Lock();

    while (!ok)
    {
        file = *g_ioFileIter;
        if (g_currentIOBuffer.availableForRead(file))
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

    g_bufferMutex->Unlock();
}

void*
IOThread(void* threadid)
{
    bool allDone = false;
    FileInfo* file;

    while (!allDone)
    {
        WaitForAvailableIOJob(file);

        // read file
        file.Read(g_currentIOBuffer);

        g_bufferMutex->Lock();

        g_currentIOBuffer->doneReading();
        g_currentIOBuffer = g_currentIOBuffer->m_next();

        if (file.hasReachedEOF())
        {
            g_ioFileIter.next();
            if (g_ioFileIter == g_fileArray.end())
            {
                allDone = true;
            }
        }

        // wake up hash thread
        g_hashCond->Broadcast();

        g_bufferMutex->Unlock();
    }
    return NULL;
}


bool
CheckAvailableHashJob(Digester*& digester, FileInputBuffer*& buffer)
{

}


void*
HashThread(void* unused)
{
    g_bufferMutex->Lock();
    while ()
    {

    }
    g_bufferMutex->Unlock();


}


int
main()
{
    g_currentIOBuffer = FileInputBuffer::g_inputRingBufferArr[0];
    g_currentHashBuffer = g_currentIOBuffer;

    g_ioFile = g_fileArray.head();
}


