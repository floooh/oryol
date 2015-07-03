//------------------------------------------------------------------------------
//  glMeshFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glMeshFactory.h"
#include "Gfx/Resource/resourcePools.h"
#include "Gfx/Resource/mesh.h"
#include "Gfx/gl/gl_impl.h"
#include "Gfx/Core/renderer.h"
#include "Gfx/gl/glTypes.h"
#include "Gfx/gl/glExt.h"
#include "Resource/ResourceState.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
glMeshFactory::glMeshFactory() :
renderer(nullptr),
meshPool(nullptr),
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
glMeshFactory::~glMeshFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
glMeshFactory::Setup(class renderer* rendr, class meshPool* mshPool) {
    o_assert_dbg(!this->isValid);
    o_assert_dbg(nullptr != rendr);
    o_assert_dbg(nullptr != mshPool);
    this->isValid = true;
    this->renderer = rendr;
    this->meshPool = mshPool;
}

//------------------------------------------------------------------------------
void
glMeshFactory::Discard() {
    o_assert_dbg(this->isValid);
    this->isValid = false;
    this->renderer = nullptr;
    this->meshPool = nullptr;
}

//------------------------------------------------------------------------------
bool
glMeshFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
ResourceState::Code
glMeshFactory::SetupResource(mesh& msh) {
    o_assert_dbg(this->isValid);
    if (msh.Setup.ShouldSetupEmpty()) {
        return this->createEmptyMesh(msh);
    }
    else if (msh.Setup.ShouldSetupFullScreenQuad()) {
        return this->createFullscreenQuad(msh);
    }
    else {
        o_error("glMeshFactory::SetupResource(): don't know how to create mesh!");
        return ResourceState::InvalidState;
    }
}

//------------------------------------------------------------------------------
ResourceState::Code
glMeshFactory::SetupResource(mesh& msh, const void* data, int32 size) {
    o_assert_dbg(msh.Setup.ShouldSetupFromData());
    return this->createFromData(msh, data, size);
}

//------------------------------------------------------------------------------
void
glMeshFactory::DestroyResource(mesh& mesh) {
    o_assert_dbg(nullptr != this->renderer);
    
    this->renderer->invalidateMeshState();
    
    for (uint8 i = 0; i < mesh.numVertexBufferSlots; i++) {
        GLuint vb = mesh.glVertexBuffers[i];
        if (0 != vb) {
            ::glDeleteBuffers(1, &vb);
        }
    }
    if (0 != mesh.glIndexBuffer) {
        ::glDeleteBuffers(1, &mesh.glIndexBuffer);
    }
    mesh.Clear();
}

//------------------------------------------------------------------------------
/**
 NOTE: this method can be called with a nullptr for vertexData, in this case
 an empty GL buffer object will be created with the requested size.
*/
GLuint
glMeshFactory::createVertexBuffer(const void* vertexData, uint32 vertexDataSize, Usage::Code usage) {
    o_assert_dbg(nullptr != this->renderer);
    o_assert_dbg(vertexDataSize > 0);
    
    this->renderer->invalidateMeshState();
    GLuint vb = 0;
    ::glGenBuffers(1, &vb);
    ORYOL_GL_CHECK_ERROR();
    o_assert_dbg(0 != vb);
    this->renderer->bindVertexBuffer(vb);
    ::glBufferData(GL_ARRAY_BUFFER, vertexDataSize, vertexData, usage);
    ORYOL_GL_CHECK_ERROR();
    this->renderer->invalidateMeshState();
    return vb;
}

//------------------------------------------------------------------------------
/**
 NOTE: this method can be called with a nullptr for indexData, in this case
 an empty GL buffer object will be created with the requested size.
*/
GLuint
glMeshFactory::createIndexBuffer(const void* indexData, uint32 indexDataSize, Usage::Code usage) {
    o_assert_dbg(nullptr != this->renderer);
    o_assert_dbg(indexDataSize > 0);
    
    this->renderer->invalidateMeshState();
    GLuint ib = 0;
    ::glGenBuffers(1, &ib);
    ORYOL_GL_CHECK_ERROR();
    o_assert_dbg(0 != ib);
    this->renderer->bindIndexBuffer(ib);
    ::glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSize, indexData, usage);
    ORYOL_GL_CHECK_ERROR();
    this->renderer->invalidateMeshState();
    return ib;
}

//------------------------------------------------------------------------------
ResourceState::Code
glMeshFactory::createFullscreenQuad(mesh& mesh) {

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
    mesh.glVertexBuffers[0] = this->createVertexBuffer(vertices, sizeof(vertices), mesh.vertexBufferAttrs.BufferUsage);
    mesh.glIndexBuffer = this->createIndexBuffer(indices, sizeof(indices), mesh.indexBufferAttrs.BufferUsage);

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
ResourceState::Code
glMeshFactory::createEmptyMesh(mesh& mesh) {
    o_assert_dbg(0 < mesh.Setup.NumVertices);
    
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
    o_assert_dbg(mesh.numPrimGroups < mesh::MaxNumPrimGroups);
    for (int32 i = 0; i < mesh.numPrimGroups; i++) {
        mesh.primGroups[i] = setup.PrimitiveGroup(i);
    }
    
    // if this is a stream update mesh, we actually create 2 vertex buffers for double-buffered updated
    if (Usage::Stream == vbAttrs.BufferUsage) {
        const uint8 numSlots = 2;
        mesh.numVertexBufferSlots = numSlots;
        for (uint8 slotIndex = 0; slotIndex < numSlots; slotIndex++) {
            mesh.glVertexBuffers[slotIndex] = this->createVertexBuffer(nullptr, vbSize, vbAttrs.BufferUsage);
        }
    }
    else {
        // normal static or dynamic mesh, no double-buffering
        mesh.glVertexBuffers[0] = this->createVertexBuffer(nullptr, vbSize, vbAttrs.BufferUsage);
    }
    if (IndexType::None != ibAttrs.Type) {
        mesh.glIndexBuffer = this->createIndexBuffer(nullptr, ibSize, ibAttrs.BufferUsage);
    }

    return ResourceState::Valid;
}
    
//------------------------------------------------------------------------------
ResourceState::Code
glMeshFactory::createFromData(mesh& mesh, const void* data, int32 size) {
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
    o_assert_dbg(mesh.numPrimGroups < mesh::MaxNumPrimGroups);
    for (int32 i = 0; i < mesh.numPrimGroups; i++) {
        mesh.primGroups[i] = setup.PrimitiveGroup(i);
    }
    
    // setup the mesh object
    const uint8* ptr = (const uint8*)data;
    const uint8* vertices = ptr + setup.DataVertexOffset;
    const int32 verticesByteSize = setup.NumVertices * setup.Layout.ByteSize();
    o_assert_dbg((ptr + size) >= (vertices + verticesByteSize));
    mesh.glVertexBuffers[0] = this->createVertexBuffer(vertices, verticesByteSize, setup.VertexUsage);
    if (setup.IndicesType != IndexType::None) {
        o_assert_dbg(setup.DataIndexOffset != InvalidIndex);
        o_assert_dbg(setup.DataIndexOffset >= verticesByteSize);
        const uint8* indices = ptr + setup.DataIndexOffset;
        const int32 indicesByteSize = setup.NumIndices * IndexType::ByteSize(setup.IndicesType);
        o_assert_dbg((ptr + size) >= (indices + indicesByteSize));
        mesh.glIndexBuffer = this->createIndexBuffer(indices, indicesByteSize, setup.IndexUsage);
    }
    
    return ResourceState::Valid;
}

} // namespace _priv
} // namespace Oryol
