#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::programBundleBase
    @ingroup _priv
    @brief private: program bundle resource base class
*/
#include "Resource/Core/resourceBase.h"
#include "Gfx/Setup/ProgramBundleSetup.h"

namespace Oryol {
namespace _priv {
class programBundleBase : public resourceBase<ProgramBundleSetup> { };
} // namespace _priv
} // namespace Oryol
