#pragma once
//------------------------------------------------------------------------------
/**
    @class Messaging::Serializer
    @brief Serializer template class for a specific data type
    
    This is a simple template class which knows how to 
    encode/decode a specific data type (the template arg) to and from
    a plain-old-data memory region.
*/
#include "Core/Types.h"
#include "Core/String/String.h"
#include "Core/String/StringAtom.h"

namespace Oryol {
namespace Messaging {
    
template<typename TYPE> class Serializer {
public:
    /// return the encoded size of the provided value
    static int32 SizeOf(const TYPE& val);
    /// encode to plain-old-data representation, return pointer to next pos, or nullptr if not enough space
    static uchar* Encode(const TYPE& val, uchar* dstPtr, int32 maxBytes);
    /// decode from plain-old-data representation, returns pointer to next pos, or nullptr if not enough data
    static const uchar* Decode(const uchar* srcPtr, int32 maxBytes, TYPE& outVal);
    /// return the encoded size for an array of values
    static int32 SizeOf(const Core::Array<TYPE>& vals);
    /// encode an array of values
    static uchar* Encode(const Core::Array<TYPE>& vals, uchar* dstPtr, int32 maxBytes);
    /// decode an array of values
    static const uchar* Decode(const uchar* srcPtr, int32 maxBytes, Core::Array<TYPE>& outVals);
};

//------------------------------------------------------------------------------
template<typename TYPE> int32
Serializer<TYPE>::SizeOf(const TYPE& val) {
    static_assert(std::is_pod<TYPE>::value, "Serializer::SizeOf(): Type not POD, must provide specialization!");
    return sizeof(TYPE);
}
    
//------------------------------------------------------------------------------
template<typename TYPE> uchar*
Serializer<TYPE>::Encode(const TYPE& val, uchar* dstPtr, int32 maxBytes) {
    static_assert(std::is_pod<TYPE>::value, "Serializer::Encode(): Type not POD, must provide specialization!");
    if (int32(sizeof(TYPE)) <= maxBytes) {
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
template<typename TYPE> const uchar*
Serializer<TYPE>::Decode(const uchar* srcPtr, int32 maxBytes, TYPE& outVal) {
    static_assert(std::is_pod<TYPE>::value, "Serializer::Encode(): Type not POD, must provide specialization!");
    if (int32(sizeof(TYPE)) <= maxBytes) {
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
template<> int32
Serializer<Core::String>::SizeOf(const Core::String& val) {
    return sizeof(int32) + val.Length();
}
    
//------------------------------------------------------------------------------
template<> uchar*
Serializer<Core::String>::Encode(const Core::String& val, uchar* dstPtr, int32 maxBytes) {
    if (SizeOf(val) <= maxBytes) {
        const int32 len = val.Length();
        dstPtr = Serializer<int32>::Encode(len, dstPtr, maxBytes);
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
template<> const uchar*
Serializer<Core::String>::Decode(const uchar* srcPtr, int32 maxBytes, Core::String& outVal) {
    if (int32(sizeof(int32)) < maxBytes) {
        // read length
        int32 len = 0;
        srcPtr = Serializer<int32>::Decode(srcPtr, sizeof(int32), len);
        o_assert(nullptr != srcPtr);
        if (len <= (maxBytes - int32(sizeof(int32)))) {
            // read and assign string data
            outVal.Assign((const char*)srcPtr, 0, len);
            return srcPtr + len;
        }
    }
    // fallthrough: not enough space
    return nullptr;
}
    
//------------------------------------------------------------------------------
template<> int32
Serializer<Core::StringAtom>::SizeOf(const Core::StringAtom& val) {
    return sizeof(int32) + val.Length();
}
    
//------------------------------------------------------------------------------
template<> uchar*
Serializer<Core::StringAtom>::Encode(const Core::StringAtom& val, uchar* dstPtr, int32 maxBytes) {
    if (SizeOf(val) <= maxBytes) {
        const int32 len = val.Length();
        dstPtr = Serializer<int32>::Encode(len, dstPtr, maxBytes);
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
template<> const uchar*
Serializer<Core::StringAtom>::Decode(const uchar* srcPtr, int32 maxBytes, Core::StringAtom& outVal) {
    if (int32(sizeof(int32)) < maxBytes) {
        // read length
        int32 len = 0;
        srcPtr = Serializer<int32>::Decode(srcPtr, sizeof(int32), len);
        o_assert(nullptr != srcPtr);
        if (len < (maxBytes - int32(sizeof(int32)))) {
            // FIXME: meh, must create temp string
            Core::String str((const char*) srcPtr, 0, len);
            outVal = str;
            return srcPtr + len;
        }
    }
    // fallthrough: not enough space
    return nullptr;
}

//------------------------------------------------------------------------------
template<typename TYPE> int32
Serializer<TYPE>::SizeOf(const Core::Array<TYPE>& vals) {
    if (vals.Size() > 0) {
        int32 size = sizeof(int32);
        for (const TYPE& val : vals) {
            size += Serializer<TYPE>::SizeOf(val);
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
template<typename TYPE> uchar*
Serializer<TYPE>::Encode(const Core::Array<TYPE>& vals, uchar* dstPtr, int32 maxBytes) {
    if (SizeOf(vals) <= maxBytes) {
        const int32 numElements = vals.Size();
        dstPtr = Serializer<int32>::Encode(numElements, dstPtr, maxBytes);
        if (numElements > 0) {
            maxBytes -= sizeof(int32);
            for (const TYPE& val : vals) {
                dstPtr = Serializer<TYPE>::Encode(val, dstPtr, maxBytes);
                maxBytes -= Serializer<TYPE>::SizeOf(val);
            }
            o_assert(maxBytes >= 0);
        }
        // success
        return dstPtr;
    }
    // fallthrough: not enough space
    return nullptr;
}

//------------------------------------------------------------------------------
template<typename TYPE> const uchar*
Serializer<TYPE>::Decode(const uchar* srcPtr, int32 maxBytes, Core::Array<TYPE>& outVals) {
    o_assert(outVals.Size() == 0);
    if (int32(sizeof(int32)) < maxBytes) {
        // read number of elements
        int32 numElements = 0;
        srcPtr = Serializer<int32>::Decode(srcPtr, sizeof(int32), numElements);
        if (numElements > 0) {
            outVals.Reserve(numElements);
            maxBytes -= sizeof(int32);
            for (int32 i = 0; i < numElements; i++) {
                TYPE val;
                srcPtr = Serializer<TYPE>::Decode(srcPtr, maxBytes, val);
                if (nullptr == srcPtr) {
                    // not enough data
                    outVals.Clear();
                    return nullptr;
                }
                outVals.AddBack(val);
                maxBytes -= Serializer<TYPE>::SizeOf(val);
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