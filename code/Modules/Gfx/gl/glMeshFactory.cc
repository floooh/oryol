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
        o_assert_dbg(!msh.Setup.InstanceMesh.IsValid());
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
    for (uint8 i = 0; i < mesh.numVAOSlots; i++) {
        GLuint vao = mesh.glVAOs[i];
        if (0 != vao) {
            glExt::DeleteVertexArrays(1, &vao);
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
    
    GLuint ib = 0;
    ::glGenBuffers(1, &ib);
    ORYOL_GL_CHECK_ERROR();
    o_assert_dbg(0 != ib);
    this->renderer->invalidateMeshState();  // IMPORTANT to unlink current VAO
    this->renderer->bindIndexBuffer(ib);
    ::glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSize, indexData, usage);
    ORYOL_GL_CHECK_ERROR();
    this->renderer->invalidateMeshState();
    return ib;
}

//------------------------------------------------------------------------------
void
glMeshFactory::attachInstanceBuffer(mesh& msh) {
    const Id& instMeshId = msh.Setup.InstanceMesh;
    if (instMeshId.IsValid()) {
        o_assert_dbg(this->meshPool->QueryState(instMeshId) == ResourceState::Valid);
        const mesh* instMesh = this->meshPool->Lookup(instMeshId);
        o_assert_dbg(instMesh);
        msh.instanceMesh = instMesh;
        
        // verify that there are no colliding vertex components
        #if ORYOL_DEBUG
        const VertexLayout& mshLayout = msh.vertexBufferAttrs.Layout;
        const VertexLayout& instLayout = instMesh->vertexBufferAttrs.Layout;
        for (int32 i = 0; i < mshLayout.NumComponents(); i++) {
            o_assert_dbg(!instLayout.Contains(mshLayout.ComponentAt(i).Attr));
        }
        #endif
    }
}

//------------------------------------------------------------------------------
void
glMeshFactory::setupVertexLayout(mesh& mesh) {
    o_assert_dbg(nullptr != this->renderer);
    
    // create and initialize vertex array object
    this->renderer->invalidateMeshState();
    
    // setup number of vertex array slots
    uint8 numVAOSlots;
    if (mesh.instanceMesh) {
        numVAOSlots = mesh.instanceMesh->numVertexBufferSlots;
    }
    else {
        numVAOSlots = mesh.numVertexBufferSlots;
    }
    mesh.numVAOSlots = numVAOSlots;
    
    // setup the GL vertex attributes
    this->glSetupVertexAttrs(mesh);
    
    // vertex array objects supported?
    if (glExt::HasExtension(glExt::VertexArrayObject)) {
        for (uint8 vaoSlotIndex = 0; vaoSlotIndex < numVAOSlots; vaoSlotIndex++) {
            GLuint vb = 0;
            GLuint vao = 0;
            glExt::GenVertexArrays(1, &vao);
            this->renderer->bindVertexArrayObject(vao);
            const GLuint ib = mesh.glIndexBuffer;
            this->renderer->bindIndexBuffer(ib);
            
            for (int32 attrIndex = 0; attrIndex < VertexAttr::NumVertexAttrs; attrIndex++) {
                const glVertexAttr& glAttr = mesh.glAttrs[vaoSlotIndex][attrIndex];
                if (glAttr.enabled) {
                    if (glAttr.vertexBuffer != vb) {
                        vb = glAttr.vertexBuffer;
                        ::glBindBuffer(GL_ARRAY_BUFFER, vb);
                    }
                    ::glVertexAttribPointer(glAttr.index,
                                            glAttr.size,
                                            glAttr.type,
                                            glAttr.normalized,
                                            glAttr.stride,
                                            (const GLvoid*) (GLintptr) glAttr.offset);
                    ORYOL_GL_CHECK_ERROR();
                    glExt::VertexAttribDivisor(glAttr.index, glAttr.divisor);
                    ORYOL_GL_CHECK_ERROR();
                    ::glEnableVertexAttribArray(glAttr.index);
                    ORYOL_GL_CHECK_ERROR();
                }
                else {
                    ::glDisableVertexAttribArray(glAttr.index);
                    ORYOL_GL_CHECK_ERROR();
                }
            }
            mesh.glVAOs[vaoSlotIndex] = vao;
        }
    }
    this->renderer->invalidateMeshState();
}

