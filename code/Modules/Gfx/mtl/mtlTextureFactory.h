#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlTextureFactory
    @ingroup _priv
    @brief Metal implementation of textureFactory
*/
#include "Resource/ResourceState.h"
#include "Gfx/Core/gfxPointers.h"
#include "Core/Containers/Map.h"
#include "Gfx/mtl/mtl_decl.h"

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

    /// setup with input data
    ResourceState::Code SetupResource(texture& tex, const void* data, int size);
    /// discard the resource
    void DestroyResource(texture& tex);
    
private:
    /// create texture with or without data
    ResourceState::Code createTexture(texture& tex, const void* data, int size);
    /// create a sampler state object and set in texture object
    void createSamplerState(texture& tex);
    /// release sampler state object of texture
    void releaseSamplerState(texture& tex);

    gfxPointers pointers;
    bool isValid;

    // cache to re-use existing sampler state objects
    struct SamplerCacheItem {
        ORYOL_OBJC_TYPED_ID(MTLSamplerState) mtlSamplerState;
        int useCount;
    };
    Map<uint32_t, SamplerCacheItem> samplerCache;
};
    
} // namespace _priv
} // namespace Oryol
