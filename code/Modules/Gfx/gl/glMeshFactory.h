#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::glMeshFactory
    @ingroup _priv
    @brief GL implementation of meshFactory
*/
#include "Resource/ResourceState.h"
#include "Gfx/gl/gl_decl.h"
#include "Gfx/Core/Enums.h"
#include "Gfx/Core/gfxPointers.h"

namespace Oryol {
namespace _priv {

class mesh;

class glMeshFactory {
public:
    /// destructor
    ~glMeshFactory();
    
    /// setup the factory
    void Setup(const gfxPointers& ptrs);
    /// discard the factory
    void Discard();
    /// return true if the object has been setup
    bool IsValid() const;

    /// setup resource
    ResourceState::Code SetupResource(mesh& mesh);
    /// setup with 'raw' data
    ResourceState::Code SetupResource(mesh& mesh, const void* data, int size);
    /// discard the resource
    void DestroyResource(mesh& mesh);
    
    /// helper method to setup a mesh object as fullscreen quad
    ResourceState::Code createFullscreenQuad(mesh& mesh);
    /// helper method to create empty mesh
    ResourceState::Code createEmptyMesh(mesh& mesh);
    /// create from data
    ResourceState::Code createFromData(mesh& mesh, const void* data, int size);
    
private:
    /// helper method to populate vertex/index attr structs
    void setupAttrs(mesh& msh);
    /// helper method to populate primitive groups
    void setupPrimGroups(mesh& msh);
    /// helper method to create vertex buffer in mesh
    GLuint createVertexBuffer(const void* vertexData, uint32_t vertexDataSize, Usage::Code usage);
    /// helper method to create index buffer in mesh
    GLuint createIndexBuffer(const void* indexData, uint32_t indexDataSize, Usage::Code usage);

    gfxPointers pointers;
    bool isValid = false;
};
    
} // namespace _priv
} // namespace Oryol
