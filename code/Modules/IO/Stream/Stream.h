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
#include "IO/Core/URL.h"
#include "IO/Core/OpenMode.h"
#include "IO/Core/ContentType.h"

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
    /// set content-type (aka MIME type)
    void SetContentType(const ContentType& ct);
    /// get content-type
    const ContentType& GetContentType() const;
    
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
    /// move write cursor position relative to current write position
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
    /// move read cursor position relative to current read position
    void MoveReadPosition(int32 diff);
    /// get the read cursor position (byte offset from start)
    int32 GetReadPosition() const;
    /// read a number of bytes from the stream (returns bytes read)
    virtual int32 Read(void* ptr, int32 numBytes);
    /// map a memory area at the current read-position, DOES NOT ADVANCE READ-POS!
    virtual const uint8* MapRead(const uint8** outMaxValidPtr);
    /// unmap previously mapped memory area
    virtual void UnmapRead();
    /// return true if currently locked for reading
    bool IsReadMapped() const;
    /// return true if read cursor is at end-of-stream
    bool IsEndOfStream() const;
    
protected:
    URL url;
    ContentType contentType;
    OpenMode::Enum openMode;
    bool isOpen;
    bool isWriteMapped;
    bool isReadMapped;
    int32 size;
    int32 writePosition;
    int32 readPosition;
};

} // namespace IO
} // namespace Oryol
