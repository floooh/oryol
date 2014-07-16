#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::IO::MemoryStream
    @brief a dynamic in-memory IO Stream

    A MemoryStream implements a Stream interface to a dynamic (growable)
    memory buffer. The MemoryStream object will keep its contents until
    destroyed or the DiscardContent method is called. 
*/
#include "IO/Core/IOConfig.h"
#include "IO/Stream/Stream.h"

namespace Oryol {
namespace IO {

class MemoryStream : public Stream {
    OryolClassDecl(MemoryStream);
public:
    /// constructor
    MemoryStream();
    /// construct with initial capacity and allocation strategy
    MemoryStream(int32 initialCapacity, int32 minGrow=ORYOL_STREAM_DEFAULT_MIN_GROW, int32 maxGrow=ORYOL_STREAM_DEFAULT_MAX_GROW);
    /// destructor
    virtual ~MemoryStream();
    
    /// set allocation strategy
    void SetAllocStrategy(int32 minGrow, int32 maxGrow=ORYOL_STREAM_DEFAULT_MAX_GROW);
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
    
    /// discard the content of the stream
    virtual void DiscardContent() override;
    
    /// write a number of bytes to the stream (returns bytes written)
    virtual int32 Write(const void* ptr, int32 numBytes) override;
    /// map a memory area at the current write position and advance write position
    virtual uint8* MapWrite(int32 numBytes) override;

    /// read a number of bytes from the stream (returns bytes read), numBytes can be EndOfStream
    virtual int32 Read(void* ptr, int32 numBytes) override;
    /// map a memory area at the current read-position, DOES NOT ADVANCE READ-POS!
    virtual const uint8* MapRead(const uint8** outMaxValidPtr) override;

private:
    /// check if there is enough room for writing numBytes
    bool hasRoom(int32 numBytes) const;
    /// grow to make room for at least numBytes
    void makeRoom(int32 numBytes);
    /// (re-)allocate to a new capacity
    void alloc(int32 newCapacity);
    /// increment writePosition, and probably size
    void incrWritePosition(int32 numBytes);
    /// increment readPosition
    void incrReadPosition(int32 numBytes);
    
    int32 minGrow;
    int32 maxGrow;
    int32 capacity;
    uchar* buffer;
};
    
} // namespace IO
} // namespace Oryol