//------------------------------------------------------------------------------
//  IndexType.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "IndexType.h"
#include "Core/Macros.h"
#include <cstring>

namespace Oryol {
namespace Render {
    
//------------------------------------------------------------------------------
const char*
IndexType::ToString(Code c) {
    switch (c) {
        __ORYOL_TOSTRING(None);
        __ORYOL_TOSTRING(Index16);
        __ORYOL_TOSTRING(Index32);
        default: return "InvalidIndexType";
    }
}

//------------------------------------------------------------------------------
IndexType::Code
IndexType::FromString(const char* str) {
    o_assert(str);
    __ORYOL_FROMSTRING(None);
    __ORYOL_FROMSTRING(Index16);
    __ORYOL_FROMSTRING(Index32);
    return IndexType::InvalidIndexType;
}

//------------------------------------------------------------------------------
int32
IndexType::ByteSize(IndexType::Code c) {
    switch (c) {
        case None:      return 0;
        case Index16:   return 2;
        case Index32:   return 4;
        default:
            o_error("IndexType::ByteSize() called with invalid type!\n");
            return 0;
    }
}


} // namespace Render
} // namespace Oryol
