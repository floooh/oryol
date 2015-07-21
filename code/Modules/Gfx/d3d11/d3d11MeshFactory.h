#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::d3d11MeshFactory
    @ingroup _priv
    @brief D3D11 implementation of MeshFactory
*/
#include "Resource/ResourceState.h"
#include "Gfx/Core/gfxPointers.h"
#include "Gfx/Core/Enums.h"
#include "Gfx/d3d11/d3d11_decl.h"

namespace Oryol {
namespace _priv {

class mesh;

class d3d11MeshFactory {
public:
    /// constructor
    d3d11MeshFactory();
    /// destructor
    ~d3d11MeshFactory();
    
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

    /// helper method to setup a mesh object as fullscreen quad
    ResourceState::Code createFullscreenQuad(mesh& mesh);
    /// helper method to create empty mesh
    ResourceState::Code createEmptyMesh(mesh& mesh);
    /// create from data
    ResourceState::Code createFromData(mesh& mesh, const void* data, int32 size);

private:
    /// helper method to create vertex or index buffer
    ID3D11Buffer* createBuffer(const void* vertexData, uint32 vertexDataSize, uint32 d3d11BindFlags, Usage::Code usage);

    gfxPointers pointers;
    ID3D11Device* d3d11Device;
    bool isValid;
};

} // namespace _priv
} // namespace Oryol

    