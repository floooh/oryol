#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::programBundlePool
    @brief resource pool specialization for program bundles
*/
#include "Resource/Pool.h"
#include "Render/Setup/ProgramBundleSetup.h"
#include "Render/Core/programBundleFactory.h"
#include "Render/Core/programBundle.h"

namespace Oryol {
namespace Render {
    
class programBundlePool : public Resource::Pool<programBundle, ProgramBundleSetup, programBundleFactory> { };
    
} // namespace Render
} // namespace Oryol

