//------------------------------------------------------------------------------
//  StringConverter.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Assertion.h"
#include "StringConverter.h"
#include "Ext/ConvertUTF/ConvertUTF.h"
#include <cstdlib>
#include <cstring>
#include <cwchar>

namespace Oryol {

//------------------------------------------------------------------------------
static void
DumpWarning(ConversionResult convRes) {
    const char* err = "";
    if (sourceExhausted == convRes) err = "sourceExhausted";
    else if (targetExhausted == convRes) err = "targetExhausted";
    else if (sourceIllegal == convRes) err = "sourceIllegal";
    o_warn("StringConverter: conversion failed with '%s'\n", err);
}

//------------------------------------------------------------------------------
int32
StringConverter::UTF8ToWide(const unsigned char* src, int32 srcNumBytes, wchar_t* dst, int32 dstMaxBytes) {
    o_assert((0 != src) && (0 != dst));
    int32 result = 0;
    const UTF8* srcPtr = src;

    // need to keep 1 wchar_t for the terminating 0
    ConversionResult convRes = conversionOK;
    wchar_t* dstEndPtr = dst + ((dstMaxBytes / sizeof(wchar_t)) - 1);
    o_assert(dstEndPtr > dst);
    if (sizeof(wchar_t) == 4) {
        UTF32* dstPtr = (UTF32*)dst;
        convRes = ConvertUTF8toUTF32(&srcPtr, src + srcNumBytes, &dstPtr, (UTF32*) dstEndPtr, strictConversion);
        o_assert(dstPtr < (UTF32*) (dst + (dstMaxBytes / sizeof(wchar_t))));
        *dstPtr = 0;
        if (conversionOK == convRes) {
            result = int32(dstPtr - (UTF32*)dst) + 1;
        }
    } 
    else {
        o_assert(2 == sizeof(wchar_t));
        UTF16* dstPtr = (UTF16*) dst;
        convRes = ConvertUTF8toUTF16(&srcPtr, src + srcNumBytes, &dstPtr, (UTF16*) dstEndPtr, strictConversion);
        o_assert(dstPtr < (UTF16*) (dst + (dstMaxBytes / sizeof(wchar_t))));
        *dstPtr = 0;
        if (conversionOK == convRes) {
            result = int32(dstPtr - (UTF16*)dst) + 1;
        }
    }
    if (conversionOK != convRes) {
        DumpWarning(convRes);
        return 0;
    }
    return result;
}

//------------------------------------------------------------------------------
int32
StringConverter::WideToUTF8(const wchar_t* src, int32 srcNumChars, unsigned char* dst, int32 dstMaxBytes) {
    o_assert((0 != src) && (0 != dst));
    int32 result = 0;
    UTF8* dstPtr = dst;

    // need to keep 1 char free for 0-termination
    ConversionResult convRes = conversionOK;
    UTF8* dstEnd = (dst + dstMaxBytes) - 1;
    o_assert(dstEnd > dst);
    if (sizeof(wchar_t) == 4) {
        o_assert(4 == sizeof(wchar_t));
        const UTF32* srcPtr = (const UTF32*) src;
        const UTF32* srcEndPtr = (UTF32*) src + srcNumChars;
        convRes = ConvertUTF32toUTF8(&srcPtr, srcEndPtr, &dstPtr, dstEnd, strictConversion);
        o_assert(dstPtr < (dst + dstMaxBytes));
        *dstPtr = 0;
        if (conversionOK == convRes) {
            result = int32(dstPtr - dst) + 1;
        }
    }
    else {
        o_assert(2 == sizeof(wchar_t));
        const UTF16* srcPtr = (UTF16*) src;
        const UTF16* srcEndPtr = (UTF16*) src + srcNumChars;
        convRes = ConvertUTF16toUTF8(&srcPtr, srcEndPtr, &dstPtr, dstEnd, strictConversion);
        o_assert(dstPtr < (dst + dstMaxBytes));
        *dstPtr = 0;
        if (conversionOK == convRes) {
            result = int32(dstPtr - dst) + 1;
        }
    }
    if (conversionOK != convRes) {
        DumpWarning(convRes);
        return 0;
    }
    return result;
}

//------------------------------------------------------------------------------
String
StringConverter::WideToUTF8(const wchar_t* wide, int32 numWideChars) {
    String converted;
    o_assert(0 != wide);
    if (numWideChars > 0) {
        if (numWideChars < MaxInternalBufferWChars) {
            unsigned char dstBuf[(MaxInternalBufferWChars + 1) * MaxUTF8Size];
            if (0 < StringConverter::WideToUTF8(wide, numWideChars, dstBuf, (int32)sizeof(dstBuf))) {
                converted = (char*) dstBuf;
            }
        }
        else {
            int32 dstBufSize = (numWideChars * MaxUTF8Size) + 1;
            unsigned char* dstBuf = (unsigned char*) Memory::Alloc(dstBufSize);
            if (0 < StringConverter::WideToUTF8(wide, numWideChars, dstBuf, dstBufSize)) {
                converted = (char*) dstBuf;
            }
            Memory::Free(dstBuf);
            return converted;
        }
    }
    return converted;
}

//------------------------------------------------------------------------------
String
StringConverter::WideToUTF8(const wchar_t* wide) {
    o_assert(0 != wide);
    int numWideChars = (int) std::wcslen(wide);
    return WideToUTF8(wide, numWideChars);
}

//------------------------------------------------------------------------------
String
StringConverter::WideToUTF8(const WideString& wide) {
    return WideToUTF8(wide.AsCStr(), wide.Length());
}

//------------------------------------------------------------------------------
WideString
StringConverter::UTF8ToWide(const unsigned char* src, int32 srcNumBytes) {
    o_assert(0 != src);
    WideString result;
    if (srcNumBytes > 0) {
        // use a char array as buffer (note, even though String::Length() returns the
        // number of bytes in the string, not the number of UTF-8 characters, this is a valid
        // check, since the NumBytes is always >= NumChars!
        if (srcNumBytes < MaxInternalBufferWChars) {
            wchar_t dstBuf[MaxInternalBufferWChars];
            bool success = (0 < StringConverter::UTF8ToWide(src, srcNumBytes, dstBuf, sizeof(dstBuf)));
            if (success) {
                result = dstBuf;
            }
        }
        else {
            // use buffer 
            int32 bufferSize = (srcNumBytes + 1) * sizeof(wchar_t);
            wchar_t* dstBuf = (wchar_t*) Memory::Alloc(bufferSize);
            bool success = (0 < StringConverter::UTF8ToWide(src, srcNumBytes, dstBuf, bufferSize));
            if (success) {
                result = dstBuf;
            }
            Memory::Free((void*) dstBuf);
        }
    }
    return result;
}

//------------------------------------------------------------------------------
WideString
StringConverter::UTF8ToWide(const unsigned char* src) {
    o_assert(0 != src);
    int32 srcNumBytes = (int32) std::strlen((const char*) src);
    return UTF8ToWide(src, srcNumBytes);
}

//------------------------------------------------------------------------------
WideString
StringConverter::UTF8ToWide(const String& src) {
    return UTF8ToWide((uchar*)src.AsCStr(), src.Length());
}

//------------------------------------------------------------------------------
template<> int8
StringConverter::FromString(const String& str) {
    return (int8) std::atoi(str.AsCStr());
}

//------------------------------------------------------------------------------
template<> uint8
StringConverter::FromString(const String& str) {
    return (uint8) std::atoi(str.AsCStr());
}

//------------------------------------------------------------------------------
template<> int16
StringConverter::FromString(const String& str) {
    return (int16) std::atoi(str.AsCStr());
}

//------------------------------------------------------------------------------
template<> uint16
StringConverter::FromString(const String& str) {
    return (uint16) std::atoi(str.AsCStr());
}

//------------------------------------------------------------------------------
template<> int32
StringConverter::FromString(const String& str) {
    return (int32) std::atoi(str.AsCStr());
}

//------------------------------------------------------------------------------
template<> uint32
StringConverter::FromString(const String& str) {
    return (int32) std::atoi(str.AsCStr());
}

//------------------------------------------------------------------------------
template<> float32
StringConverter::FromString(const String& str) {
    return (float32) std::atof(str.AsCStr());
}

//------------------------------------------------------------------------------
template<>
String StringConverter::ToString(const int32& val) {
    StringBuilder sb;
    sb.Format(32, "%i", val);
    return sb.GetString();
}

//------------------------------------------------------------------------------
template<>
String StringConverter::ToString(const float32& val) {
    StringBuilder sb;
    sb.Format(32, "%f", val);
    return sb.GetString();
}

} // namespace Oryol
