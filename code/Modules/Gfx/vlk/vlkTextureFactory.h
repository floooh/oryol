#pragma once
//-------------------------------------------------------------------------------
/**
    @class Oryol::_priv::vlkTextureFactory
    @ingroup _priv
    @brief Vulkan implementation of textureFactory
*/
#include "Resource/ResourceState.h"
#include "Gfx/Core/gfxPointers.h"

namespace Oryol {
namespace _priv {

class texture;

class vlkTextureFactory {
public:
    /// destructor
    ~vlkTextureFactory();

    /// setup the factory
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
    gfxPointers pointers;
    bool isValid = false;
};

} // namespace _priv
} // namespace Oryol
