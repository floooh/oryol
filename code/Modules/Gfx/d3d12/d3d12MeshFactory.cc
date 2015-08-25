//------------------------------------------------------------------------------
//  d3d12MeshFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "d3d12_impl.h"
#include "d3d12MeshFactory.h"
#include "Gfx/Resource/mesh.h"
#include "Gfx/Core/renderer.h"
#include "Gfx/Resource/gfxResourceContainer.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d12MeshFactory::d3d12MeshFactory() :
isValid(false) {
    // empty
}

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
bool
d3d12MeshFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d12MeshFactory::SetupResource(mesh& msh) {
    o_assert_dbg(this->isValid);
    if (msh.Setup.ShouldSetupEmpty()) {
        return this->createEmptyMesh(msh);    
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
d3d12MeshFactory::SetupResource(mesh& msh, const void* data, int32 size) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(msh.Setup.ShouldSetupFromData());
    return this->createFromData(msh, data, size);
}

//------------------------------------------------------------------------------
void
d3d12MeshFactory::DestroyResource(mesh& msh) {
    o_assert_dbg(this->isValid);

    d3d12ResourceAllocator& resAllocator = this->pointers.resContainer->resAllocator;
    this->pointers.renderer->invalidateMeshState();
    const uint64 frameIndex = this->pointers.renderer->frameIndex;
    for (auto& buf : msh.buffers) {
        for (ID3D12Resource* d3d12Res : buf.d3d12DefaultBuffers) {
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
    vbAttrs.StepFunction = msh.Setup.StepFunction;
    vbAttrs.StepRate = msh.Setup.StepRate;
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
    for (int32 i = 0; i < msh.numPrimGroups; i++) {
        msh.primGroups[i] = msh.Setup.PrimitiveGroup(i);
    }
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d12MeshFactory::createFromData(mesh& msh, const void* data, int32 size) {
    o_assert_dbg(nullptr == msh.buffers[mesh::vb].d3d12DefaultBuffers[0]);
    o_assert_dbg(nullptr == msh.buffers[mesh::ib].d3d12DefaultBuffers[0]);
    o_assert_dbg(1 == msh.buffers[mesh::vb].numSlots);
    o_assert_dbg(1 == msh.buffers[mesh::ib].numSlots);
    o_assert_dbg(nullptr != data);
    o_assert_dbg(size > 0);
    o_assert_dbg(Usage::Immutable == msh.Setup.VertexUsage);

    // FIXME: might make sense to put vertices, indices, and double buffer copies
    // into a single D3D12 buffer, but need to figure out whether 64kByte alignment
    // is needed on each subsection (guess: yes)
    // also need scatter-gather-copy for this in resource allocator AllocBuffer
    // ...so far this method is a copy of the Metal meshFactory implementation

    this->setupAttrs(msh);
    this->setupPrimGroups(msh);
    const auto& vbAttrs = msh.vertexBufferAttrs;
    const auto& ibAttrs = msh.indexBufferAttrs;
    o_assert_dbg(Usage::Immutable == vbAttrs.BufferUsage);
    o_assert_dbg(IndexType::None != ibAttrs.Type ? Usage::Immutable == ibAttrs.BufferUsage : true);
    
    d3d12ResourceAllocator& resAllocator = this->pointers.resContainer->resAllocator;
    ID3D12GraphicsCommandList* cmdList = this->pointers.renderer->d3d12CommandList; // FIXME: double buffered???
    const uint64 frameIndex = this->pointers.renderer->frameIndex;

    // create vertex buffer
    const uint8* ptr = (const uint8*) data;
    const uint8* vertices = ptr + msh.Setup.DataVertexOffset;
    const int32 vbSize = vbAttrs.NumVertices * msh.Setup.Layout.ByteSize();
    o_assert_dbg((ptr + size) >= (vertices + vbSize));
    msh.buffers[mesh::vb].d3d12DefaultBuffers[0] = resAllocator.AllocBuffer(cmdList, frameIndex, data, size);
    o_assert_dbg(nullptr != msh.buffers[mesh::vb].d3d12DefaultBuffers[0]);

    // create optional index buffer
    if (ibAttrs.Type != IndexType::None) {
        o_assert_dbg(Usage::Immutable == ibAttrs.BufferUsage);
        o_assert_dbg(InvalidIndex != msh.Setup.DataIndexOffset);
        o_assert_dbg(msh.Setup.DataIndexOffset >= (msh.Setup.DataVertexOffset + vbSize));
        const uint8* indices = ptr + msh.Setup.DataIndexOffset;
        const int32 ibSize = ibAttrs.NumIndices * IndexType::ByteSize(ibAttrs.Type);
        o_assert_dbg((ptr + size) >= (indices + ibSize));
        msh.buffers[mesh::ib].d3d12DefaultBuffers[0] = resAllocator.AllocBuffer(cmdList, frameIndex, data, size);
        o_assert_dbg(nullptr != msh.buffers[mesh::ib].d3d12DefaultBuffers[0]);
    }
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d12MeshFactory::createEmptyMesh(mesh& msh) {
    o_error("FIXME!");
    return ResourceState::Failed;
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d12MeshFactory::createFullscreenQuad(mesh& msh) {
    o_error("FIXME!");
    return ResourceState::Failed;
}

} // namespace _priv
} // namespace Oryol
