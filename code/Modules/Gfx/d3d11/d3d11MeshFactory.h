#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11MeshFactory
    @ingroup _priv
    @brief D3D11 implementation of MeshFactory
*/
#include "Resource/ResourceState.h"
#include "Gfx/Resource/mesh.h"

namespace Oryol {
namespace _priv {

class renderer;
class meshPool;
class mesh;

class d3d11MeshFactory {
public:
    /// constructor
    d3d11MeshFactory();
    /// destructor
    ~d3d11MeshFactory();
    
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
};

} // namespace _priv
} // namespace Oryol

    