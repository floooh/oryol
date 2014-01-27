#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::IO::StreamBuffer
    @brief Base class for IO stream buffers
    
    Base class for stream buffers. Stream buffers are used by Stream object
    as data sink and/or source.
*/
#include "Core/Types.h"
#include "IO/URL.h"
#include "IO/Mode.h"

namespace Oryol {
namespace IO {

class StreamBuffer {
public:
    /// constructor
    StreamBuffer();
    /// destructor
    ~StreamBuffer();
    
    /// set URL
    void SetURL(const URL& u);
    /// get URL
    const URL& GetURL() const;
    /// set open-mode
    void SetMode(Mode m);
    /// get open-mode
    Mode GetMode() const;
    
    /// open the stream buffer
    bool Open();
    /// close the stream buffer
    void Close();
    /// return true if currently open
    bool IsOpen() const;
    /// discard the content of the stream
    void DiscardContent();

    /// get the number of data bytes in the stream
    int32 GetSize() const;
    
    /// set the write cursor position (byte offset from start)
    void SetWritePosition(int32 pos);
    /// get the write cursor position (byte offset from start)
    int32 GetWritePosition() const;
    /// write a number of bytes to the stream (returns bytes written)
    int32 Write(void* ptr, int32 numBytes);
    /// map a memory area at the current write position and advance write position
    void* MapWrite(int32 numBytes);
    /// unmap previously mapped memory area
    void UnmapWrite();
    
    /// set the read cursor position (byte offset from start)
    void SetReadPosition(int32 pos);
    /// get the read cursor position (byte offset from start)
    int32 GetReadPosition() const;
    /// read a number of bytes from the stream (returns bytes read)
    int32 Read(void* ptr, int32 numBytes);
    /// map a memory area at the current read-position and advance read position
    void* MapRead(int32 numBytes);
    /// unmap previosuly mapped memory area
    void UnmapRead();
    /// return true if read cursor is at end-of-file
    bool IsEof() const;
    
protected:
    URL url;
    Mode mode;
    bool isOpen;
    int32 size;
    int32 writePosition;
    int32 readPosition;
};

//------------------------------------------------------------------------------
inline bool
StreamBuffer::IsOpen() const {
    return this->isOpen;
}

//------------------------------------------------------------------------------
inline void
StreamBuffer::SetURL(const URL& url_) {
    o_assert(!this->isOpen);
    this->url = url_;
}

//------------------------------------------------------------------------------
inline const URL&
StreamBuffer::GetURL() const {
    return this->url;
}

//------------------------------------------------------------------------------
inline void
StreamBuffer::SetMode(Mode mode_) {
    o_assert(!this->isOpen);
    this->mode = mode_;
}

//------------------------------------------------------------------------------
inline Mode
StreamBuffer::GetMode() const {
    return this->mode;
}

//------------------------------------------------------------------------------
inline int32
StreamBuffer::GetSize() const {
    return this->size;
}

//------------------------------------------------------------------------------
inline void
StreamBuffer::SetWritePosition(int32 pos) {
    o_assert(this->isOpen);
    o_assert(pos >= 0 && pos < this->size);
    this->writePosition = pos;
}

//------------------------------------------------------------------------------
inline int32
StreamBuffer::GetWritePosition() const {
    return this->writePosition;
}

//------------------------------------------------------------------------------
inline void
StreamBuffer::SetReadPosition(int32 pos) {
    o_assert(this->isOpen);
    o_assert(pos >= 0 && pos < this->size);
    this->readPosition = pos;
}

//------------------------------------------------------------------------------
inline int32
StreamBuffer::GetReadPosition() const {
    return this->readPosition;
}

//------------------------------------------------------------------------------
inline bool
StreamBuffer::IsEof() const {
    return this->readPosition == this->size;
}

} // namespace IO
} // namespace Oryol
