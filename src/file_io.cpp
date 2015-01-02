
#include "file_io.hpp"

// default 64M
size_t g_default_buffer_size  = 8 * 1024;
size_t g_default_buffer_count = 8 * 1024;


FileInputBuffer** g_input_ring_buffers = NULL;
size_t            g_ring_buffer_count = 0;


void InitFileInputBuffer()
{
    g_ring_buffer_count = g_default_buffer_count;
    g_input_ring_buffers = new FileInputBuffer*[g_ring_buffer_count];

    for (int i = 0; i < g_ring_buffer_count; ++i)
    {
        g_input_ring_buffers[i] = new FileInputBuffer(g_default_buffer_size);
    }
}


FileInputBuffer::FileInputBuffer(size_t bufferSize)
    : m_file(NULL), m_offset(0), m_length(0), m_reachEOF(false),
      m_statusArr(NULL), m_hashCount(0)
{
    m_buffer = new char[bufferSize];
    m_bufferSize = bufferSize;
}

FileInputBuffer::~FileInputBuffer()
{
    if (m_buffer != NULL)
        delete[] m_buffer;
}


