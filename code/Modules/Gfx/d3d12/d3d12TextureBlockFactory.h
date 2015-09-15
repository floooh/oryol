#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d12TextureBlockFactory
    @ingroup _priv
    @brief D3D12 implementation of textureBlockFactory
*/
#include "Resource/ResourceState.h"
#include "Gfx/Core/gfxPointers.h"

namespace Oryol {
namespace _priv {

class textureBlock;

class d3d12TextureBlockFactory {
public:
    /// constructor
    d3d12TextureBlockFactory();
    /// destructor
    ~d3d12TextureBlockFactory();

    /// setup the factory
    void Setup(const gfxPointers& ptrs);
    /// discard the factory
    void Discard();
    /// return true if factory has been setup
    bool IsValid() const;

    /// setup resource
    ResourceState::Code SetupResource(textureBlock& tb);
    /// destroy resource
    void DestroyResource(textureBlock& tb);

private:
    bool isValid;
    gfxPointers pointers;
};

} // namespace _priv
} // namespace Oryol
