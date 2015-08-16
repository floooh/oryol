#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d12TextureFactory
    @ingroup _priv
    @brief D3D12 implementation of texture factory
*/
#include "Resource/ResourceState.h"
#include "Gfx/Core/gfxPointers.h"

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
    gfxPointers pointers;
    bool isValid;
};

} // _priv
} // Oryol
