#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::programBundlePool
    @ingroup _priv
    @brief resource pool specialization for program bundles
*/
#include "Resource/Pool.h"
#include "Render/Setup/ProgramBundleSetup.h"
#include "Render/Core/programBundleFactory.h"
#include "Render/Core/programBundle.h"

namespace Oryol {
namespace _priv {
class programBundlePool : public Resource::Pool<programBundle, ProgramBundleSetup, programBundleFactory> { };
} // namespace _priv
} // namespace Oryol

