#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::Module
    @brief Render system Module class
*/
#include "Render/RenderFacade.h"

namespace Oryol {
namespace Render {
    
class Module {
public:
    /// setup the IO system
    static void Setup();
    /// discard the IO system
    static void Discard();
};
    
} // namespace Render
} // namespace Oryol