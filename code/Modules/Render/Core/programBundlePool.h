#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::programBundlePool
    @ingroup _priv
    @brief resource pool specialization for program bundles
*/
#include "Resource/ResourcePool.h"
#include "Render/Setup/ProgramBundleSetup.h"
#include "Render/Core/programBundleFactory.h"
#include "Render/Core/programBundle.h"

namespace Oryol {
namespace _priv {
class programBundlePool : public ResourcePool<programBundle, ProgramBundleSetup, programBundleFactory> { };
} // namespace _priv
} // namespace Oryol

