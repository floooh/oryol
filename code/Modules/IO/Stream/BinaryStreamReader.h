#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::IO::BinaryStreamReader
    @brief stream reader for reading binary-encoded data from a stream
    
    A StreamReader for reading typed data from a binary stream. For complex
    data types, a specialized Serializer class must exist (the common
    low-level types are already handled by the default Serializer
    implementation).
*/
#include "IO/Stream/StreamReader.h"
#include "IO/Stream/Stream.h"
#include "Messaging/Serializer.h"

namespace Oryol {
namespace IO {

class BinaryStreamReader : public StreamReader {
    OryolClassDecl(BinaryStreamReader);
public:
    /// default constructor
    BinaryStreamReader() { };
    /// construct with stream
    BinaryStreamReader(const Core::Ptr<Stream>& stream) :
        StreamReader(stream) {
    };
    /// read a typed value from the stream
    template<typename TYPE> bool Read(TYPE& val);
    /// read array of typed values from stream
    template<typename TYPE> bool Read(Core::Array<TYPE>& vals);
};

//------------------------------------------------------------------------------
template<typename TYPE> bool
BinaryStreamReader::Read(TYPE& val) {
    // hmm this is expensive :/
    // better to Map/Unmap only once and call Decode many times
    // (which is a case for a code generator)
    bool retval = false;
    const uint8* maxValidPtr = nullptr;
    const uint8* srcPtr = this->stream->MapRead(&maxValidPtr);
    if (srcPtr != nullptr) {
        const uint8* endPtr = Messaging::Serializer::Decode<TYPE>(srcPtr, maxValidPtr, val);
        if (endPtr != nullptr) {
            const int32 numBytesDecoded = endPtr - srcPtr;
            o_assert(numBytesDecoded == Messaging::Serializer::EncodedSize<TYPE>(val));
            this->stream->MoveReadPosition(numBytesDecoded);
            retval = true;
        }
        this->stream->UnmapRead();
    }
    return retval;
}

//------------------------------------------------------------------------------
template<typename TYPE> bool
BinaryStreamReader::Read(Core::Array<TYPE>& val) {
    // hmm this is expensive :/
    // better to Map/Unmap only once and call Decode many times
    // (which is a case for a code generator)
    bool retval = false;
    const uint8* maxValidPtr = nullptr;
    const uint8* srcPtr = this->stream->MapRead(&maxValidPtr);
    if (nullptr != srcPtr) {
        const uint8* endPtr = Messaging::Serializer::DecodeArray<TYPE>(srcPtr, maxValidPtr, val);
        if (nullptr != endPtr) {
            const int32 numBytesDecoded = endPtr - srcPtr;
            o_assert(numBytesDecoded == Messaging::Serializer::EncodedArraySize<TYPE>(val));
            this->stream->MoveReadPosition(numBytesDecoded);
            retval = true;
        }
        this->stream->UnmapRead();
    }
    return retval;
}

} // namespace IO
} // namespace Core
