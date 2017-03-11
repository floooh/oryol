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
    /// destructor
    ~d3d11MeshFactory();
    
    /// setup with a pointer to the state wrapper object
    void Setup(const gfxPointers& ptrs);
    /// discard the factory
    void Discard();

    /// setup with 'raw' data
    ResourceState::Code SetupResource(mesh& mesh, const void* data, int size);
    /// discard the resource
    void DestroyResource(mesh& mesh);

    /// helper method to setup a mesh object as fullscreen quad
    ResourceState::Code createFullscreenQuad(mesh& mesh);
    /// create from data
    ResourceState::Code createBuffers(mesh& mesh, const void* data, int size);

private:
    /// helper method to create vertex or index buffer
    ID3D11Buffer* createBuffer(const void* vertexData, uint32_t vertexDataSize, uint32_t d3d11BindFlags, Usage::Code usage);
    /// helper method to populate vertex and index buffer attributes in mesh object
    void setupAttrs(mesh& mesh);
    /// helper method to populate primitive groups array in mesh object
    void setupPrimGroups(mesh& mesh);

    gfxPointers pointers;
    ID3D11Device* d3d11Device = nullptr;
    bool isValid = false;
};

} // namespace _priv
} // namespace Oryol

    