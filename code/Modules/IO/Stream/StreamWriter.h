#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::IO::StreamWriter
    @brief base class for stream writers
    
    Stream writers provide a way to write structured/formatted data 
    to a stream object. To use, create a StreamWriter and attach an
    already open Stream object. The stream object must have been opened
    in a Writable open mode.
*/
#include "Core/RefCounted.h"

namespace Oryol {
namespace IO {
    
class Stream;

class StreamWriter : public Core::RefCounted {
    OryolClassDecl(StreamWriter);
public:
    /// default constructor
    StreamWriter();
    /// construct with Stream object (Stream object must be open)
    StreamWriter(const Core::Ptr<Stream>& stream);
    /// destructor
    virtual ~StreamWriter();
    
    /// attach stream object (must be open, replaces previous object)
    virtual void AttachStream(const Core::Ptr<Stream>& stream);
    /// detach stream object
    virtual void DetachStream();
    /// get currently attached stream object
    const Core::Ptr<Stream>& GetStream() const;
    
protected:
    Core::Ptr<Stream> stream;
};
    
} // namespace IO
} // namespace Oryol
    