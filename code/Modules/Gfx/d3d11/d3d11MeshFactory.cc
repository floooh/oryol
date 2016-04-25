//------------------------------------------------------------------------------
//  d3d11MeshFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d11MeshFactory.h"
#include "d3d11_impl.h"
#include "Gfx/Core/renderer.h"
#include "Gfx/Resource/resourcePools.h"
#include "Gfx/d3d11/d3d11Types.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d11MeshFactory::~d3d11MeshFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
d3d11MeshFactory::Setup(const gfxPointers& ptrs) {
    o_assert_dbg(!this->isValid);
    this->isValid = true;
    this->pointers = ptrs;
    this->d3d11Device = this->pointers.renderer->d3d11Device;
}

//------------------------------------------------------------------------------
void
d3d11MeshFactory::Discard() {
    o_assert_dbg(this->isValid);
    this->pointers = gfxPointers();
    this->d3d11Device = nullptr;
    this->isValid = false;
}

//------------------------------------------------------------------------------
bool
d3d11MeshFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d11MeshFactory::SetupResource(mesh& msh) {
    o_assert_dbg(this->isValid);
    if (msh.Setup.ShouldSetupEmpty()) {
        return this->createBuffers(msh, nullptr, 0);
    }
    else if (msh.Setup.ShouldSetupFullScreenQuad()) {
        return this->createFullscreenQuad(msh);
    }
    else {
        o_error("d3d11MeshFactory::SetupResource(): don't know how to create mesh!");
        return ResourceState::InvalidState;
    }
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d11MeshFactory::SetupResource(mesh& msh, const void* data, int size) {
    o_assert_dbg(msh.Setup.ShouldSetupFromData());
    return this->createBuffers(msh, data, size);
}

//------------------------------------------------------------------------------
void
d3d11MeshFactory::DestroyResource(mesh& mesh) {
    this->pointers.renderer->invalidateMeshState();
    if (mesh.d3d11VertexBuffer) {
        mesh.d3d11VertexBuffer->Release();
    }
    if (mesh.d3d11IndexBuffer) {
        mesh.d3d11IndexBuffer->Release();
    }
    mesh.Clear();
}

//------------------------------------------------------------------------------
ID3D11Buffer*
d3d11MeshFactory::createBuffer(const void* data, uint32_t dataSize, uint32_t d3d11BindFlags, Usage::Code usage) {
    o_assert_dbg(this->d3d11Device);
    o_assert_dbg((D3D11_BIND_VERTEX_BUFFER == d3d11BindFlags) || (D3D11_BIND_INDEX_BUFFER == d3d11BindFlags));

    D3D11_BUFFER_DESC desc;
    Memory::Clear(&desc, sizeof(desc));
    desc.ByteWidth = dataSize;
    desc.Usage = d3d11Types::asResourceUsage(usage);
    desc.BindFlags = d3d11BindFlags;
    desc.CPUAccessFlags = d3d11Types::asResourceCPUAccessFlag(usage);

    D3D11_SUBRESOURCE_DATA* initDataPtr = nullptr;
    D3D11_SUBRESOURCE_DATA initData;
    Memory::Clear(&initData, sizeof(initData));
    if (data) {
        initData.pSysMem = data;
        initDataPtr = &initData;
    }
    ID3D11Buffer* buf = nullptr;
    HRESULT hr = this->d3d11Device->CreateBuffer(&desc, initDataPtr, &buf);
    o_assert(SUCCEEDED(hr));
    o_assert_dbg(buf);

    return buf;
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d11MeshFactory::createFullscreenQuad(mesh& mesh) {
    o_assert_dbg(nullptr == mesh.d3d11VertexBuffer);
    o_assert_dbg(nullptr == mesh.d3d11IndexBuffer);

    VertexBufferAttrs vbAttrs;
    vbAttrs.NumVertices = 4;
    vbAttrs.BufferUsage = Usage::Immutable;
    vbAttrs.Layout.Add(VertexAttr::Position, VertexFormat::Float3);
    vbAttrs.Layout.Add(VertexAttr::TexCoord0, VertexFormat::Float2);
    mesh.vertexBufferAttrs = vbAttrs;

    IndexBufferAttrs ibAttrs;
    ibAttrs.NumIndices = 6;
    ibAttrs.Type = IndexType::Index16;
    ibAttrs.BufferUsage = Usage::Immutable;
    mesh.indexBufferAttrs = ibAttrs;

    mesh.numPrimGroups = 1;
    mesh.primGroups[0] = PrimitiveGroup(0, 6);

    // vertices
    const float topV = mesh.Setup.FullScreenQuadFlipV ? 0.0f : 1.0f;
    const float botV = mesh.Setup.FullScreenQuadFlipV ? 1.0f : 0.0f;
    float vertices[] = {
        -1.0f, +1.0f, 0.0f, 0.0f, topV,     // top-left corner
        +1.0f, +1.0f, 0.0f, 1.0f, topV,     // top-right corner
        +1.0f, -1.0f, 0.0f, 1.0f, botV,     // bottom-right corner
        -1.0f, -1.0f, 0.0f, 0.0f, botV      // bottom-left corner
    };

    // indices
    uint16_t indices[] = {
        0, 2, 1,            // topleft -> bottomright -> topright
        0, 3, 2,            // topleft -> bottomleft -> bottomright
    };

    // create vertex and index buffer
    mesh.d3d11VertexBuffer = this->createBuffer(vertices, sizeof(vertices), D3D11_BIND_VERTEX_BUFFER, mesh.vertexBufferAttrs.BufferUsage);
    mesh.d3d11IndexBuffer = this->createBuffer(indices, sizeof(indices), D3D11_BIND_INDEX_BUFFER, mesh.indexBufferAttrs.BufferUsage);

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
d3d11MeshFactory::setupAttrs(mesh& mesh) {

    VertexBufferAttrs vbAttrs;
    vbAttrs.NumVertices = mesh.Setup.NumVertices;
    vbAttrs.Layout = mesh.Setup.Layout;
    vbAttrs.BufferUsage = mesh.Setup.VertexUsage;
    mesh.vertexBufferAttrs = vbAttrs;

    IndexBufferAttrs ibAttrs;
    ibAttrs.NumIndices = mesh.Setup.NumIndices;
    ibAttrs.Type = mesh.Setup.IndicesType;
    ibAttrs.BufferUsage = mesh.Setup.IndexUsage;
    mesh.indexBufferAttrs = ibAttrs;
}

//------------------------------------------------------------------------------
void
d3d11MeshFactory::setupPrimGroups(mesh& mesh) {
    mesh.numPrimGroups = mesh.Setup.NumPrimitiveGroups();
    o_assert_dbg(mesh.numPrimGroups < GfxConfig::MaxNumPrimGroups);
    for (int i = 0; i < mesh.numPrimGroups; i++) {
        mesh.primGroups[i] = mesh.Setup.PrimitiveGroup(i);
    }
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d11MeshFactory::createBuffers(mesh& mesh, const void* data, int size) {
    o_assert_dbg(nullptr == mesh.d3d11VertexBuffer);
    o_assert_dbg(nullptr == mesh.d3d11IndexBuffer);

    this->setupAttrs(mesh);
    this->setupPrimGroups(mesh);
    const auto& vbAttrs = mesh.vertexBufferAttrs;
    const auto& ibAttrs = mesh.indexBufferAttrs;
    
    if (mesh.Setup.NumVertices > 0) {
        const int vbSize = vbAttrs.NumVertices * vbAttrs.Layout.ByteSize();
        const uint8_t* vertices = nullptr;
        if (InvalidIndex != mesh.Setup.DataVertexOffset) {
            const uint8_t* ptr = (const uint8_t*)data;
            o_assert_dbg(ptr && (size > 0));
            vertices = ptr + mesh.Setup.DataVertexOffset;
            o_assert_dbg((ptr + size) >= (vertices + vbSize));
        }
        mesh.d3d11VertexBuffer = this->createBuffer(vertices, vbSize, D3D11_BIND_VERTEX_BUFFER, vbAttrs.BufferUsage);
    }
    if (IndexType::None != ibAttrs.Type) {
        const int ibSize = ibAttrs.NumIndices * IndexType::ByteSize(ibAttrs.Type);
        const uint8_t* indices = nullptr;
        if (InvalidIndex != mesh.Setup.DataIndexOffset) {
            const uint8_t* ptr = (const uint8_t*)data;
            o_assert_dbg(ptr && (size > 0));
            indices = ((const uint8_t*)ptr) + mesh.Setup.DataIndexOffset;
            o_assert_dbg((ptr + size) >= (indices + ibSize));
        }
        mesh.d3d11IndexBuffer = this->createBuffer(indices, ibSize, D3D11_BIND_INDEX_BUFFER, ibAttrs.BufferUsage);
    }
    return ResourceState::Valid;
}

} // namespace _priv
} // namespace Oryol
