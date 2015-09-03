#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::glTextureBundleFactory
    @ingroup _priv
    @brief GL implementation of textureBundleFactory
*/
#include "Resource/ResourceState.h"
#include "Gfx/Core/gfxPointers.h"

namespace Oryol {
namespace _priv {

class textureBundle;

class glTextureBundleFactory {
public:
    /// constructor
    glTextureBundleFactory();
    /// destructor
    ~glTextureBundleFactory();

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