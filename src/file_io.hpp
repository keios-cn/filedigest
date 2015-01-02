

#ifndef FILE_IO_
#define FILE_IO_

#include "types_def.hpp"
#include "digest_common.hpp"

class FileInfo
{
private:
    const char* m_fileName;   // includes path
    uint64 m_fileLength;


    Digester** m_hashers;
    DigestResult* m_results;
    int m_hashCount;
};


class BufferHashStatus
{
private:
    enum { NOT_RUN, RUNNING, DONE } m_status;

    // Lock ?
};


class FileInputBuffer
{
private:
    char* m_buffer;
    size_t m_bufferSize;

    FileInfo* m_file;
    uint64 m_offset;   // offset in file
    size_t m_length;   // length in buffer, will be same as bufferSize, unless last section
    bool m_reachEOF;        // end of file flag

    BufferHashStatus* m_statusArr;
    int m_hashCount;

public:
    FileInputBuffer(size_t bufferSize);
    ~FileInputBuffer();

};

void InitFileInputBuffer();


#endif  /* FILE_IO_ */

