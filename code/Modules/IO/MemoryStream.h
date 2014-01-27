#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::IO::MemoryStream
    @brief a dynamic in-memory IO Stream

    A MemoryStream implements a Stream interface to a dynamic (growable)
    memory buffer. The MemoryStream object will keep its contents until
    destroyed or the DiscardContent method is called. 
*/
#include "Core/Config.h"
#include "IO/Stream.h"

namespace Oryol {
namespace IO {

class MemoryStream : public Stream {
    OryolClassDecl(MemoryStream);
public:
    /// constructor
    MemoryStream();
    /// construct with initial capacity and allocation strategy
    MemoryStream(int32 initialCapacity, int32 minGrow=ORYOL_CONTAINER_DEFAULT_MIN_GROW, int32 maxGrow=ORYOL_CONTAINER_DEFAULT_MAX_GROW);
    /// destructor
    virtual ~MemoryStream();
    
    /// set allocation strategy
    void SetAllocStrategy(int32 minGrow, int32 maxGrow=ORYOL_CONTAINER_DEFAULT_MAX_GROW);
    /// get min-grow value
    int32 GetMinGrow() const;
    /// get max-grow value
    int32 GetMaxGrow() const;
    /// get current capacity
    int32 Capacity() const;
    /// increase capacity to hold at least numBytes more (may reallocate)
    void Reserve(int32 numBytes);
    /// trim to actual size (reallocates)
    void Trim();
    
    /// open the stream buffer
    virtual bool Open(OpenMode::Enum mode);
    /// close the stream buffer
    virtual void Close();
    /// discard the content of the stream
    virtual void DiscardContent();
    
    /// write a number of bytes to the stream (returns bytes written)
    virtual int32 Write(void* ptr, int32 numBytes);
    /// map a memory area at the current write position and advance write position
    virtual void* MapWrite(int32 numBytes);
    /// unmap previously mapped memory area
    virtual void UnmapWrite();

    /// read a number of bytes from the stream (returns bytes read)
    virtual int32 Read(void* ptr, int32 numBytes);
    /// map a memory area at the current read-position and advance read position
    virtual void* MapRead(int32 numBytes);
    /// unmap previosuly mapped memory area
    virtual void UnmapRead();

private:
    /// grow to make room for at least numBytes
    void makeRoom(int32 numBytes);
    /// (re-)allocate to a new capacity
    void alloc(int32 newCapacity);
    
    int32 minGrow;
    int32 maxGrow;
    int32 capacity;
    uchar* buffer;
};
    
} // namespace IO
} // namespace Oryol