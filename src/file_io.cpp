
#include "file_io.hpp"
#include <vector>
#include <string>
#include <stdio.h>


FileInputBuffer** FileInputBuffer::g_inputRingBufferArr = NULL;
size_t FileInputBuffer::g_ringBufferCount = 0;

void
FileInputBuffer::Initialize(int hashCount, size_t bufferSize, size_t bufferCount)
{
    g_ringBufferCount = bufferCount;
    g_inputRingBufferArr = new FileInputBuffer*[g_ringBufferCount];

    g_inputRingBufferArr[0] = new FileInputBuffer(0, bufferSize, hashCount);
    for (size_t i = 1; i < g_ringBufferCount; ++i)
    {
        g_inputRingBufferArr[i] = new FileInputBuffer(i, bufferSize, hashCount);
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

FileInputBuffer::FileInputBuffer(int index, size_t bufferSize, int hashCount)
    : m_myIndex(index), m_file(NULL), m_offset(0), m_length(0), m_reachEOF(false),
        m_reading(false), m_prev(NULL), m_next(NULL)
{
    m_buffer = new u8[bufferSize];
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

bool
FileInfo::read(FileInputBuffer* buffer)
{
    ASSERT(buffer->getReading());

    if (m_fileHandle == NULL)
    {
        m_fileHandle = fopen(m_fileName.c_str(), "rb");
        if (m_fileHandle == NULL)
        {
            printf("Unable to open file: %s \n", m_fileName.c_str());
            return false;
        }
        dbg_printf("Open file success. \n");
        m_currentOffset = 0;
    }

    size_t nr = fread(buffer->getBuffer(), 
                1, 
                buffer->getBufferSize(), 
                m_fileHandle);

    if (nr != buffer->getBufferSize())
    {
        if (feof(m_fileHandle))
        {
            m_reachEOF = true;
            buffer->setReadEOF();
            dbg_printf("Read %u bytes in buffer %d, reach EOF \n", nr, buffer->getIndex());
        }
        else
        {
            m_ferror = ferror(m_fileHandle);
            return false;
        }
        fclose(m_fileHandle);
        m_fileHandle = NULL;
    }
    else
    {
        dbg_printf("Read %u bytes in buffer %d \n", nr, buffer->getIndex());
    }

    buffer->setReadOffset(m_currentOffset);
    buffer->setReadLength(nr);
    m_currentOffset += nr;

    return true;
}


BufferHashStatus::BufferHashStatus()
 : m_status(NOT_RUN)
{

}

