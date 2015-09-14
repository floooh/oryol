#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlTextureBlockFactory
    @ingroup _priv
    @brief Metal implementation of textureBlockFactory
*/
#include "Resource/ResourceState.h"
#include "Gfx/Core/gfxPointers.h"

namespace Oryol {
namespace _priv {

class textureBlock;

class mtlTextureBlockFactory {
public:
    /// constructor
    mtlTextureBlockFactory();
    /// destructor
    ~mtlTextureBlockFactory();

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
