#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11RenderPassFactory
    @ingroup _priv
    @brief D3D11 implementation of renderPassFactory
*/
#include "Gfx/Resource/renderPassFactoryBase.h"
#include "Gfx/d3d11/d3d11_decl.h"

namespace Oryol {
namespace _priv {
class d3d11RenderPassFactory : public renderPassFactoryBase {
public:
    /// destructor
    ~d3d11RenderPassFactory();
    /// setup the factory
    void Setup(const gfxPointers& ptrs);
    /// discard the factory
    void Discard();
    /// setup renderpass resource
    ResourceState::Code SetupResource(renderPass& rp);
    /// destroy renderpass resource
    void DestroyResource(renderPass& rp);
private:
    ID3D11Device* d3d11Device = nullptr;
};
} // namespace _priv
} // namespace Oryol