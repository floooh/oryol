#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::IO::BinaryStreamWriter
    @brief stream writer for writing binary-encoded data to a stream
    
    A StreamWriter for writing typed data to a binary stream. For complex
    data types, a specialized Serializer class must exist (the common
    low-level types are already handled by the default Serializer 
    implementation).
*/
#include "IO/Stream/StreamWriter.h"
#include "IO/Stream/Stream.h"
#include "Messaging/Serializer.h"

namespace Oryol {
namespace IO {
    
class BinaryStreamWriter : public StreamWriter {
    OryolClassDecl(BinaryStreamWriter);
public:
    /// default constructor
    BinaryStreamWriter() { };
    /// construct with stream
    BinaryStreamWriter(const Core::Ptr<Stream>& stream) :
        StreamWriter(stream) {
    };
    /// write a typed value to the stream
    template<typename TYPE> bool Write(const TYPE& val);
    /// write an array of values
    template<typename TYPE> bool Write(const Core::Array<TYPE>& vals);
};

//------------------------------------------------------------------------------
template<typename TYPE> bool
BinaryStreamWriter::Write(const TYPE& val) {
    // hmm this is quite expensive :/
    // better to Map/Unmap only once and call Encode many times
    // (which is a case for a code generator)
    bool retval = false;
    int32 neededSize = Messaging::Serializer::EncodedSize<TYPE>(val);
    uint8* dstPtr = this->stream->MapWrite(neededSize);
    if (nullptr != dstPtr) {
        const uint8* endPtr = Messaging::Serializer::Encode<TYPE>(val, dstPtr, dstPtr + neededSize);
        if (nullptr != endPtr) {
            o_assert((endPtr - dstPtr) == neededSize);
            retval = true;
        }
        this->stream->UnmapWrite();
    }
    return retval;
}
    
//------------------------------------------------------------------------------
template<typename TYPE> bool
BinaryStreamWriter::Write(const Core::Array<TYPE>& vals) {
    bool retval = false;
    int32 neededSize = Messaging::Serializer::EncodedArraySize<TYPE>(vals);
    uint8* dstPtr = this->stream->MapWrite(neededSize);
    if (nullptr != dstPtr) {
        const uint8* endPtr = Messaging::Serializer::EncodeArray<TYPE>(vals, dstPtr, dstPtr + neededSize);
        if (nullptr != endPtr) {
            o_assert((endPtr - dstPtr) == neededSize);
            retval = true;
        }
        this->stream->UnmapWrite();
    }
    return retval;
}

} // namespace BinaryStreamWriter
} // namespace Oryol