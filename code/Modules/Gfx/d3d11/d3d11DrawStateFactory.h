#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11DrawStateFactory
    @ingroup _priv
    @brief D3D11 implementation of drawStateFactory

    FIXME: there should be a way to reuse input layout objects by shader
    signature, this would drastically reduce the number of 
    shader layout objects
*/
#include "Gfx/Resource/drawStateFactoryBase.h"
#include "Gfx/d3d11/d3d11_decl.h"
#include "Core/Containers/Map.h"
#include "Gfx/Core/RasterizerState.h"
#include "Gfx/Core/DepthStencilState.h"
#include "Gfx/Core/BlendState.h"

namespace Oryol {
namespace _priv {

class d3d11DrawStateFactory : public drawStateFactoryBase {
public:
    /// constructor
    d3d11DrawStateFactory();
    /// destructor
    ~d3d11DrawStateFactory();

    /// setup the factory
    void Setup(class renderer* rendr, class meshPool* mshPool, class programBundlePool* pbPool);
    /// discard the factory
    void Discard();
    /// setup shader resource
    ResourceState::Code SetupResource(drawState& ds);
    /// destroy the shader
    void DestroyResource(drawState& ds);

private:
    ID3D11Device* d3d11Device;
};

} // namespace _priv
} // namespace Oryol

