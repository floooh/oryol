//------------------------------------------------------------------------------
//  vlkMeshFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "vlk_impl.h"
#include "vlkMeshFactory.h"
#include "Gfx/Resource/resource.h"
#include "Gfx/Core/gfxPointers.h"
#include "Gfx/Core/displayMgr.h"
#include "Core/Assertion.h"
#include "Gfx/vlk/vlkResAllocator.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
vlkMeshFactory::~vlkMeshFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
vlkMeshFactory::Setup(const gfxPointers& ptrs) {
    o_assert_dbg(!this->isValid);
    this->isValid = true;
    this->pointers = ptrs;
}

//------------------------------------------------------------------------------
void
vlkMeshFactory::Discard() {
    o_assert_dbg(this->isValid);
    this->isValid = false;
}

//------------------------------------------------------------------------------
bool
vlkMeshFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
ResourceState::Code
vlkMeshFactory::SetupResource(mesh& msh) {
    o_assert_dbg(this->isValid);
    if (msh.Setup.ShouldSetupEmpty()) {
        return this->create(msh, nullptr, 0);
    }
    else if (msh.Setup.ShouldSetupFullScreenQuad()) {
        return this->createFullscreenQuad(msh);
    }
    else {
        o_error("vlkMeshFactory::SetupResource(): don't know how to create mesh!\n");
        return ResourceState::InvalidState;
    }
}

//------------------------------------------------------------------------------
ResourceState::Code
vlkMeshFactory::SetupResource(mesh& msh, const void* data, int32 size) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(msh.Setup.ShouldSetupFromData());
    return this->create(msh, data, size);
}

//------------------------------------------------------------------------------
void
vlkMeshFactory::DestroyResource(mesh& msh) {
    o_assert_dbg(this->isValid);

    vlkResAllocator& resAllocator = this->pointers.displayMgr->vlkContext.ResAllocator;
    const uint64 frameIndex = this->pointers.displayMgr->vlkContext.CurFrameIndex;
    vlkResAllocator::BufferItem freeItem;
    for (auto& buf : msh.buffers) {
        for (auto& slot : buf.slots) {
            if (slot.renderBuffer) {
                freeItem.buffer = slot.renderBuffer;
                freeItem.memory = slot.renderBufferMemory;
                resAllocator.releaseBuffer(frameIndex, freeItem);
            }
            if (slot.stagingBuffer) {
                freeItem.buffer = slot.stagingBuffer;
                freeItem.memory = slot.stagingBufferMemory;
                resAllocator.releaseBuffer(frameIndex, freeItem);
            }
        }
    }
    msh.Clear();
}

