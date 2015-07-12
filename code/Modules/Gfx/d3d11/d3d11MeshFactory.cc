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
d3d11MeshFactory::d3d11MeshFactory() :
renderer(nullptr),
meshPool(nullptr),
d3d11Device(nullptr),
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
d3d11MeshFactory::~d3d11MeshFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
d3d11MeshFactory::Setup(class renderer* rendr, class meshPool* mshPool) {
    o_assert_dbg(!this->isValid);
    o_assert_dbg(rendr);
    o_assert_dbg(mshPool);
    o_assert_dbg(rendr->d3d11Device);

    this->isValid = true;
    this->renderer = rendr;
    this->meshPool = mshPool;
    this->d3d11Device = renderer->d3d11Device;
}

//------------------------------------------------------------------------------
void
d3d11MeshFactory::Discard() {
    o_assert_dbg(this->isValid);
    this->isValid = false;
    this->renderer = nullptr;
    this->meshPool = nullptr;
    this->d3d11Device = nullptr;
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
        return this->createEmptyMesh(msh);
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
d3d11MeshFactory::SetupResource(mesh& msh, const void* data, int32 size) {
    o_assert_dbg(msh.Setup.ShouldSetupFromData());
    return this->createFromData(msh, data, size);
}

//------------------------------------------------------------------------------
void
d3d11MeshFactory::DestroyResource(mesh& mesh) {
    o_assert_dbg(nullptr != this->renderer);

    this->renderer->invalidateMeshState();

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
d3d11MeshFactory::createBuffer(const void* data, uint32 dataSize, uint32 d3d11BindFlags, Usage::Code usage) {
    o_assert_dbg(this->d3d11Device);
    o_assert_dbg((D3D11_BIND_VERTEX_BUFFER == d3d11BindFlags) || (D3D11_BIND_INDEX_BUFFER == d3d11BindFlags));

    D3D11_BUFFER_DESC desc;
    Memory::Clear(&desc, sizeof(desc));
    desc.ByteWidth = dataSize;
    desc.Usage = d3d11Types::asBufferUsage(usage);
    desc.BindFlags = d3d11BindFlags;
    desc.CPUAccessFlags = d3d11Types::asBufferCPUAccessFlag(usage);

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
    vbAttrs.StepFunction = VertexStepFunction::PerVertex;
    vbAttrs.StepRate = 1;
    mesh.vertexBufferAttrs = vbAttrs;

    IndexBufferAttrs ibAttrs;
    ibAttrs.NumIndices = 6;
    ibAttrs.Type = IndexType::Index16;
    ibAttrs.BufferUsage = Usage::Immutable;
    mesh.indexBufferAttrs = ibAttrs;

    mesh.numPrimGroups = 1;
    mesh.primGroups[0] = PrimitiveGroup(PrimitiveType::Triangles, 0, 6);

    // vertices
    const float32 topV = mesh.Setup.FullScreenQuadFlipV ? 0.0f : 1.0f;
    const float32 botV = mesh.Setup.FullScreenQuadFlipV ? 1.0f : 0.0f;
    float32 vertices[] = {
        -1.0f, +1.0f, 0.0f, 0.0f, topV,     // top-left corner
        +1.0f, +1.0f, 0.0f, 1.0f, topV,     // top-right corner
        +1.0f, -1.0f, 0.0f, 1.0f, botV,     // bottom-right corner
        -1.0f, -1.0f, 0.0f, 0.0f, botV      // bottom-left corner
    };

    // indices
    uint16 indices[] = {
        0, 2, 1,            // topleft -> bottomright -> topright
        0, 3, 2,            // topleft -> bottomleft -> bottomright
    };

    // create vertex and index buffer
    mesh.d3d11VertexBuffer = this->createBuffer(vertices, sizeof(vertices), D3D11_BIND_VERTEX_BUFFER, mesh.vertexBufferAttrs.BufferUsage);
    mesh.d3d11IndexBuffer = this->createBuffer(indices, sizeof(indices), D3D11_BIND_INDEX_BUFFER, mesh.indexBufferAttrs.BufferUsage);

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d11MeshFactory::createEmptyMesh(mesh& mesh) {
    o_assert_dbg(0 < mesh.Setup.NumVertices);
    o_assert_dbg(nullptr == mesh.d3d11VertexBuffer);
    o_assert_dbg(nullptr == mesh.d3d11IndexBuffer);

    const MeshSetup& setup = mesh.Setup;

    VertexBufferAttrs vbAttrs;
    vbAttrs.NumVertices = setup.NumVertices;
    vbAttrs.Layout = setup.Layout;
    vbAttrs.BufferUsage = setup.VertexUsage;
    vbAttrs.StepFunction = setup.StepFunction;
    vbAttrs.StepRate = setup.StepRate;
    mesh.vertexBufferAttrs = vbAttrs;
    const int32 vbSize = vbAttrs.NumVertices * vbAttrs.Layout.ByteSize();

    IndexBufferAttrs ibAttrs;
    ibAttrs.NumIndices = setup.NumIndices;
    ibAttrs.Type = setup.IndicesType;
    ibAttrs.BufferUsage = setup.IndexUsage;
    mesh.indexBufferAttrs = ibAttrs;
    const int32 ibSize = ibAttrs.NumIndices * IndexType::ByteSize(ibAttrs.Type);

    mesh.numPrimGroups = setup.NumPrimitiveGroups();
    o_assert_dbg(mesh.numPrimGroups < GfxConfig::MaxNumPrimGroups);
    for (int32 i = 0; i < mesh.numPrimGroups; i++) {
        mesh.primGroups[i] = setup.PrimitiveGroup(i);
    }

    mesh.d3d11VertexBuffer = this->createBuffer(nullptr, vbSize, D3D11_BIND_VERTEX_BUFFER, vbAttrs.BufferUsage);
    if (IndexType::None != ibAttrs.Type) {
        mesh.d3d11IndexBuffer  = this->createBuffer(nullptr, ibSize, D3D11_BIND_INDEX_BUFFER, ibAttrs.BufferUsage);
    }

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d11MeshFactory::createFromData(mesh& mesh, const void* data, int32 size) {
    o_assert_dbg(nullptr == mesh.d3d11VertexBuffer);
    o_assert_dbg(nullptr == mesh.d3d11IndexBuffer);
    o_assert_dbg(nullptr != data);
    o_assert_dbg(size > 0);

    const MeshSetup& setup = mesh.Setup;

    // setup vertex buffer attrs
    VertexBufferAttrs vbAttrs;
    vbAttrs.NumVertices = setup.NumVertices;
    vbAttrs.BufferUsage = setup.VertexUsage;
    vbAttrs.Layout = setup.Layout;
    vbAttrs.StepFunction = setup.StepFunction;
    vbAttrs.StepRate = setup.StepRate;
    mesh.vertexBufferAttrs = vbAttrs;

    // setup index buffer attrs
    IndexBufferAttrs ibAttrs;
    ibAttrs.NumIndices = setup.NumIndices;
    ibAttrs.Type = setup.IndicesType;
    ibAttrs.BufferUsage = setup.IndexUsage;
    mesh.indexBufferAttrs = ibAttrs;

    // setup primitive groups
    mesh.numPrimGroups = setup.NumPrimitiveGroups();
    o_assert_dbg(mesh.numPrimGroups < GfxConfig::MaxNumPrimGroups);
    for (int32 i = 0; i < mesh.numPrimGroups; i++) {
        mesh.primGroups[i] = setup.PrimitiveGroup(i);
    }

    // setup the mesh object
    const uint8* ptr = (const uint8*)data;
    const uint8* vertices = ptr + setup.DataVertexOffset;
    const int32 verticesByteSize = setup.NumVertices * setup.Layout.ByteSize();
    o_assert_dbg((ptr + size) >= (vertices + verticesByteSize));
    mesh.d3d11VertexBuffer = this->createBuffer(vertices, verticesByteSize, D3D11_BIND_VERTEX_BUFFER, setup.VertexUsage);
    if (setup.IndicesType != IndexType::None) {
        o_assert_dbg(setup.DataIndexOffset != InvalidIndex);
        o_assert_dbg(setup.DataIndexOffset >= verticesByteSize);
        const uint8* indices = ((const uint8*)ptr) + setup.DataIndexOffset;
        const int32 indicesByteSize = setup.NumIndices * IndexType::ByteSize(setup.IndicesType);
        o_assert_dbg((ptr + size) >= (indices + indicesByteSize));
        mesh.d3d11IndexBuffer = this->createBuffer(indices, indicesByteSize, D3D11_BIND_INDEX_BUFFER, setup.IndexUsage);
    }
    return ResourceState::Valid;
}

} // namespace _priv
} // namespace Oryol