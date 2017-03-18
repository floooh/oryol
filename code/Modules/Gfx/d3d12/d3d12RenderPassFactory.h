#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d12RenderPassFactory
    @ingroup _priv
    @brief D3D12 impl of renderPassFactory
*/
#include "Gfx/Resource/renderPassFactoryBase.h"
#include "Gfx/d3d12/d3d12_decl.h"

namespace Oryol {
namespace _priv {
class d3d12RenderPassFactory : public renderPassFactoryBase {
public:
    /// setup renderpass resource
    ResourceState::Code SetupResource(renderPass& rp);
    /// destroy renderpass resource
    void DestroyResource(renderPass& rp);
};

} // namespace _priv
} // namespace Oryol
