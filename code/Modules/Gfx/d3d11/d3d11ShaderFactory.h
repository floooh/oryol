#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11ShaderFactory
    @ingroup _priv
    @brief D3D11 implementation of shader factory
*/
#include "Resource/ResourceState.h"
#include "Gfx/Resource/mesh.h"
#include "Gfx/Core/Enums.h"

namespace Oryol {
namespace _priv {

class shader;

class d3d11ShaderFactory {
public:
    /// constructor
    d3d11ShaderFactory();
    /// destructor
    ~d3d11ShaderFactory();

    /// setup the factory
    void Setup();
    /// discard the factory
    void Discard();
    /// return true if the object has been setup
    bool IsValid() const;

    /// setup shader resource (directly setup shader, no loaders)
    ResourceState::Code SetupResource(shader& shd);
    /// destroy the shader
    void DestroyResource(shader& shd);
};

} // namespace _priv
} // namespace Oryol