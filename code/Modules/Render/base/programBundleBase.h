#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::programBundleBase
    @brief private: program bundle resource base class
*/
#include "Resource/resourceBase.h"
#include "Render/Setup/ProgramBundleSetup.h"

namespace Oryol {
namespace Render {
    
class programBundleBase : public Resource::resourceBase<ProgramBundleSetup> { };
    
} // namespace Render
} // namespace Oryol
