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
/**
 Find index of first occurance of any chacters in delim, between startIndex
 (including) and endIndex (excluding). If endIndex is 0, search until end of string.
 Returns InvalidIndex if not found.
*/
int32
StringUtil::FindFirstOf(const String& str, int32 startIndex, int32 endIndex, const char* delims) {
    o_assert(0 != delims);
    o_assert((endIndex == 0) || (endIndex >= startIndex));
    if (startIndex < str.Length()) {
        const char* ptr = str.AsCStr() + startIndex;
        int index = std::strcspn(ptr, delims);
        if (((endIndex != 0) && (index >= endIndex)) || (index >= str.Length())) {
            return InvalidIndex;
        }
        else {
            return startIndex + index;
        }
    }
    else {
        return InvalidIndex;
    }
}

//------------------------------------------------------------------------------
/**
 Find index of first occurange of any characters NOT in delim, between startIndex
 (including) and endIndex (exluding). If endIndex is 0, search until end of string.
 Returns InvalidIndex if not found.
*/
int32
StringUtil::FindFirstNotOf(const String& str, int32 startIndex, int32 endIndex, const char* delims) {
    o_assert(0 != delims);
    o_assert((endIndex == 0) || (endIndex >= startIndex));
    if (startIndex < str.Length()) {
        const char* ptr = str.AsCStr() + startIndex;
        int index = std::strspn(ptr, delims);
        if (((endIndex != 0) && (index >= endIndex)) || (index >= str.Length())) {
            return InvalidIndex;
        }
        else {
            return startIndex + index;
        }
    }
    else {
        return InvalidIndex;
    }
}

//------------------------------------------------------------------------------
/**
 Find first occurance of subStr between startIndex (including) and endIndex
 (excluding). If endIndex is 0, search until end of string. 
 Returns InvalidIndex if not found.
*/
int32
StringUtil::FindSubString(const String& str, int32 startIndex, int32 endIndex, const char* subStr) {
    o_assert(0 != subStr);
    o_assert((endIndex == 0) || (endIndex >= startIndex));
    if (startIndex < str.Length()) {
        const char* ptr = str.AsCStr() + startIndex;
        const char* occur = std::strstr(ptr, subStr);
        if (nullptr == occur) {
            return InvalidIndex;
        }
        else {
            int32 index = occur - ptr;
            if ((endIndex != 0) && (index >= endIndex)) {
                return InvalidIndex;
            }
            else {
                return startIndex + index;
            }
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
    
    int32 leftSplitIndex = StringUtil::FindFirstOf(str, 0, 0, delims);
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
        int32 rightSplitIndex = StringUtil::FindFirstNotOf(str, leftSplitIndex, 0, delims);
        if (InvalidIndex != rightSplitIndex) {
            right.Assign(str.AsCStr() + rightSplitIndex, 0);
        }
        else {
            right.Clear();
        }
    }
}

} // namespace Core
} // namespace Oryol