#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::IO::MemoryStreamBuffer
    
    A growable in-memory StreamBuffer.
*/
#include "IO/StreamBuffer.h"

namespace Oryol {
namespace IO {

class MemoryStreamBuffer : public StreamBuffer {
public:
    /// constructor
    MemoryStreamBuffer();
    /// destructor
    ~MemoryStreamBuffer();
    
    /// open the stream buffer
    bool Open(const URL& url, Mode mode);
    /// close the stream buffer
    void Close();
    /// discard the content of the stream
    void DiscardContent();
    
    /// write a number of bytes to the stream (returns bytes written)
    int32 Write(void* ptr, int32 numBytes);
    /// set the write cursor position (byte offset from start)
    void SetWritePosition(int32 offset);
    /// get the write cursor position (byte offset from start)
    int32 GetWritePosition() const;
    /// map a memory area at the current write position and advance write position
    void* MapWrite(int32 numBytes);
    /// unmap previously mapped memory area
    void UnmapWrite();

    /// read a number of bytes from the stream (returns bytes read)
    int32 Read(void* ptr, int32 numBytes);
    /// set the read cursor position (byte offset from start)
    void SetReadPosition(int32 offset);
    /// get the read cursor position (byte offset from start)
    int32 GetReadPosition() const;
    /// map a memory area at the current read-position and advance read position
    void* MapRead(int32 numBytes);
    /// unmap previosuly mapped memory area
    void UnmapRead();
};
    
} // namespace IO
} // namespace Oryol