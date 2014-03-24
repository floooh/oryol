#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::IO::Module
    @brief IO system Module class
*/
#include "IO/IOFacade.h"

namespace Oryol {
namespace IO {
    
class Module {
public:
    /// setup the IO system
    static void Setup();
    /// discard the IO system
    static void Discard();
};

} // namespace IO
} // namespace Oryol