//------------------------------------------------------------------------------
void
glMeshFactory::glSetupVertexAttrs(mesh& msh) {
    
    // check if mesh needs instancing, and host supports instancing
    // and lookup the optional instancing mesh
    const glMesh* instMesh = msh.instanceMesh;
    if (nullptr != instMesh) {
        o_assert_dbg(glExt::HasExtension(glExt::InstancedArrays));
        
        // if instancing is used, geometry mesh cannot be dynamic
        // (FIXME: this is currently a rather arbitrary restriction)
        o_assert_dbg(msh.numVertexBufferSlots == 1);
    }

    // first disable all attrs
    const uint8 numVAOSlots = msh.numVAOSlots;
    for (uint8 vaoSlotIndex = 0; vaoSlotIndex < numVAOSlots; vaoSlotIndex++) {
        for (uint8 attrIndex = 0; attrIndex < VertexAttr::NumVertexAttrs; attrIndex++) {
            glVertexAttr& glAttr = msh.glAttrs[vaoSlotIndex][attrIndex];
            glAttr.enabled = GL_FALSE;
            glAttr.index = attrIndex;
            glAttr.vertexBuffer = 0;
            glAttr.divisor = 0;
        }
    }
    
    // setup glVertexAttr arrays
    for (uint8 vaoSlotIndex = 0; vaoSlotIndex < numVAOSlots; vaoSlotIndex++) {
        const glMesh* meshes[2] = { &msh, instMesh };
        for (int32 i = 0; i < 2; i++) {
            const glMesh* curMesh = meshes[i];
            if (nullptr != curMesh) {
                const VertexLayout& layout = curMesh->vertexBufferAttrs.Layout;
                const int32 numComps = layout.NumComponents();
                for (int i = 0; i < numComps; i++) {
                    const VertexLayout::Component& comp = layout.ComponentAt(i);
                    glVertexAttr& glAttr = msh.glAttrs[vaoSlotIndex][comp.Attr];  // msh is not a bug
                    glAttr.enabled = GL_TRUE;
                    if (curMesh == &msh) {
                        if (instMesh) {
                            // instancing situation: static geometry
                            glAttr.vertexBuffer = msh.glVertexBuffers[0];
                        }
                        else {
                            // non-instancing: static or dynamic geometry
                            glAttr.vertexBuffer = msh.glVertexBuffers[vaoSlotIndex];
                        }
                        glAttr.divisor = 0;
                    }
                    else {
                        // instancing: instance data
                        glAttr.vertexBuffer = instMesh->glVertexBuffers[vaoSlotIndex];
                        glAttr.divisor = 1;
                    }
                    switch (comp.Format) {
                        case VertexFormat::Float:
                            glAttr.size = 1;
                            glAttr.type = GL_FLOAT;
                            glAttr.normalized = GL_FALSE;
                            break;
                            
                        case VertexFormat::Float2:
                            glAttr.size = 2;
                            glAttr.type = GL_FLOAT;
                            glAttr.normalized = GL_FALSE;
                            break;
                            
                        case VertexFormat::Float3:
                            glAttr.size = 3;
                            glAttr.type = GL_FLOAT;
                            glAttr.normalized = GL_FALSE;
                            break;
                            
                        case VertexFormat::Float4:
                            glAttr.size = 4;
                            glAttr.type = GL_FLOAT;
                            glAttr.normalized = GL_FALSE;
                            break;
                            
                        case VertexFormat::Byte4:
                            glAttr.size = 4;
                            glAttr.type = GL_BYTE;
                            glAttr.normalized = GL_FALSE;
                            break;
                            
                        case VertexFormat::Byte4N:
                            glAttr.size = 4;
                            glAttr.type = GL_BYTE;
                            glAttr.normalized = GL_TRUE;
                            break;
                            
                        case VertexFormat::UByte4:
                            glAttr.size = 4;
                            glAttr.type = GL_UNSIGNED_BYTE;
                            glAttr.normalized = GL_FALSE;
                            break;
                            
                        case VertexFormat::UByte4N:
                            glAttr.size = 4;
                            glAttr.type = GL_UNSIGNED_BYTE;
                            glAttr.normalized = GL_TRUE;
                            break;
                            
                        case VertexFormat::Short2:
                            glAttr.size = 2;
                            glAttr.type = GL_SHORT;
                            glAttr.normalized = GL_FALSE;
                            break;
                            
                        case VertexFormat::Short2N:
                            glAttr.size = 2;
                            glAttr.type = GL_SHORT;
                            glAttr.normalized = GL_TRUE;
                            break;
                            
                        case VertexFormat::Short4:
                            glAttr.size = 4;
                            glAttr.type = GL_SHORT;
                            glAttr.normalized = GL_FALSE;
                            break;
                            
                        case VertexFormat::Short4N:
                            glAttr.size = 4;
                            glAttr.type = GL_SHORT;
                            glAttr.normalized = GL_TRUE;
                            break;
                            
                        default:
                            o_error("glMeshFactory::glSetupVertexAttrs(): invalid vertex format!\n");
                            break;
                    }
                    glAttr.stride = layout.ByteSize();
                    glAttr.offset = layout.ComponentByteOffset(i);
                }
            }
        }
    }
}

