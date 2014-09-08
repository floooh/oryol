#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Messaging::Serializer
    @brief Serializer template class for a specific data type
    
    This is a simple template class which knows how to 
    encode/decode a specific data type (the template arg) to and from
    a plain-old-data memory region.
*/
#include <string.h>
#include "Core/Types.h"
#include "Core/String/String.h"
#include "Core/String/StringAtom.h"

namespace Oryol {
namespace Messaging {
    
class Serializer {
public:
    /// return the encoded size of the provided value
    template<typename TYPE> static int32 EncodedSize(const TYPE& val);
    /// encode to plain-old-data representation, return pointer to next pos, or nullptr if not enough space
    template<typename TYPE> static uint8* Encode(const TYPE& val, uint8* dstPtr, const uint8* maxPtr);
    /// decode from plain-old-data representation, returns pointer to next pos, or nullptr if not enough data
    template<typename TYPE> static const uint8* Decode(const uint8* srcPtr, const uint8* maxPtr, TYPE& outVal);
    /// return the encoded size for an array of values
    template<typename TYPE> static int32 EncodedArraySize(const Core::Array<TYPE>& vals);
    /// encode an array of values
    template<typename TYPE> static uint8* EncodeArray(const Core::Array<TYPE>& vals, uint8* dstPtr, const uint8* maxPtr);
    /// decode an array of values
    template<typename TYPE> static const uint8* DecodeArray(const uint8* srcPtr, const uint8* maxPtr, Core::Array<TYPE>& outVals);
};

//------------------------------------------------------------------------------
template<typename TYPE> inline int32
Serializer::EncodedSize(const TYPE& val) {
    static_assert(std::is_pod<TYPE>::value, "Serializer::SizeOf(): Type not POD, must provide specialization!");
    return sizeof(TYPE);
}

//------------------------------------------------------------------------------
template<typename TYPE> inline uint8*
Serializer::Encode(const TYPE& val, uint8* dstPtr, const uint8* maxPtr) {
    static_assert(std::is_pod<TYPE>::value, "Serializer::Encode(): Type not POD, must provide specialization!");
    if ((dstPtr + sizeof(TYPE)) <= maxPtr) {
        // must copy byte-wise because of alignment restrictions
        // hopefully this will be an intrinsic
        memcpy(dstPtr, &val, sizeof(TYPE));
        return dstPtr + sizeof(TYPE);
    }
    else {
        // not enough space
        return nullptr;
    }
}

//------------------------------------------------------------------------------
template<typename TYPE> inline const uint8*
Serializer::Decode(const uint8* srcPtr, const uint8* maxPtr, TYPE& outVal) {
    static_assert(std::is_pod<TYPE>::value, "Serializer::Encode(): Type not POD, must provide specialization!");
    if ((srcPtr + sizeof(TYPE)) <= maxPtr) {
        // must copy byte-wise because of alignment restrictions
        // hopefully this will be an intrinsic
        memcpy(&outVal, srcPtr, sizeof(TYPE));
        return srcPtr + sizeof(TYPE);
    }
    else {
        // not enough data
        return nullptr;
    }
}

//------------------------------------------------------------------------------
template<> inline int32
Serializer::EncodedSize(const Core::String& val) {
    return sizeof(int32) + val.Length();
}
    
//------------------------------------------------------------------------------
template<> inline uint8*
Serializer::Encode(const Core::String& val, uint8* dstPtr, const uint8* maxPtr) {
    if ((dstPtr + EncodedSize(val)) <= maxPtr) {
        const int32 len = val.Length();
        dstPtr = Serializer::Encode<int32>(len, dstPtr, maxPtr);
        o_assert(nullptr != dstPtr);
        memcpy(dstPtr, val.AsCStr(), len);
        return dstPtr + len;
    }
    else {
        // not enough space
        return nullptr;
    }
}
    
//------------------------------------------------------------------------------
template<> inline const uint8*
Serializer::Decode(const uint8* srcPtr, const uint8* maxPtr, Core::String& outVal) {
    if ((srcPtr + sizeof(int32)) <= maxPtr) {
        // read length
        int32 len = 0;
        srcPtr = Serializer::Decode<int32>(srcPtr, maxPtr, len);
        o_assert(nullptr != srcPtr);
        if ((srcPtr + len) <= maxPtr) {
            // read and assign string data
            outVal.Assign((const char*)srcPtr, 0, len);
            return srcPtr + len;
        }
    }
    // fallthrough: not enough space
    return nullptr;
}
    
//------------------------------------------------------------------------------
template<> inline int32
Serializer::EncodedSize(const Core::StringAtom& val) {
    return sizeof(int32) + val.Length();
}
    
//------------------------------------------------------------------------------
template<> inline uint8*
Serializer::Encode(const Core::StringAtom& val, uint8* dstPtr, const uint8* maxPtr) {
    if ((dstPtr + EncodedSize(val)) <= maxPtr) {
        const int32 len = val.Length();
        dstPtr = Serializer::Encode<int32>(len, dstPtr, maxPtr);
        o_assert(nullptr != dstPtr);
        memcpy(dstPtr, val.AsCStr(), len);
        return dstPtr + len;
    }
    else {
        // not enough space
        return nullptr;
    }
}
    
//------------------------------------------------------------------------------
template<> inline const uint8*
Serializer::Decode(const uint8* srcPtr, const uint8* maxPtr, Core::StringAtom& outVal) {
    if ((srcPtr + sizeof(int32)) <= maxPtr) {
        // read length
        int32 len = 0;
        srcPtr = Serializer::Decode<int32>(srcPtr, maxPtr, len);
        o_assert(nullptr != srcPtr);
        if ((srcPtr + len) <= maxPtr) {
            /// @todo: meh, must create temp string
            Core::String str((const char*) srcPtr, 0, len);
            outVal = str;
            return srcPtr + len;
        }
    }
    // fallthrough: not enough space
    return nullptr;
}
    
//------------------------------------------------------------------------------
template<typename TYPE> inline int32
Serializer::EncodedArraySize(const Core::Array<TYPE>& vals) {
    if (vals.Size() > 0) {
        int32 size = sizeof(int32);
        for (const TYPE& val : vals) {
            size += Serializer::EncodedSize<TYPE>(val);
        }
        return size;
    }
    else {
        // if the array is empty, we still need to write the number of elements
        // (which is 0)
        return sizeof(int32);
    }
}
    
//------------------------------------------------------------------------------
template<typename TYPE> inline uint8*
Serializer::EncodeArray(const Core::Array<TYPE>& vals, uint8* dstPtr, const uint8* maxPtr) {
    if ((dstPtr + EncodedArraySize<TYPE>(vals)) <= maxPtr) {
        const int32 numElements = vals.Size();
        dstPtr = Serializer::Encode<int32>(numElements, dstPtr, maxPtr);
        if (numElements > 0) {
            for (const TYPE& val : vals) {
                dstPtr = Serializer::Encode<TYPE>(val, dstPtr, maxPtr);
                o_assert(nullptr != dstPtr);
            }
        }
        // success
        return dstPtr;
    }
    // fallthrough: not enough space
    return nullptr;
}
    
//------------------------------------------------------------------------------
template<typename TYPE> inline const uint8*
Serializer::DecodeArray(const uint8* srcPtr, const uint8* maxPtr, Core::Array<TYPE>& outVals) {
    o_assert(outVals.Size() == 0);
    if ((srcPtr + sizeof(int32)) < maxPtr) {
        // read number of elements
        int32 numElements = 0;
        srcPtr = Serializer::Decode<int32>(srcPtr, maxPtr, numElements);
        if (numElements > 0) {
            outVals.Reserve(numElements);
            for (int32 i = 0; i < numElements; i++) {
                TYPE val;
                srcPtr = Serializer::Decode<TYPE>(srcPtr, maxPtr, val);
                if (nullptr == srcPtr) {
                    // not enough data
                    outVals.Clear();
                    return nullptr;
                }
                outVals.Add(val);
            }
        }
        // success
        return srcPtr;
    }
    // fallthrough not enough data
    return nullptr;
}

} // namespace Messagin
} // namespace Oryol