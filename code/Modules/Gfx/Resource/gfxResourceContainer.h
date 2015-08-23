#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::gfxResourceContainer
    @ingroup _priv
    @brief resource container implementation of Gfx module
*/
#if ORYOL_D3D12
#include "Gfx/d3d12/d3d12ResourceContainer.h"
namespace Oryol {
namespace _priv {
class gfxResourceContainer : public d3d12ResourceContainer { };
} }
#else
#include "Gfx/Resource/gfxResourceContainerBase.h"
namespace Oryol {
namespace _priv {
class gfxResourceContainer : public gfxResourceContainerBase { };
} }
#endif
