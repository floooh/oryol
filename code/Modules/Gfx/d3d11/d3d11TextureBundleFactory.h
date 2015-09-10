#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11TextureBundleFactory
    @ingroup _priv
    @brief D3D11 implementation of textureBundleFactory
*/
#include "Resource/ResourceState.h"
#include "Gfx/Core/gfxPointers.h"

namespace Oryol {
namespace _priv {

class textureBundle;

class d3d11TextureBundleFactory {
public:
    /// constructor
    d3d11TextureBundleFactory();
    /// destructor
    ~d3d11TextureBundleFactory();

    /// setup the factory
    void Setup(const gfxPointers& ptrs);
    /// discard the factory
    void Discard();
    /// return true if factory has been setup
    bool IsValid() const;

    /// setup resource
    ResourceState::Code SetupResource(textureBundle& tb);
    /// destroy resource
    void DestroyResource(textureBundle& tb);

private:
    bool isValid;
    gfxPointers pointers;
};

} // namespace _priv
} // namespace Oryol
