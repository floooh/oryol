#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::vlkMeshFactory
    @ingroup _priv
    @brief Vulkan implementation of meshFactory
*/
#include "Resource/ResourceState.h"
#include "Gfx/Core/gfxPointers.h"

namespace Oryol {
namespace _priv {

class mesh;

class vlkMeshFactory {
public:
    /// destructor
    ~vlkMeshFactory();

    /// setup with a pointer to the state wrapper object
    void Setup(const gfxPointers& ptrs);
    /// discard the factory
    void Discard();
    /// return true if the object has been setup
    bool IsValid() const;

    /// setup resource
    ResourceState::Code SetupResource(mesh& mesh);
    /// setup with 'raw' data
    ResourceState::Code SetupResource(mesh& mesh, const void* data, int32 size);
    /// discard the resource
    void DestroyResource(mesh& mesh);

    gfxPointers pointers;
    bool isValid = false;
};

} // namespace _priv
} // namespace Oryol
