#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::glMeshFactory
    @brief GL implementation of MeshFactory
*/
#include "Render/base/loaderFactory.h"
#include "Render/Core/mesh.h"

namespace Oryol {
namespace Render {

class stateWrapper;
class meshPool;
class mesh;
class meshLoaderBase;

class glMeshFactory : public loaderFactory<mesh, meshLoaderBase, ResourceType::Mesh> {
public:
    /// constructor
    glMeshFactory();
    /// destructor
    ~glMeshFactory();
    
    /// setup with a pointer to the state wrapper object
    void Setup(stateWrapper* stWrapper, meshPool* mshPool);
    /// discard the factory
    void Discard();
    /// return true if the object has been setup
    bool IsValid() const;

    /// setup resource, continue calling until res state is not Pending
    void SetupResource(mesh& mesh);
    /// setup with input data, continue calling until res state is not Pending
    void SetupResource(mesh& mesh, const Core::Ptr<IO::Stream>& data);
    /// discard the resource
    void DestroyResource(mesh& mesh);
    
    /// helper method to create vertex buffer in mesh
    GLuint createVertexBuffer(const void* vertexData, uint32 vertexDataSize, Usage::Code usage);
    /// helper method to create index buffer in mesh
    GLuint createIndexBuffer(const void* indexData, uint32 indexDataSize, Usage::Code usage);
    /// lookup and attach instance buffer to mesh
    void attachInstanceBuffer(mesh& mesh);
    /// helper method to create platform-specific vertex layout
    void setupVertexLayout(mesh& mesh);
    /// helper method to setup a mesh object as fullscreen quad
    void createFullscreenQuad(mesh& mesh);
    /// helper method to create empty mesh
    void createEmptyMesh(mesh& mesh);
    
private:
    /// setup a Mesh's GL vertex attributes
    void glSetupVertexAttrs(mesh& mesh);

    class stateWrapper* stateWrapper;
    class meshPool* meshPool;
    bool isValid;
};
    
} // namespace Render
} // namespace Oryol
 