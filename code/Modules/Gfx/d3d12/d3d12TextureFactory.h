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

namespace Oryol {
namespace _priv {

class texture;

class d3d12TextureFactory {
public:
    /// destructor
    ~d3d12TextureFactory();

    /// setup with a pointer to the state wrapper object
    void Setup(const gfxPointers& ptrs);
    /// discard the factory
    void Discard();

    /// setup with input data
    ResourceState::Code SetupResource(texture& tex, const void* data, int size);
    /// discard the resource
    void DestroyResource(texture& tex);

private:
    /// setup texture attributes object in texture
    void setupTextureAttrs(texture& tex);
    /// create a texture with or without associated data
    ResourceState::Code createTexture(texture& tex, const void* data, int32_t size);

    gfxPointers pointers;
    bool isValid = false;
};

} // _priv
} // Oryol
