#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d12ShaderFactory
    @ingroup _priv
    @brief D3D12 implementation of shaderFactory
*/
#include "Resource/ResourceState.h"
#include "Gfx/Core/gfxPointers.h"

namespace Oryol {
namespace _priv {

class shader;

class d3d12ShaderFactory {
public:
    /// constructor
    d3d12ShaderFactory();
    /// destructor
    ~d3d12ShaderFactory();

    /// setup with a pointer to the state wrapper object
    void Setup(const gfxPointers& ptrs);
    /// discard the factory
    void Discard();
    /// return true if the object has been setup
    bool IsValid() const;

    /// setup resource
    ResourceState::Code SetupResource(shader& shd);
    /// destroy the resource
    void DestroyResource(shader& shd);

private:
    gfxPointers pointers;
    bool isValid;
};

} // namespace _priv
} // namespace Oryol



