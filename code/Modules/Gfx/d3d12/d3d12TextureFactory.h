#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d12TextureFactory
    @ingroup _priv
    @brief D3D12 implementation of texture factory
*/
#include "Resource/ResourceState.h"
#include "Resource/Id.h"
#include "Gfx/Core/gfxPointers.h"
#include "Gfx/d3d12/d3d12_decl.h"
#include "Core/Containers/Map.h"

namespace Oryol {
namespace _priv {

class texture;

class d3d12TextureFactory {
public:
    /// constructor
    d3d12TextureFactory();
    /// destructor
    ~d3d12TextureFactory();

    /// setup with a pointer to the state wrapper object
    void Setup(const gfxPointers& ptrs);
    /// discard the factory
    void Discard();
    /// return true if the object has been setup
    bool IsValid() const;

    /// setup resource
    ResourceState::Code SetupResource(texture& tex);
    /// setup with input data
    ResourceState::Code SetupResource(texture& tex, const void* data, int32 size);
    /// discard the resource
    void DestroyResource(texture& tex);

private:
    /// create render target texture
    ResourceState::Code createRenderTarget(texture& tex);
    /// create a texture from pixel data in memory
    ResourceState::Code createFromPixelData(texture& tex, const void* data, int32 size);
    /// create empty texture (cannot be immutable)
    ResourceState::Code createEmptyTexture(texture& tex);
    /// setup texture attributes object in texture
    void setupTextureAttrs(texture& tex);

    gfxPointers pointers;
    bool isValid;
};

} // _priv
} // Oryol
