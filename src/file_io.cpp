
#include "file_io.hpp"
#include <vector>
#include <string>


// default 64M
size_t g_defaultBufferSize  = 8 * 1024;
size_t g_defaultBufferCount = 8 * 1024;



void
FileInputBuffer::Initialize(int hashCount)
{
    g_ringBufferCount = g_defaultBufferCount;
    g_inputRingBufferArr = new FileInputBuffer*[g_ringBufferCount];

    g_inputRingBufferArr[0] = new FileInputBuffer(g_defaultBufferSize, hashCount);
    for (size_t i = 1; i < g_ringBufferCount; ++i)
    {
        g_inputRingBufferArr[i] = new FileInputBuffer(g_defaultBufferSize, hashCount);
        g_inputRingBufferArr[i-1]->m_next = g_inputRingBufferArr[i];
        g_inputRingBufferArr[i]->m_prev = g_inputRingBufferArr[i-1];
    }
    g_inputRingBufferArr[0]->m_prev = g_inputRingBufferArr[g_ringBufferCount-1];
    g_inputRingBufferArr[g_ringBufferCount-1]->m_next = g_inputRingBufferArr[0];
}

void
FileInputBuffer::Uninitialize()
{
    for (size_t i = 0; i < g_ringBufferCount; ++i)
    {
        delete g_inputRingBufferArr[i];
    }
    delete[] g_inputRingBufferArr;
    g_inputRingBufferArr = NULL;
}

FileInputBuffer::FileInputBuffer(size_t bufferSize, int hashCount)
    : m_file(NULL), m_offset(0), m_length(0), m_reachEOF(false),
        m_reading(false), m_prev(NULL), m_next(NULL)
{
    m_buffer = new char[bufferSize];
    m_bufferSize = bufferSize;

    m_hashStatusArr = new BufferHashStatus[hashCount];
    m_hashCount = hashCount;
}

FileInputBuffer::~FileInputBuffer()
{
    if (m_buffer != NULL)
        delete[] m_buffer;

    if (m_hashStatusArr != NULL)
        delete[] m_hashStatusArr;
}



bool
FileInfo::InitializeHashers(const std::vector<std::string>& hashNames)
{
    m_hashCount = hashNames.size();

    m_hashers.resize(m_hashCount);
    m_results.resize(m_hashCount);

    for (int i = 0; i < m_hashCount; ++i)
    {
        m_hashers[i] = DigesterFactory::CreateDigesterByName(hashNames[i].c_str());
        if (m_hashers[i] == NULL)
            return false;
    }
    return true;
}


BufferHashStatus::BufferHashStatus()
 : m_status(NOT_RUN)
{

}

