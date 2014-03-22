#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::glMeshFactory
    @brief GL implementation of MeshFactory
*/
#include "Resource/loaderFactory.h"
#include "Render/Core/mesh.h"

namespace Oryol {
namespace Render {

class stateWrapper;
class mesh;
class meshLoaderBase;

class glMeshFactory : public Resource::loaderFactory<mesh, meshLoaderBase> {
public:
    /// constructor
    glMeshFactory();
    /// destructor
    ~glMeshFactory();
    
    /// setup with a pointer to the state wrapper object
    void Setup(stateWrapper* stWrapper);
    /// discard the factory
    void Discard();
    /// return true if the object has been setup
    bool IsValid() const;
    /// discard the resource
    void DestroyResource(mesh& mesh);
    
    /// helper method to create vertex buffer in mesh
    void createVertexBuffer(const void* vertexData, uint32 vertexDataSize, mesh& outMesh);
    /// helper method to create index buffer in mesh
    void createIndexBuffer(const void* indexData, uint32 indexDataSize, mesh& outMesh);
    /// helper method to create platform-specific vertex layout
    void createVertexLayout(mesh& outMesh);
    
private:
    /// setup a Mesh's GL vertex attributes
    void glSetupVertexAttrs(mesh& mesh);

    stateWrapper* glStateWrapper;
    bool isValid;
};
    
} // namespace Render
} // namespace Oryol
 