//------------------------------------------------------------------------------
void
vlkMeshFactory::setupAttrs(mesh& msh) {

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
vlkMeshFactory::setupPrimGroups(mesh& msh) {
    msh.numPrimGroups = msh.Setup.NumPrimitiveGroups();
    o_assert_dbg(msh.numPrimGroups < GfxConfig::MaxNumPrimGroups);
    for (int32 i = 0; i < msh.numPrimGroups; i++) {
        msh.primGroups[i] = msh.Setup.PrimitiveGroup(i);
    }
}

//------------------------------------------------------------------------------
void
vlkMeshFactory::createBuffers(mesh& msh, int bufType, Usage::Code usage, const void* data, int32 size) {

    vlkContext& context = this->pointers.displayMgr->vlkContext;
    vlkResAllocator& resAllocator = context.ResAllocator;
    const uint64 frameIndex = context.CurFrameIndex;
    VkDevice device = context.Device;
    VkCommandBuffer cmdBuf = context.curCommandBuffer();
    const VkBufferUsageFlags vlkUsageFlags = bufType == mesh::vb ? VK_BUFFER_USAGE_VERTEX_BUFFER_BIT : VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

    auto& buf = msh.buffers[bufType];
    buf.numSlots = Usage::Immutable == usage ? 1 : vlkConfig::NumFrames;
    for (uint8 slotIndex = 0; slotIndex < buf.numSlots; slotIndex++) {
        auto& slot = buf.slots[slotIndex];
        if (Usage::Stream == usage) {
            // Stream usage means the buffer is updated every frame, only
            // allocate an upload buffer, which the GPU directly renders from
            auto item = resAllocator.allocStagingBuffer(device, vlkUsageFlags, size);
            slot.stagingBuffer = item.buffer;
            slot.stagingBufferMemory = item.memory;
        }
        else if (Usage::Dynamic == usage) {
            // Dynamic update means the buffer is update infrequently, 
            // allocate an upload buffer and a 'fast' buffer which the GPU renders from
            auto item = resAllocator.allocStaticBuffer(device, cmdBuf, frameIndex, vlkUsageFlags, data, size);
            slot.renderBuffer = item.buffer;
            slot.renderBufferMemory = item.memory;
            item = resAllocator.allocStagingBuffer(device, 0, size);
            slot.stagingBuffer = item.buffer;
            slot.stagingBufferMemory = item.memory;
        }
        else {
            // immutable buffer with data
            auto item = resAllocator.allocStaticBuffer(device, cmdBuf, frameIndex, vlkUsageFlags, data, size);
            slot.renderBuffer = item.buffer;
            slot.renderBufferMemory = item.memory;
        }
    }
}

//------------------------------------------------------------------------------
ResourceState::Code
vlkMeshFactory::create(mesh& msh, const void* data, int32 size) {
    // generic buffer creation helper method to create vertex and/or 
    // index buffers with or without data for all usages
    o_assert_dbg(nullptr == msh.buffers[mesh::vb].slots[0].renderBuffer);
    o_assert_dbg(nullptr == msh.buffers[mesh::vb].slots[0].stagingBuffer);
    o_assert_dbg(1 == msh.buffers[mesh::vb].numSlots);
    o_assert_dbg(1 == msh.buffers[mesh::ib].numSlots);

    this->setupAttrs(msh);
    this->setupPrimGroups(msh);
    const auto& vbAttrs = msh.vertexBufferAttrs;
    const auto& ibAttrs = msh.indexBufferAttrs;

    // create optional vertex buffer
    if (msh.Setup.NumVertices > 0) {
        const int32 vbSize = vbAttrs.NumVertices * msh.Setup.Layout.ByteSize();
        const uint8* vertices = nullptr;
        if (InvalidIndex != msh.Setup.DataVertexOffset) {
            o_assert_dbg(data && (size > 0));
            const uint8* ptr = (const uint8*) data;
            vertices = ptr + msh.Setup.DataVertexOffset;
            o_assert_dbg((ptr + size) >= (vertices + vbSize));
        }
        this->createBuffers(msh, mesh::vb, vbAttrs.BufferUsage, vertices, vbSize);
    }

    // create optional index buffer
    if (ibAttrs.Type != IndexType::None) {
        const int32 ibSize = ibAttrs.NumIndices * IndexType::ByteSize(ibAttrs.Type);
        const uint8* indices = nullptr;
        if (InvalidIndex != msh.Setup.DataIndexOffset) {
            o_assert_dbg(data && (size > 0));
            const uint8* ptr = (const uint8*)data;
            indices = ptr + msh.Setup.DataIndexOffset;
            o_assert_dbg((ptr + size) >= (indices + ibSize));
        }
        this->createBuffers(msh, mesh::ib, ibAttrs.BufferUsage, indices, ibSize);
    }
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
ResourceState::Code
vlkMeshFactory::createFullscreenQuad(mesh& msh) {
    o_assert_dbg(nullptr == msh.buffers[mesh::vb].slots[0].renderBuffer);
    o_assert_dbg(nullptr == msh.buffers[mesh::vb].slots[0].stagingBuffer);
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
    const float32 topV = msh.Setup.FullScreenQuadFlipV ? 0.0f : 1.0f;
    const float32 botV = msh.Setup.FullScreenQuadFlipV ? 1.0f : 0.0f;
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

    // create buffer objects
    vlkContext& context = this->pointers.displayMgr->vlkContext;
    vlkResAllocator& resAllocator = context.ResAllocator;
    const uint64 frameIndex = context.CurFrameIndex;
    VkDevice device = context.Device;
    VkCommandBuffer cmdBuf = context.curCommandBuffer();

    auto item = resAllocator.allocStaticBuffer(device, cmdBuf, frameIndex, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, vertices, sizeof(vertices));
    auto& vbSlot = msh.buffers[mesh::vb].slots[0];
    vbSlot.renderBuffer = item.buffer;
    vbSlot.renderBufferMemory = item.memory;
    o_assert_dbg(msh.buffers[mesh::vb].slots[0].renderBuffer && msh.buffers[mesh::vb].slots[0].renderBufferMemory);
    
    item = resAllocator.allocStaticBuffer(device, cmdBuf, frameIndex, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, indices, sizeof(indices));
    auto& ibSlot = msh.buffers[mesh::ib].slots[0];
    ibSlot.renderBuffer = item.buffer;
    ibSlot.renderBufferMemory = item.memory;
    o_assert_dbg(msh.buffers[mesh::ib].slots[0].renderBuffer && msh.buffers[mesh::ib].slots[0].renderBufferMemory);

    return ResourceState::Valid;
}

} // namespace _priv
} // namespace Oryol
