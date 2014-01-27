#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::IO::Stream
    @brief Base class for IO streams
    
    IO streams are data sinks or data sources with a tradition
    Open/Read/Write/Close interface. Unlike POSIX file functions
    they maintain a separate read- and write-cursor position.
*/
#include "Core/RefCounted.h"
#include "IO/URL.h"
#include "IO/OpenMode.h"

namespace Oryol {
namespace IO {

class Stream : public Core::RefCounted {
    OryolClassDecl(Stream);
public:
    /// constructor
    Stream();
    /// destructor
    virtual ~Stream();
    
    /// set URL
    void SetURL(const URL& u);
    /// get URL
    const URL& GetURL() const;
    
    /// open the stream buffer
    virtual bool Open(OpenMode::Enum mode);
    /// close the stream buffer
    virtual void Close();
    /// return true if currently open
    bool IsOpen() const;
    /// discard the content of the stream
    virtual void DiscardContent();

    /// get current open-mode (only valid inside Open/Close)
    OpenMode::Enum GetOpenMode() const;
    /// get the number of data bytes in the stream
    int32 GetSize() const;
    
    /// set the write cursor position (byte offset from start)
    void SetWritePosition(int32 pos);
    /// get the write cursor position (byte offset from start)
    int32 GetWritePosition() const;
    /// write a number of bytes to the stream (returns bytes written)
    virtual int32 Write(void* ptr, int32 numBytes);
    /// map a memory area at the current write position and advance write position
    virtual void* MapWrite(int32 numBytes);
    /// unmap previously mapped memory area
    virtual void UnmapWrite();
    
    /// set the read cursor position (byte offset from start)
    void SetReadPosition(int32 pos);
    /// get the read cursor position (byte offset from start)
    int32 GetReadPosition() const;
    /// read a number of bytes from the stream (returns bytes read)
    virtual int32 Read(void* ptr, int32 numBytes);
    /// map a memory area at the current read-position and advance read position
    virtual void* MapRead(int32 numBytes);
    /// unmap previosuly mapped memory area
    virtual void UnmapRead();
    /// return true if read cursor is at end-of-file
    bool IsEof() const;
    
protected:
    URL url;
    OpenMode::Enum openMode;
    bool isOpen;
    int32 size;
    int32 writePosition;
    int32 readPosition;
};

//------------------------------------------------------------------------------
inline bool
Stream::IsOpen() const {
    return this->isOpen;
}

//------------------------------------------------------------------------------
inline void
Stream::SetURL(const URL& url_) {
    o_assert(!this->isOpen);
    this->url = url_;
}

//------------------------------------------------------------------------------
inline const URL&
Stream::GetURL() const {
    return this->url;
}

//------------------------------------------------------------------------------
inline OpenMode::Enum
Stream::GetOpenMode() const {
    return this->openMode;
}

//------------------------------------------------------------------------------
inline int32
Stream::GetSize() const {
    return this->size;
}

//------------------------------------------------------------------------------
inline void
Stream::SetWritePosition(int32 pos) {
    o_assert(this->isOpen);
    o_assert(pos >= 0 && pos < this->size);
    this->writePosition = pos;
}

//------------------------------------------------------------------------------
inline int32
Stream::GetWritePosition() const {
    return this->writePosition;
}

//------------------------------------------------------------------------------
inline void
Stream::SetReadPosition(int32 pos) {
    o_assert(this->isOpen);
    o_assert(pos >= 0 && pos < this->size);
    this->readPosition = pos;
}

//------------------------------------------------------------------------------
inline int32
Stream::GetReadPosition() const {
    return this->readPosition;
}

//------------------------------------------------------------------------------
inline bool
Stream::IsEof() const {
    return this->readPosition == this->size;
}

} // namespace IO
} // namespace Oryol
