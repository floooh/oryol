#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::glMeshFactory
    @ingroup _priv
    @brief GL implementation of MeshFactory
*/
#include "Gfx/Resource/mesh.h"
#include "IO/Stream/Stream.h"

namespace Oryol {
namespace _priv {

class renderer;
class meshPool;
class mesh;

class glMeshFactory {
public:
    /// constructor
    glMeshFactory();
    /// destructor
    ~glMeshFactory();
    
    /// setup with a pointer to the state wrapper object
    void Setup(renderer* rendr, meshPool* mshPool);
    /// discard the factory
    void Discard();
    /// return true if the object has been setup
    bool IsValid() const;

    /// setup resource
    bool SetupResource(mesh& mesh);
    /// setup with 'raw' data
    bool SetupResource(mesh& mesh, const Ptr<Stream>& data);
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
    bool createFullscreenQuad(mesh& mesh);
    /// helper method to create empty mesh
    bool createEmptyMesh(mesh& mesh);
    /// create from stream data
    bool createFromStream(mesh& mesh, const Ptr<Stream>& data);
    
private:
    /// setup a Mesh's GL vertex attributes
    void glSetupVertexAttrs(mesh& mesh);

    class renderer* renderer;
    class meshPool* meshPool;
    bool isValid;
};
    
} // namespace _priv
} // namespace Oryol

 