//------------------------------------------------------------------------------
ResourceState::Code
glMeshFactory::createFullscreenQuad(mesh& mesh) {
    o_assert_dbg(!mesh.Setup.InstanceMesh.IsValid());
    
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
    mesh.primGroups[0] = PrimitiveGroup(PrimitiveType::Triangles, 0, 6);

    // vertices
    float32 vertices[] = {
        -1.0f, +1.0f, 0.0f, 0.0f, 1.0f,     // top-left corner
        +1.0f, +1.0f, 0.0f, 1.0f, 1.0f,     // top-right corner
        +1.0f, -1.0f, 0.0f, 1.0f, 0.0f,     // bottom-right corner
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,     // bottom-left corner
    };

    // indices
    uint16 indices[] = {
        0, 2, 1,            // topleft -> bottomright -> topright
        0, 3, 2,            // topleft -> bottomleft -> bottomright
    };
    
    // create vertex and index buffer
    mesh.glVertexBuffers[0] = this->createVertexBuffer(vertices, sizeof(vertices), mesh.vertexBufferAttrs.BufferUsage);
    mesh.glIndexBuffer = this->createIndexBuffer(indices, sizeof(indices), mesh.indexBufferAttrs.BufferUsage);
    this->attachInstanceBuffer(mesh);
    this->setupVertexLayout(mesh);
    
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
        mesh.numVAOSlots = numSlots;
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
    this->attachInstanceBuffer(mesh);
    this->setupVertexLayout(mesh);
    
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
        const uint8* indices = ((const uint8*)ptr) + setup.DataIndexOffset;
        const int32 indicesByteSize = setup.NumIndices * IndexType::ByteSize(setup.IndicesType);
        o_assert_dbg((ptr + size) >= (indices + indicesByteSize));
        mesh.glIndexBuffer = this->createIndexBuffer(indices, indicesByteSize, setup.IndexUsage);
    }
    
    this->attachInstanceBuffer(mesh);
    this->setupVertexLayout(mesh);

    return ResourceState::Valid;
}

} // namespace _priv
} // namespace Oryol
