#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlMeshFactory
    @ingroup _priv
    @brief Metal implementation of meshFactory
*/
#include "Resource/ResourceState.h"
#include "Gfx/Core/Types.h"
#include "Gfx/Core/gfxPointers.h"
#include "Gfx/mtl/mtl_decl.h"

namespace Oryol {
namespace _priv {

class mesh;

class mtlMeshFactory {
public:
    /// constructor
    mtlMeshFactory();
    /// destructor
    ~mtlMeshFactory();
    
    /// setup with a pointer to the state wrapper object
    void Setup(const gfxPointers& ptrs);
    /// discard the factory
    void Discard();
    /// return true if the object has been setup
    bool IsValid() const;

    /// setup with 'raw' data
    ResourceState::Code SetupResource(mesh& mesh, const void* data, int size);
    /// discard the resource
    void DestroyResource(mesh& mesh);

    /// helper method to setup mesh as fullscreen quad
    ResourceState::Code createFullscreenQuad(mesh& mesh);
    /// create mesh
    ResourceState::Code createMesh(mesh& mesh, const void* data, int size);

private:
    /// setup vertex/index buffer attrs structs in the mesh
    void setupAttrs(mesh& msh);
    /// setup the primitive groups in the mesh
    void setupPrimGroups(mesh& msh);
    /// helper method to create vertex or index buffer
    ORYOL_OBJC_TYPED_ID(MTLBuffer) createBuffer(const void* data, uint32_t dataSize, Usage::Code usage);

    gfxPointers pointers;
    bool isValid;
};

} // namespace _priv
} // namespace Oryol
