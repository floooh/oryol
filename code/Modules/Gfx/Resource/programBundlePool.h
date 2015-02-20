#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::programBundlePool
    @ingroup _priv
    @brief resource pool specialization for program bundles
*/
#include "Resource/Core/ResourcePool.h"
#include "Gfx/Setup/ProgramBundleSetup.h"
#include "Gfx/Resource/programBundleFactory.h"
#include "Gfx/Resource/programBundle.h"

namespace Oryol {
namespace _priv {
class programBundlePool : public ResourcePool<programBundle, ProgramBundleSetup, programBundleFactory> { };
} // namespace _priv
} // namespace Oryol

