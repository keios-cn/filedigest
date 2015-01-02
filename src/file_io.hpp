

#ifndef FILE_IO_
#define FILE_IO_

#include "types_def.hpp"

class FileInfo
{
private:
    const char* fileName;   // includes path
    uint64 fileLength;
};


class FileInputBuffer
{
private:
    char* buffer;
    int bufferSize;

    FileInfo* file;
    uint64 offset;   // offset in file
    int length;      // length in buffer, will be same as bufferSize, unless last section
    bool eof;        // end of file flag

};


#endif  /* FILE_IO_ */

