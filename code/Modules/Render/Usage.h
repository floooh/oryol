#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::Usage
    @brief graphics resource usage types
*/
#include "Core/Types.h"

namespace Oryol {
namespace Render {
    
class Usage {
public:
    /// usage enum
    enum Code {
        Immutable = 0,  ///> resource is immutable, can only be initialized
        DynamicWrite,   ///> dynamic resource, infrequently written by CPU
        DynamicStream,  ///> dynamic resource, frequently written by CPU
        
        NumUsages,
        InvalidUsage
    };
    
    /// convert to string
    static const char* ToString(Code c);
    /// convert from string
    static Code FromString(const char* str);
};
    
} // namespace Render
} // namespace Oryol
 