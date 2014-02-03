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
    /// return true if the stream has been opened in a readable OpenMode
    bool IsReadable() const;
    /// return true if the stream has been opened in a writable OpenMode
    bool IsWritable() const;
    /// get the number of data bytes in the stream
    int32 Size() const;
    
    /// set the write cursor position (byte offset from start)
    void SetWritePosition(int32 pos);
    /// move write cursor position relativ to current write position
    void MoveWritePosition(int32 diff);
    /// get the write cursor position (byte offset from start)
    int32 GetWritePosition() const;
    /// write a number of bytes to the stream (returns bytes written)
    virtual int32 Write(const void* ptr, int32 numBytes);
    /// map a memory area at the current write position and advance write position
    virtual uint8* MapWrite(int32 numBytes);
    /// unmap previously mapped memory area
    virtual void UnmapWrite();
    /// return true if currently mapped for writing
    bool IsWriteMapped() const;
    
    /// set the read cursor position (byte offset from start)
    void SetReadPosition(int32 pos);
    /// move read cursor position relativ to current read position
    void MoveReadPosition(int32 diff);
    /// get the read cursor position (byte offset from start)
    int32 GetReadPosition() const;
    /// read a number of bytes from the stream (returns bytes read)
    virtual int32 Read(void* ptr, int32 numBytes);
    /// map a memory area at the current read-position, DOES NOT ADVANCE READ-POS!
    virtual const uint8* MapRead(const uint8*& outMaxValidPtr);
    /// unmap previosuly mapped memory area
    virtual void UnmapRead();
    /// return true if currently locked for reading
    bool IsReadMapped() const;
    /// return true if read cursor is at end-of-stream
    bool IsEndOfStream() const;
    
protected:
    URL url;
    OpenMode::Enum openMode;
    bool isOpen;
    bool isWriteMapped;
    bool isReadMapped;
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
inline bool
Stream::IsReadable() const {
    return (OpenMode::ReadOnly == this->openMode) ||
           (OpenMode::ReadWrite == this->openMode) ||
           (OpenMode::ReadWriteAppend == this->openMode);
}

//------------------------------------------------------------------------------
inline bool
Stream::IsWritable() const {
    return (OpenMode::WriteOnly == this->openMode) ||
           (OpenMode::WriteAppend == this->openMode) ||
           (OpenMode::ReadWrite == this->openMode) ||
           (OpenMode::ReadWriteAppend == this->openMode);
}

//------------------------------------------------------------------------------
inline bool
Stream::IsWriteMapped() const {
    return this->isWriteMapped;
}

//------------------------------------------------------------------------------
inline bool
Stream::IsReadMapped() const {
    return this->isReadMapped;
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
Stream::Size() const {
    return this->size;
}

//------------------------------------------------------------------------------
inline void
Stream::SetWritePosition(int32 pos) {
    o_assert(this->isOpen);
    o_assert((pos >= 0) && (pos <= this->size));
    this->writePosition = pos;
}

//------------------------------------------------------------------------------
inline void
Stream::MoveWritePosition(int32 diff) {
    o_assert(this->isOpen);
    int32 newPos = this->writePosition + diff;
    o_assert((newPos >= 0) && (newPos <= this->size));
    this->writePosition = newPos;
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
    o_assert((pos >= 0) && (pos <= this->size));
    this->readPosition = pos;
}

//------------------------------------------------------------------------------
inline void
Stream::MoveReadPosition(int32 diff) {
    o_assert(this->isOpen);
    int32 newPos = this->readPosition + diff;
    o_assert((newPos >= 0) && (newPos <= this->size));
    this->readPosition = newPos;
}

//------------------------------------------------------------------------------
inline int32
Stream::GetReadPosition() const {
    return this->readPosition;
}

//------------------------------------------------------------------------------
inline bool
Stream::IsEndOfStream() const {
    return this->readPosition == this->size;
}

} // namespace IO
} // namespace Oryol
