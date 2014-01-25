//------------------------------------------------------------------------------
//  StringUtil.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core/Macros.h"
#include "StringUtil.h"

namespace Oryol {
namespace Core {

#if ORYOL_WINDOWS
#define o_strtok strtok_s
#else
#define o_strtok strtok_r
#endif
    
//------------------------------------------------------------------------------
int32
StringUtil::Tokenize(const String& str, const char* delims, Array<String>& outTokens) {
    o_assert(nullptr != delims);

    outTokens.Clear();
    String tmpStr = str.MakeCopy();    // this creates a unique copy which will be modified
    char* ptr = const_cast<char*>(tmpStr.AsCStr());
    const char* token;
    char* contextPtr = 0;
    while (0 != (token = o_strtok(ptr, delims, &contextPtr))) {
        outTokens.AddBack(token);
        ptr = 0;
    }
    return outTokens.Size();
}

//------------------------------------------------------------------------------
int32
StringUtil::Tokenize(const String& str, const char* delims, char fence, Array<String>& outTokens)
{
    outTokens.Clear();
    String tmpStr = str.MakeCopy();
    char* ptr = const_cast<char*>(tmpStr.AsCStr());
    char* end = ptr + tmpStr.Length();
    while (ptr < end)
    {
        char* c;
        
        // skip white space
        while (*ptr && std::strchr(delims, *ptr))
        {
            ptr++;
        }
        if (*ptr)
        {
            // check for fenced area
            if ((fence == *ptr) && (0 != (c = std::strchr(++ptr, fence))))
            {
                *c++ = 0;
                outTokens.AddBack(ptr);
                ptr = c;
            }
            else if (0 != (c = std::strpbrk(ptr, delims)))
            {
                *c++ = 0;
                outTokens.AddBack(ptr);
                ptr = c;
            }
            else
            {
                outTokens.AddBack(ptr);
                break;
            }
        }
    }
    return outTokens.Size();
}

//------------------------------------------------------------------------------
int32
StringUtil::FindFirstOf(const String& str, int32 startIndex, const char* delims) {
    o_assert(0 != delims);
    if (startIndex < str.Length()) {
        const char* ptr = str.AsCStr() + startIndex;
        int index = std::strcspn(ptr, delims);
        if (index == str.Length()) {
            return InvalidIndex;
        }
        else {
            return index;
        }
    }
    else {
        return InvalidIndex;
    }
}

//------------------------------------------------------------------------------
int32
StringUtil::FindFirstNotOf(const String& str, int32 startIndex, const char* delims) {
    o_assert(0 != delims);
    if (startIndex < str.Length()) {
        const char* ptr = str.AsCStr() + startIndex;
        int index = std::strspn(ptr, delims);
        if (index == str.Length()) {
            return InvalidIndex;
        }
        else {
            return index;
        }
    }
    else {
        return InvalidIndex;
    }
}

//------------------------------------------------------------------------------
void
StringUtil::Bisect(const String& str, const char* delims, String& left, String& right) {
    o_assert(nullptr != delims);
    o_assert((&str != &left) && (&str != &right));
    
    int32 leftSplitIndex = StringUtil::FindFirstOf(str, 0, delims);
    if (InvalidIndex == leftSplitIndex) {
        // no delimiters in source string
        left = str;
        right.Clear();
    }
    else {
        if (leftSplitIndex > 0) {
            left.Assign(str.AsCStr(), leftSplitIndex);
        }
        else {
            left.Clear();
        }
        // skip delimiters
        int32 rightSplitIndex = StringUtil::FindFirstNotOf(str, leftSplitIndex, delims);
        if (InvalidIndex != rightSplitIndex) {
            right.Assign(str.AsCStr() + leftSplitIndex + rightSplitIndex, 0);
        }
        else {
            right.Clear();
        }
    }
}

//------------------------------------------------------------------------------
String
StringUtil::Concatenate(char delim, std::initializer_list<String> list) {
    // count overall needed length
    int32 len = 0;
    for (const String& str : list) {
        len += str.Length() + 1;
    }
    len--;
    
    // reserve a string
    String concat;
    if (len > 0) {
        char* ptr = concat.Alloc(len);
        const char* end = ptr + len;
        for (const String& str : list) {
            const int32 strLen = str.Length();
            Memory::Copy(str.AsCStr(), ptr, strLen);
            ptr += strLen;
            if (ptr < end) {
                *ptr++ = delim;
            }
        }
    }
    return concat;
}

//------------------------------------------------------------------------------
String
StringUtil::Append(std::initializer_list<String> list) {
    // count overall needed length
    int32 len = 0;
    for (const String& str : list) {
        len += str.Length();
    }
    
    // reserve a string
    String merge;
    if (len > 0) {
        char* ptr = merge.Alloc(len);
        for (const String& str : list) {
            const int32 strLen = str.Length();
            Memory::Copy(str.AsCStr(), ptr, strLen);
            ptr += strLen;
        }
    }
    return merge;
}

//------------------------------------------------------------------------------
String
StringUtil::Truncate(const String& str, int32 newLen) {
    if (newLen < str.Length()) {
        return String(str, 0, newLen);
    }
    else {
        return String(str);
    }
}

} // namespace Core
} // namespace Oryol