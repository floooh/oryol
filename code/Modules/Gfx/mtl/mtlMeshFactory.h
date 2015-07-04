#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::mtlMeshFactory
    @ingroup _priv
    @brief Metal implementation of meshFactory
*/
#include "Resource/ResourceState.h"
#include "Gfx/Core/Enums.h"
#include "Gfx/mtl/mtl_decl.h"

namespace Oryol {
namespace _priv {

class renderer;
class meshPool;
class mesh;

class mtlMeshFactory {
public:
    /// constructor
    mtlMeshFactory();
    /// destructor
    ~mtlMeshFactory();
    
    /// setup with a pointer to the state wrapper object
    void Setup(renderer* rendr, meshPool* mshPool);
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

    /// helper method to setup mesh as fullscreen quad
    ResourceState::Code createFullscreenQuad(mesh& mesh);
    /// helper method to create empty mesh
    ResourceState::Code createEmptyMesh(mesh& mesh);
    /// create mesh from data
    ResourceState::Code createFromData(mesh& mesh, const void* data, int32 size);

private:
    /// helper method to create vertex or index buffer
    ORYOL_OBJC_TYPED_ID(MTLBuffer) createBuffer(const void* data, uint32 dataSize, Usage::Code usage);

    class renderer* renderer;
    class meshPool* meshPool;
    bool isValid;
};

} // namespace _priv
} // namespace Oryol
