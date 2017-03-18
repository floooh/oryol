//------------------------------------------------------------------------------
//  d3d12MeshFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d12_impl.h"
#include "d3d12Types.h"
#include "d3d12MeshFactory.h"
#include "Gfx/Resource/resource.h"
#include "Gfx/Core/renderer.h"
#include "Gfx/Resource/gfxResourceContainer.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d12MeshFactory::~d3d12MeshFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
d3d12MeshFactory::Setup(const gfxPointers& ptrs) {
    o_assert_dbg(!this->isValid);
    this->isValid = true;
    this->pointers = ptrs;
}

//------------------------------------------------------------------------------
void
d3d12MeshFactory::Discard() {
    o_assert_dbg(this->isValid);
    this->pointers = gfxPointers();
    this->isValid = false;
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d12MeshFactory::SetupResource(mesh& msh) {
    o_assert_dbg(this->isValid);
    if (msh.Setup.ShouldSetupEmpty()) {
        return this->create(msh, nullptr, 0);    
    }
    else if (msh.Setup.ShouldSetupFullScreenQuad()) {
        return this->createFullscreenQuad(msh);    
    }
    else {
        o_error("d3d12MeshFactory::SetupResource(): don't know how to create mesh!\n");
        return ResourceState::InvalidState;
    }
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d12MeshFactory::SetupResource(mesh& msh, const void* data, int size) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(msh.Setup.ShouldSetupFromData());
    return this->create(msh, data, size);
}

//------------------------------------------------------------------------------
void
d3d12MeshFactory::DestroyResource(mesh& msh) {
    o_assert_dbg(this->isValid);

    d3d12ResAllocator& resAllocator = this->pointers.renderer->resAllocator;
    const uint64_t frameIndex = this->pointers.renderer->frameIndex;
    for (auto& buf : msh.buffers) {
        for (ID3D12Resource* d3d12Res : buf.d3d12RenderBuffers) {
            if (d3d12Res) {
                resAllocator.ReleaseDeferred(frameIndex, d3d12Res);
            }
        }
        for (ID3D12Resource* d3d12Res : buf.d3d12UploadBuffers) {
            if (d3d12Res) {
                resAllocator.ReleaseDeferred(frameIndex, d3d12Res);
            }
        }
    }
    msh.Clear();
}

//------------------------------------------------------------------------------
void
d3d12MeshFactory::setupAttrs(mesh& msh) {
    
    VertexBufferAttrs vbAttrs;
    vbAttrs.NumVertices = msh.Setup.NumVertices;
    vbAttrs.Layout = msh.Setup.Layout;
    vbAttrs.BufferUsage = msh.Setup.VertexUsage;
    msh.vertexBufferAttrs = vbAttrs;

    IndexBufferAttrs ibAttrs;
    ibAttrs.NumIndices = msh.Setup.NumIndices;
    ibAttrs.Type = msh.Setup.IndicesType;
    ibAttrs.BufferUsage = msh.Setup.IndexUsage;
    msh.indexBufferAttrs = ibAttrs;
}

//------------------------------------------------------------------------------
void
d3d12MeshFactory::setupPrimGroups(mesh& msh) {
    msh.numPrimGroups = msh.Setup.NumPrimitiveGroups();
    o_assert_dbg(msh.numPrimGroups < GfxConfig::MaxNumPrimGroups);
    for (int i = 0; i < msh.numPrimGroups; i++) {
        msh.primGroups[i] = msh.Setup.PrimitiveGroup(i);
    }
}

//------------------------------------------------------------------------------
void
d3d12MeshFactory::createBuffers(mesh& msh, int bufType, Usage::Code usage, const void* data, int size) {
    // FIXME: might make sense to put vertices, indices, and double buffer copies
    // into a single D3D12 buffer, but need to figure out whether 64kByte alignment
    // is needed on each subsection (guess: yes)
    // also need scatter-gather-copy for this in resource allocator AllocBuffer
    // ...so far this method is a copy of the Metal meshFactory implementation

    ID3D12Device* d3d12Device = this->pointers.renderer->d3d12Device;
    o_assert_dbg(d3d12Device);
    d3d12ResAllocator& resAllocator = this->pointers.renderer->resAllocator;
    ID3D12GraphicsCommandList* cmdList = this->pointers.renderer->curCommandList();
    const uint64_t frameIndex = this->pointers.renderer->frameIndex;
    D3D12_RESOURCE_STATES initState;
    if (mesh::ib == bufType) {
        initState = D3D12_RESOURCE_STATE_INDEX_BUFFER;
    }
    else {
        initState = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
    }

    msh.buffers[bufType].numSlots = Usage::Immutable == usage ? 1 : d3d12Mesh::NumSlots;
    for (uint8_t slotIndex = 0; slotIndex < msh.buffers[bufType].numSlots; slotIndex++) {
        if (Usage::Stream == usage) {
            // Stream usage means the buffer is updated every frame, only
            // allocate an upload buffer, which the GPU directly renders from
            msh.buffers[bufType].d3d12RenderBuffers[slotIndex] = resAllocator.AllocUploadBuffer(d3d12Device, size);
            msh.buffers[bufType].d3d12UploadBuffers[slotIndex] = nullptr;
        }
        else if (Usage::Dynamic == usage) {
            // Dynamic update means the buffer is update infrequently, 
            // allocate an upload buffer and a 'fast' buffer which the GPU renders from
            msh.buffers[bufType].d3d12RenderBuffers[slotIndex] = resAllocator.AllocStaticBuffer(d3d12Device, cmdList, frameIndex, initState, data, size);
            msh.buffers[bufType].d3d12UploadBuffers[slotIndex] = resAllocator.AllocUploadBuffer(d3d12Device, size);
            o_assert_dbg(nullptr != msh.buffers[bufType].d3d12UploadBuffers[slotIndex]);
        }
        else {
            // immutable buffer with data
            msh.buffers[bufType].d3d12RenderBuffers[slotIndex] = resAllocator.AllocStaticBuffer(d3d12Device, cmdList, frameIndex, initState, data, size);
            msh.buffers[bufType].d3d12UploadBuffers[slotIndex] = nullptr;
        }
        o_assert_dbg(nullptr != msh.buffers[bufType].d3d12RenderBuffers[slotIndex]);
    }
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d12MeshFactory::create(mesh& msh, const void* data, int size) {
    // generic buffer creation helper method to create vertex and/or 
    // index buffers with or without data for all usages
    o_assert_dbg(nullptr == msh.buffers[mesh::vb].d3d12RenderBuffers[0]);
    o_assert_dbg(nullptr == msh.buffers[mesh::vb].d3d12UploadBuffers[0]);
    o_assert_dbg(1 == msh.buffers[mesh::vb].numSlots);
    o_assert_dbg(1 == msh.buffers[mesh::ib].numSlots);

    this->setupAttrs(msh);
    this->setupPrimGroups(msh);
    const auto& vbAttrs = msh.vertexBufferAttrs;
    const auto& ibAttrs = msh.indexBufferAttrs;

    // create optional vertex buffer
    if (msh.Setup.NumVertices > 0) {
        const int vbSize = vbAttrs.NumVertices * msh.Setup.Layout.ByteSize();
        const uint8_t* vertices = nullptr;
        if (InvalidIndex != msh.Setup.DataVertexOffset) {
            o_assert_dbg(data && (size > 0));
            const uint8_t* ptr = (const uint8_t*) data;
            vertices = ptr + msh.Setup.DataVertexOffset;
            o_assert_dbg((ptr + size) >= (vertices + vbSize));
        }
        this->createBuffers(msh, mesh::vb, vbAttrs.BufferUsage, vertices, vbSize);
    }

    // create optional index buffer
    if (ibAttrs.Type != IndexType::None) {
        const int ibSize = ibAttrs.NumIndices * IndexType::ByteSize(ibAttrs.Type);
        const uint8_t* indices = nullptr;
        if (InvalidIndex != msh.Setup.DataIndexOffset) {
            o_assert_dbg(data && (size > 0));
            const uint8_t* ptr = (const uint8_t*)data;
            indices = ptr + msh.Setup.DataIndexOffset;
            o_assert_dbg((ptr + size) >= (indices + ibSize));
        }
        this->createBuffers(msh, mesh::ib, ibAttrs.BufferUsage, indices, ibSize);
    }
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d12MeshFactory::createFullscreenQuad(mesh& msh) {
    o_assert_dbg(nullptr == msh.buffers[mesh::vb].d3d12RenderBuffers[0]);
    o_assert_dbg(nullptr == msh.buffers[mesh::ib].d3d12RenderBuffers[0]);
    o_assert_dbg(1 == msh.buffers[mesh::vb].numSlots);
    o_assert_dbg(1 == msh.buffers[mesh::ib].numSlots);

    VertexBufferAttrs vbAttrs;
    vbAttrs.NumVertices = 4;
    vbAttrs.BufferUsage = Usage::Immutable;
    vbAttrs.Layout.Add(VertexAttr::Position, VertexFormat::Float3);
    vbAttrs.Layout.Add(VertexAttr::TexCoord0, VertexFormat::Float2);
    msh.vertexBufferAttrs = vbAttrs;

    IndexBufferAttrs ibAttrs;
    ibAttrs.NumIndices = 6;
    ibAttrs.Type = IndexType::Index16;
    ibAttrs.BufferUsage = Usage::Immutable;
    msh.indexBufferAttrs = ibAttrs;

    msh.numPrimGroups = 1;
    msh.primGroups[0] = PrimitiveGroup(0, 6);

    // vertices
    const float topV = msh.Setup.FullScreenQuadFlipV ? 0.0f : 1.0f;
    const float botV = msh.Setup.FullScreenQuadFlipV ? 1.0f : 0.0f;
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
    
    // create d3d12 buffers
    ID3D12Device* d3d12Device = this->pointers.renderer->d3d12Device;
    o_assert_dbg(d3d12Device);
    d3d12ResAllocator& resAllocator = this->pointers.renderer->resAllocator;
    ID3D12GraphicsCommandList* cmdList = this->pointers.renderer->curCommandList();
    const uint64_t frameIndex = this->pointers.renderer->frameIndex;

    msh.buffers[mesh::vb].d3d12RenderBuffers[0] = resAllocator.AllocStaticBuffer(d3d12Device, cmdList, frameIndex, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, vertices, sizeof(vertices));
    o_assert_dbg(nullptr != msh.buffers[mesh::vb].d3d12RenderBuffers[0]);

    msh.buffers[mesh::ib].d3d12RenderBuffers[0] = resAllocator.AllocStaticBuffer(d3d12Device, cmdList, frameIndex, D3D12_RESOURCE_STATE_INDEX_BUFFER, indices, sizeof(indices));
    o_assert_dbg(nullptr != msh.buffers[mesh::ib].d3d12RenderBuffers[0]);

    return ResourceState::Valid;
}

} // namespace _priv
} // namespace Oryol
