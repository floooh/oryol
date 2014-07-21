//------------------------------------------------------------------------------
//  glMeshFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Render/base/meshLoaderBase.h"
#include "glMeshFactory.h"
#include "Render/gl/gl_impl.h"
#include "Render/Core/stateWrapper.h"
#include "Render/Core/meshPool.h"
#include "Render/gl/glTypes.h"
#include "Render/gl/glExt.h"
#include "Resource/State.h"
#include <array>

namespace Oryol {
namespace Render {

using namespace Core;
using namespace IO;

//------------------------------------------------------------------------------
glMeshFactory::glMeshFactory() :
stateWrapper(nullptr),
meshPool(nullptr),
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
glMeshFactory::~glMeshFactory() {
    o_assert(!this->isValid);
}

//------------------------------------------------------------------------------
void
glMeshFactory::Setup(class stateWrapper* stWrapper, class meshPool* mshPool) {
    o_assert(!this->isValid);
    o_assert(nullptr != stWrapper);
    o_assert(nullptr != mshPool);
    this->isValid = true;
    this->stateWrapper = stWrapper;
    this->meshPool = mshPool;
}

//------------------------------------------------------------------------------
void
glMeshFactory::Discard() {
    o_assert(this->isValid);
    this->isValid = false;
    this->stateWrapper = nullptr;
    this->meshPool = nullptr;
}

//------------------------------------------------------------------------------
bool
glMeshFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
void
glMeshFactory::SetupResource(mesh& msh) {
    o_assert(this->isValid);
    o_assert((msh.GetState() == Resource::State::Setup) || (msh.GetState() == Resource::State::Pending));
    
    // decide whether a loader needs to take over, or whether we handle this right here
    const MeshSetup& setup = msh.GetSetup();
    if (setup.ShouldSetupEmpty()) {
        this->createEmptyMesh(msh);
        o_assert((msh.GetState() == Resource::State::Valid) || (msh.GetState() == Resource::State::Failed));
    }
    else if (setup.ShouldSetupFullScreenQuad()) {
        o_assert(!setup.InstanceMesh.IsValid());
        this->createFullscreenQuad(msh);
        o_assert(msh.GetState() == Resource::State::Valid);
    }
    else {
        // let a loader take over, parent class will take care of this
        o_assert(msh.GetSetup().ShouldSetupFromFile());
        Resource::loaderFactory<mesh, meshLoaderBase>::SetupResource(msh);
    }
}

//------------------------------------------------------------------------------
void
glMeshFactory::SetupResource(mesh& msh, const Ptr<Stream>& data) {
    Resource::loaderFactory<mesh, meshLoaderBase>::SetupResource(msh, data);
}

//------------------------------------------------------------------------------
void
glMeshFactory::DestroyResource(mesh& mesh) {
    o_assert(nullptr != this->stateWrapper);
    
    loaderFactory::DestroyResource(mesh);
    
    this->stateWrapper->InvalidateMeshState();
    
    for (uint8 i = 0; i < mesh.getNumVertexBufferSlots(); i++) {
        GLuint vb = mesh.glGetVertexBuffer(i);
        if (0 != vb) {
            ::glDeleteBuffers(1, &vb);
        }
    }
    for (uint8 i = 0; i < mesh.getNumVAOSlots(); i++) {
        GLuint vao = mesh.glGetVAO(i);
        if (0 != vao) {
            glExt::DeleteVertexArrays(1, &vao);
        }
    }
    GLuint ib = mesh.glGetIndexBuffer();
    if (0 != ib) {
        ::glDeleteBuffers(1, &ib);
    }
    mesh.clear();
    mesh.setState(Resource::State::Setup);
}

//------------------------------------------------------------------------------
/**
 NOTE: this method can be called with a nullptr for vertexData, in this case
 an empty GL buffer object will be created with the requested size.
*/
GLuint
glMeshFactory::createVertexBuffer(const void* vertexData, uint32 vertexDataSize, Usage::Code usage) {
    o_assert(nullptr != this->stateWrapper);
    o_assert(vertexDataSize > 0);
    
    this->stateWrapper->InvalidateMeshState();
    GLuint vb = 0;
    ::glGenBuffers(1, &vb);
    ORYOL_GL_CHECK_ERROR();
    o_assert(0 != vb);
    this->stateWrapper->BindVertexBuffer(vb);
    ::glBufferData(GL_ARRAY_BUFFER, vertexDataSize, vertexData, usage);
    ORYOL_GL_CHECK_ERROR();
    this->stateWrapper->InvalidateMeshState();
    return vb;
}

//------------------------------------------------------------------------------
/**
 NOTE: this method can be called with a nullptr for indexData, in this case
 an empty GL buffer object will be created with the requested size.
*/
GLuint
glMeshFactory::createIndexBuffer(const void* indexData, uint32 indexDataSize, Usage::Code usage) {
    o_assert(nullptr != this->stateWrapper);
    o_assert(indexDataSize > 0);
    
    GLuint ib = 0;
    ::glGenBuffers(1, &ib);
    ORYOL_GL_CHECK_ERROR();
    o_assert(0 != ib);
    this->stateWrapper->InvalidateMeshState();  // IMPORTANT to unlink current VAO
    this->stateWrapper->BindIndexBuffer(ib);
    ::glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSize, indexData, usage);
    ORYOL_GL_CHECK_ERROR();
    this->stateWrapper->InvalidateMeshState();
    return ib;
}

//------------------------------------------------------------------------------
void
glMeshFactory::attachInstanceBuffer(mesh& msh) {
    const Resource::Id& instMeshId = msh.GetSetup().InstanceMesh;
    if (instMeshId.IsValid()) {
        o_assert(this->meshPool->QueryState(instMeshId) == Resource::State::Valid);
        const mesh* instMesh = this->meshPool->Lookup(instMeshId);
        msh.setInstanceMesh(instMesh);
        
        // verify that there are no colliding vertex components
        const VertexLayout& mshLayout = msh.GetVertexBufferAttrs().Layout;
        const VertexLayout& instLayout = instMesh->GetVertexBufferAttrs().Layout;
        for (int32 i = 0; i < mshLayout.NumComponents(); i++) {
            o_assert(!instLayout.Contains(mshLayout.Component(i).Attr()));
        }
    }
}

//------------------------------------------------------------------------------
void
glMeshFactory::setupVertexLayout(mesh& mesh) {
    o_assert(mesh.GetState() != Resource::State::Valid);
    o_assert(nullptr != this->stateWrapper);
    
    // create and initialize vertex array object
    this->stateWrapper->InvalidateMeshState();
    
    // setup number of vertex array slots
    uint8 numVAOSlots;
    if (mesh.getInstanceMesh()) {
        numVAOSlots = mesh.getInstanceMesh()->getNumVertexBufferSlots();
    }
    else {
        numVAOSlots = mesh.getNumVertexBufferSlots();
    }
    mesh.setNumVAOSlots(numVAOSlots);
    
    // setup the GL vertex attributes
    this->glSetupVertexAttrs(mesh);
    
    // vertex array objects supported?
    if (glExt::HasExtension(glExt::VertexArrayObject)) {
        for (uint8 vaoSlotIndex = 0; vaoSlotIndex < numVAOSlots; vaoSlotIndex++) {
            GLuint vb = 0;
            GLuint vao = 0;
            glExt::GenVertexArrays(1, &vao);
            this->stateWrapper->BindVertexArrayObject(vao);
            const GLuint ib = mesh.glGetIndexBuffer();
            this->stateWrapper->BindIndexBuffer(ib);
            
            for (int32 attrIndex = 0; attrIndex < VertexAttr::NumVertexAttrs; attrIndex++) {
                const glVertexAttr& glAttr = mesh.glAttr(vaoSlotIndex, attrIndex);
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
            mesh.glSetVAO(vaoSlotIndex, vao);
        }
    }
    this->stateWrapper->InvalidateMeshState();
}

//------------------------------------------------------------------------------
void
glMeshFactory::glSetupVertexAttrs(mesh& msh) {
    
    // check if mesh needs instancing, and host supports instancing
    // and lookup the optional instancing mesh
    const glMesh* instMesh = msh.getInstanceMesh();
    if (nullptr != instMesh) {
        o_assert(glExt::HasExtension(glExt::InstancedArrays));
        
        // if instancing is used, geometry mesh cannot be dynamic
        // (FIXME: this is currently a rather arbitrary restriction)
        o_assert(msh.getNumVertexBufferSlots() == 1);
    }

    // first disable all attrs
    const uint8 numVAOSlots = msh.getNumVAOSlots();
    for (uint8 vaoSlotIndex = 0; vaoSlotIndex < numVAOSlots; vaoSlotIndex++) {
        for (uint8 attrIndex = 0; attrIndex < VertexAttr::NumVertexAttrs; attrIndex++) {
            glVertexAttr& glAttr = msh.glAttr(vaoSlotIndex, attrIndex);
            glAttr.enabled = GL_FALSE;
            glAttr.index = attrIndex;
            glAttr.vertexBuffer = 0;
            glAttr.divisor = 0;
        }
    }
    
    // setup glVertexAttr arrays
    for (uint8 vaoSlotIndex = 0; vaoSlotIndex < numVAOSlots; vaoSlotIndex++) {
        const std::array<const glMesh*, 2> meshes{ { &msh, instMesh } };
        for (const glMesh* curMesh : meshes) {
            if (nullptr != curMesh) {
                const VertexLayout& layout = curMesh->GetVertexBufferAttrs().Layout;
                const int32 numComps = layout.NumComponents();
                for (int i = 0; i < numComps; i++) {
                    const VertexComponent& comp = layout.Component(i);
                    glVertexAttr& glAttr = msh.glAttr(vaoSlotIndex, comp.Attr());  // msh is not a bug
                    glAttr.enabled = GL_TRUE;
                    if (curMesh == &msh) {
                        if (instMesh) {
                            // instancing situation: static geometry
                            glAttr.vertexBuffer = msh.glGetVertexBuffer(0);
                        }
                        else {
                            // non-instancing: static or dynamic geometry
                            glAttr.vertexBuffer = msh.glGetVertexBuffer(vaoSlotIndex);
                        }
                        glAttr.divisor = 0;
                    }
                    else {
                        // instancing: instance data
                        glAttr.vertexBuffer = instMesh->glGetVertexBuffer(vaoSlotIndex);
                        glAttr.divisor = 1;
                    }
                    switch (comp.Format()) {
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
void
glMeshFactory::createFullscreenQuad(mesh& mesh) {
    o_assert(!mesh.GetSetup().InstanceMesh.IsValid());
    
    // vertices
    float32 vertices[] = {
        -1.0f, +1.0f, 0.0f, 0.0f, 1.0f,     // top-left corner
        +1.0f, +1.0f, 0.0f, 1.0f, 1.0f,     // top-right corner
        +1.0f, -1.0f, 0.0f, 1.0f, 0.0f,     // bottom-right corner
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,     // bottom-left corner
    };
    VertexBufferAttrs vbAttrs;
    vbAttrs.NumVertices = 4;
    vbAttrs.BufferUsage = Usage::Immutable;
    VertexLayout layout;
    layout.Add(VertexAttr::Position, VertexFormat::Float3);
    layout.Add(VertexAttr::TexCoord0, VertexFormat::Float2);
    vbAttrs.Layout = layout;
    mesh.setVertexBufferAttrs(vbAttrs);
    
    // indices
    IndexBufferAttrs ibAttrs;
    uint16 indices[] = {
        0, 2, 1,            // topleft -> bottomright -> topright
        0, 3, 2,            // topleft -> bottomleft -> bottomright
    };
    ibAttrs.NumIndices = 6;
    ibAttrs.Type = IndexType::Index16;
    ibAttrs.BufferUsage = Usage::Immutable;
    mesh.setIndexBufferAttrs(ibAttrs);
    
    // primitive grous
    mesh.setNumPrimitiveGroups(1);
    mesh.setPrimitiveGroup(0, PrimitiveGroup(PrimitiveType::Triangles, 0, 6));
    mesh.glSetVertexBuffer(0, this->createVertexBuffer(vertices, sizeof(vertices), mesh.GetVertexBufferAttrs().BufferUsage));
    mesh.glSetIndexBuffer(this->createIndexBuffer(indices, sizeof(indices), mesh.GetIndexBufferAttrs().BufferUsage));
    this->attachInstanceBuffer(mesh);
    this->setupVertexLayout(mesh);
    
    mesh.setState(Resource::State::Valid);
}

//------------------------------------------------------------------------------
void
glMeshFactory::createEmptyMesh(mesh& mesh) {
    
    const MeshSetup& setup = mesh.GetSetup();
    o_assert(setup.NumVertices > 0);
    
    const int32 numVertices = setup.NumVertices;
    const VertexLayout& layout = setup.Layout;
    const int32 vbSize = numVertices * layout.ByteSize();
    
    VertexBufferAttrs vbAttrs;
    vbAttrs.NumVertices = numVertices;
    vbAttrs.Layout = layout;
    vbAttrs.BufferUsage = setup.VertexUsage;
    mesh.setVertexBufferAttrs(vbAttrs);
    
    const int32 numIndices = setup.NumIndices;
    const IndexType::Code indexType = setup.IndicesType;
    const int32 ibSize = numIndices * IndexType::ByteSize(indexType);
    
    IndexBufferAttrs ibAttrs;
    ibAttrs.NumIndices = setup.NumIndices;
    ibAttrs.Type = setup.IndicesType;
    ibAttrs.BufferUsage = setup.IndexUsage;
    mesh.setIndexBufferAttrs(ibAttrs);
    
    const int32 numPrimGroups = setup.NumPrimitiveGroups();
    if (numPrimGroups > 0) {
        mesh.setNumPrimitiveGroups(numPrimGroups);
        for (int32 i = 0; i < numPrimGroups; i++) {
            mesh.setPrimitiveGroup(i, setup.PrimitiveGroup(i));
        }
    }
    
    // if this is a dynamic mesh, we actually create 2 vertex buffers for double-buffered updated
    if (Usage::Dynamic == vbAttrs.BufferUsage) {
        const uint8 numSlots = 2;
        mesh.setNumVertexBufferSlots(numSlots);
        mesh.setNumVAOSlots(numSlots);
        for (uint8 slotIndex = 0; slotIndex < numSlots; slotIndex++) {
            mesh.glSetVertexBuffer(slotIndex, this->createVertexBuffer(nullptr, vbSize, vbAttrs.BufferUsage));
        }
    }
    else {
        // normal static or stream-dynamic mesh, no double-buffering
        mesh.glSetVertexBuffer(0, this->createVertexBuffer(nullptr, vbSize, vbAttrs.BufferUsage));
    }
    if (indexType != IndexType::None) {
        mesh.glSetIndexBuffer(this->createIndexBuffer(nullptr, ibSize, ibAttrs.BufferUsage));
    }
    this->attachInstanceBuffer(mesh);
    this->setupVertexLayout(mesh);
    
    mesh.setState(Resource::State::Valid);
}

} // namespace Render
} // namespace Oryol
