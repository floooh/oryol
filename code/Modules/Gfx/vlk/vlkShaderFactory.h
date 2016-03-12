#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::vlkShaderFactory
    @ingroup _priv
    @brief Vulkan implementation of shaderFactory
*/
#include "Resource/ResourceState.h"
#include "Gfx/Core/gfxPointers.h"

namespace Oryol {
namespace _priv {

class shader;

class vlkShaderFactory { 
public:
    /// destructor
    ~vlkShaderFactory();

    /// setup the factory
    void Setup(const gfxPointers& ptrs);
    /// discard the factory
    void Discard();
    /// return true if the object has been setup
    bool IsValid() const;
    
    /// setup resource
    ResourceState::Code SetupResource(shader& shd);
    /// destroy the resource
    void DestroyResource(shader& shd);

    gfxPointers pointers;
    bool isValid = false;
};

} // namespace _priv
} // namespace Oryol