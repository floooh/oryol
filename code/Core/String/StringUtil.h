#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::CoreStringUtil
    
    Various string utility functions.
*/
#include "Core/Types.h"
#include "Core/Containers/Array.h"
#include "Core/String/String.h"
#include "Core/String/WideString.h"

namespace Oryol {
namespace Core {
    
class StringUtil {
public:
    /// convert a simple type to string
    template<class TYPE> static String ToString(const TYPE& val);
    /// convert a string to simple type
    template<class TYPE> static TYPE FromString(const String& str);
    /// convert an UTF-8 String to WideString
    static WideString UTF8ToWideString(const String& str);
    /// convert WideString to String
    static String WideToUTF8String(const WideString& str);
};
    
} // namespace Core
} // namespace Oryol