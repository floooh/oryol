#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlTextureFactory
    @ingroup _priv
    @brief Metal implementation of textureFactory
*/
#include "Resource/ResourceState.h"
#include "Gfx/Core/gfxPointers.h"

namespace Oryol {
namespace _priv {

class texture;

class mtlTextureFactory {
public:
    /// constructor
    mtlTextureFactory();
    /// destructor
    ~mtlTextureFactory();
    
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
    /// create a render-target texture
    ResourceState::Code createRenderTarget(texture& tex);
    /// create a texture from pixel-data in memory
    ResourceState::Code createFromPixelData(texture& tex, const void* data, int32 size);
    /// create a sampler state object and set in texture object
    void createSamplerState(texture& tex);

    gfxPointers pointers;
    bool isValid;
};
    
} // namespace _priv
} // namespace Oryol
