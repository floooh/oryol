//------------------------------------------------------------------------------
//  mtlMeshFactory.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtl_impl.h"
#include "mtlMeshFactory.h"
#include "mtlTypes.h"
#include "Gfx/Resource/mesh.h"
#include "Gfx/Core/renderer.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
mtlMeshFactory::mtlMeshFactory() :
renderer(nullptr),
meshPool(nullptr),
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
mtlMeshFactory::~mtlMeshFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
mtlMeshFactory::Setup(class renderer* rendr, class meshPool* mshPool) {
    o_assert_dbg(!this->isValid);
    o_assert_dbg(nullptr != rendr);
    o_assert_dbg(nullptr != mshPool);
    this->isValid = true;
    this->renderer = rendr;
    this->meshPool = mshPool;
}

//------------------------------------------------------------------------------
void
mtlMeshFactory::Discard() {
    o_assert_dbg(this->isValid);
    this->isValid = false;
    this->renderer = nullptr;
    this->meshPool = nullptr;
}

//------------------------------------------------------------------------------
bool
mtlMeshFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
ResourceState::Code
mtlMeshFactory::SetupResource(mesh& msh) {
    o_assert_dbg(this->isValid);
    if (msh.Setup.ShouldSetupEmpty()) {
        o_error("FIXME!");
        // return this->createEmptyMesh(msh);
    }
    else if (msh.Setup.ShouldSetupFullScreenQuad()) {
        o_error("FIXME!");
        // return this->createFullscreenQuad(msh);
    }
    else {
        o_error("mtlMeshFactory::SetupResource(): don't know how to create mesh!\n");
        return ResourceState::InvalidState;
    }
}

//------------------------------------------------------------------------------
ResourceState::Code
mtlMeshFactory::SetupResource(mesh& msh, const void* data, int32 size) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(msh.Setup.ShouldSetupFromData());
    return this->createFromData(msh, data, size);
}

//------------------------------------------------------------------------------
void
mtlMeshFactory::DestroyResource(mesh& msh) {
    o_assert_dbg(this->renderer);
    this->renderer->invalidateMeshState();
    if (nil != msh.mtlVertexBuffer) {
        ORYOL_OBJC_RELEASE(msh.mtlVertexBuffer);
    }
    if (nil != msh.mtlIndexBuffer) {
        ORYOL_OBJC_RELEASE(msh.mtlIndexBuffer);
    }
    msh.Clear();
}

//------------------------------------------------------------------------------
/**
    NOTE: data pointer can be a nullptr
*/
id<MTLBuffer>
mtlMeshFactory::createBuffer(const void* data, uint32 dataSize, Usage::Code usage) {
    o_assert_dbg(dataSize > 0);
    o_assert_dbg(this->renderer && this->renderer->mtlDevice);

if (usage != Usage::Immutable) {
    o_error("FIXME: double buffering?\n");
}

    MTLResourceOptions options = mtlTypes::asBufferResourceOptions(usage);
    id<MTLBuffer> buf;
    if (data) {
        buf = [this->renderer->mtlDevice newBufferWithBytes:data length:dataSize options:options];
    }
    else {
        buf = [this->renderer->mtlDevice newBufferWithLength:dataSize options:options];
    }
    return buf;
}

//------------------------------------------------------------------------------
ResourceState::Code
mtlMeshFactory::createFromData(mesh& msh, const void* data, int32 size) {
    o_assert_dbg(nil == msh.mtlVertexBuffer);
    o_assert_dbg(nil == msh.mtlIndexBuffer);
    o_assert_dbg(nullptr != data);
    o_assert_dbg(size > 0);

    const MeshSetup& setup = msh.Setup;

    // setup vertex buffer attrs
    VertexBufferAttrs vbAttrs;
    vbAttrs.NumVertices = setup.NumVertices;
    vbAttrs.BufferUsage = setup.VertexUsage;
    vbAttrs.Layout = setup.Layout;
    vbAttrs.StepFunction = setup.StepFunction;
    vbAttrs.StepRate = setup.StepRate;
    msh.vertexBufferAttrs = vbAttrs;

    // setup index buffer attrs
    IndexBufferAttrs ibAttrs;
    ibAttrs.NumIndices = setup.NumIndices;
    ibAttrs.Type = setup.IndicesType;
    ibAttrs.BufferUsage = setup.IndexUsage;
    msh.indexBufferAttrs = ibAttrs;

    // setup primitive groups
    msh.numPrimGroups = setup.NumPrimitiveGroups();
    o_assert_dbg(msh.numPrimGroups < mesh::MaxNumPrimGroups);
    for (int32 i = 0; i < msh.numPrimGroups; i++) {
        msh.primGroups[i] = setup.PrimitiveGroup(i);
    }

    // setup the mesh object
    const uint8* ptr = (const uint8*) data;
    const uint8* vertices = ptr + setup.DataVertexOffset;
    const int32 verticesByteSize = setup.NumVertices * setup.Layout.ByteSize();
    o_assert_dbg((ptr + size) >= (vertices + verticesByteSize));
    msh.mtlVertexBuffer = this->createBuffer(vertices, verticesByteSize, setup.VertexUsage);
    o_assert_dbg(nil != msh.mtlVertexBuffer);
    if (setup.IndicesType != IndexType::None) {
        o_assert_dbg(setup.DataIndexOffset != InvalidIndex);
        o_assert_dbg(setup.DataIndexOffset >= verticesByteSize);
        const uint8* indices = ptr + setup.DataIndexOffset;
        const int32 indicesByteSize = setup.NumIndices * IndexType::ByteSize(setup.IndicesType);
        o_assert_dbg((ptr + size) >= (indices + indicesByteSize));
        msh.mtlIndexBuffer = this->createBuffer(indices, indicesByteSize, setup.IndexUsage);
        o_assert_dbg(nil != msh.mtlIndexBuffer);
    }
    return ResourceState::Valid;
}

} // namespace _priv
} // namespace Oryol
