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
        o_assert(!setup.GetInstanceMesh().IsValid());
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
    
    GLuint vb = mesh.glGetVertexBuffer();
    if (0 != vb) {
        ::glDeleteBuffers(1, &vb);
    }
    GLuint ib = mesh.glGetIndexBuffer();
    if (0 != ib) {
        ::glDeleteBuffers(1, &ib);
    }
    GLuint vao = mesh.glGetVertexArrayObject();
    if (0 != vao) {
        glExt::DeleteVertexArrays(1, &vao);
    }
    mesh.clear();
    mesh.setState(Resource::State::Setup);
}

//------------------------------------------------------------------------------
/**
 NOTE: this method can be called with a nullptr for vertexData, in this case
 an empty GL buffer object will be created with the requested size.
*/
void
glMeshFactory::createVertexBuffer(const void* vertexData, uint32 vertexDataSize, mesh& outMesh) {
    o_assert(outMesh.GetState() != Resource::State::Valid);
    o_assert(nullptr != this->stateWrapper);
    o_assert(0 == outMesh.glGetVertexBuffer());
    o_assert(vertexDataSize > 0);
    
    this->stateWrapper->InvalidateMeshState();
    GLuint vb = 0;
    ::glGenBuffers(1, &vb);
    ORYOL_GL_CHECK_ERROR();
    o_assert(0 != vb);
    const GLenum glUsage = outMesh.GetVertexBufferAttrs().GetUsage();
    this->stateWrapper->BindVertexBuffer(vb);
    ::glBufferData(GL_ARRAY_BUFFER, vertexDataSize, vertexData, glUsage);
    ORYOL_GL_CHECK_ERROR();
    outMesh.glSetVertexBuffer(vb);
    this->stateWrapper->InvalidateMeshState();
}

//------------------------------------------------------------------------------
/**
 NOTE: this method can be called with a nullptr for indexData, in this case
 an empty GL buffer object will be created with the requested size.
*/
void
glMeshFactory::createIndexBuffer(const void* indexData, uint32 indexDataSize, mesh& outMesh) {
    o_assert(outMesh.GetState() != Resource::State::Valid);
    o_assert(nullptr != this->stateWrapper);
    o_assert(0 == outMesh.glGetIndexBuffer());
    o_assert(indexDataSize > 0);
    
    this->stateWrapper->InvalidateMeshState();
    GLuint ib = 0;
    ::glGenBuffers(1, &ib);
    ORYOL_GL_CHECK_ERROR();
    o_assert(0 != ib);
    const GLenum glUsage = outMesh.GetVertexBufferAttrs().GetUsage();
    this->stateWrapper->BindIndexBuffer(ib);
    ::glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSize, indexData, glUsage);
    ORYOL_GL_CHECK_ERROR();
    outMesh.glSetIndexBuffer(ib);
    this->stateWrapper->InvalidateMeshState();
}

//------------------------------------------------------------------------------
void
glMeshFactory::attachInstanceBuffer(mesh& msh) {
    const Resource::Id& instMeshId = msh.GetSetup().GetInstanceMesh();
    if (instMeshId.IsValid()) {
        o_assert(this->meshPool->QueryState(instMeshId) == Resource::State::Valid);
        const mesh* instMesh = this->meshPool->Lookup(instMeshId);
        msh.glSetInstanceBuffer(instMesh->glGetVertexBuffer());
        
        // don't allow recursive/cascaded instance buffers
        o_assert(0 == instMesh->glGetInstanceBuffer());
        
        // verify that there are no colliding vertex components
        const VertexLayout& mshLayout = msh.GetVertexBufferAttrs().GetVertexLayout();
        const VertexLayout& instLayout = instMesh->GetVertexBufferAttrs().GetVertexLayout();
        for (int32 i = 0; i < mshLayout.GetNumComponents(); i++) {
            o_assert(!instLayout.Contains(mshLayout.GetComponent(i).GetAttr()));
        }
    }
}

//------------------------------------------------------------------------------
void
glMeshFactory::createVertexLayout(mesh& outMesh) {
    o_assert(outMesh.GetState() != Resource::State::Valid);
    o_assert(nullptr != this->stateWrapper);
    o_assert(0 != outMesh.glGetVertexBuffer());
    o_assert(0 == outMesh.glGetVertexArrayObject());
    
    o_assert2(!outMesh.GetSetup().GetInstanceMesh().IsValid(), "FIXME: implement instancing in glMeshFactory::createVertexLayout");
    
    // first convert the vertex layout to GL vertex attributes
    this->glSetupVertexAttrs(outMesh);
    
    // create and initialize vertex array object
    this->stateWrapper->InvalidateMeshState();
    
    // vertex array objects supported?
    GLuint vb = 0;
    if (glExt::HasExtension(glExt::VertexArrayObject)) {
        GLuint vao = 0;
        glExt::GenVertexArrays(1, &vao);
        this->stateWrapper->BindVertexArrayObject(vao);
        this->stateWrapper->BindIndexBuffer(outMesh.glGetIndexBuffer());
        
        for (int32 attrIndex = 0; attrIndex < VertexAttr::NumVertexAttrs; attrIndex++) {
            const glVertexAttr& glAttr = outMesh.glAttr(attrIndex);
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
                ::glEnableVertexAttribArray(glAttr.index);
                ORYOL_GL_CHECK_ERROR();
            }
            else {
                ::glDisableVertexAttribArray(glAttr.index);
                ORYOL_GL_CHECK_ERROR();
            }
        }
        outMesh.glSetVertexArrayObject(vao);
    }
    this->stateWrapper->InvalidateMeshState();
}

//------------------------------------------------------------------------------
void
glMeshFactory::glSetupVertexAttrs(mesh& mesh) {

    o_assert2(!mesh.GetSetup().GetInstanceMesh().IsValid(), "FIXME: implement instancing in glMeshFactory::createVertexLayout");

    // first disable all attrs
    for (int32 i = 0; i < VertexAttr::NumVertexAttrs; i++) {
        glVertexAttr& glAttr = mesh.glAttr(i);
        glAttr.enabled = GL_FALSE;
        glAttr.index = i;
    }
    
    // now go through the vertex layout attributes and setup GL values
    const VertexLayout& layout = mesh.GetVertexBufferAttrs().GetVertexLayout();
    const int32 numComps = layout.GetNumComponents();
    for (int i = 0; i < numComps; i++) {
        const VertexComponent& comp = layout.GetComponent(i);
        glVertexAttr& glAttr = mesh.glAttr(comp.GetAttr());
        glAttr.enabled = GL_TRUE;
        glAttr.vertexBuffer = mesh.glGetVertexBuffer();
        switch (comp.GetFormat()) {
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
        glAttr.stride = layout.GetByteSize();
        glAttr.offset = layout.GetComponentByteOffset(i);
    }
}

//------------------------------------------------------------------------------
void
glMeshFactory::createFullscreenQuad(mesh& mesh) {
    o_assert(!mesh.GetSetup().GetInstanceMesh().IsValid());
    
    // vertices
    float32 vertices[] = {
        -1.0f, +1.0f, 0.0f, 0.0f, 1.0f,     // top-left corner
        +1.0f, +1.0f, 0.0f, 1.0f, 1.0f,     // top-right corner
        +1.0f, -1.0f, 0.0f, 1.0f, 0.0f,     // bottom-right corner
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,     // bottom-left corner
    };
    VertexBufferAttrs vbAttrs;
    vbAttrs.setNumVertices(4);
    vbAttrs.setUsage(Usage::Immutable);
    VertexLayout layout;
    layout.Add(VertexAttr::Position, VertexFormat::Float3);
    layout.Add(VertexAttr::TexCoord0, VertexFormat::Float2);
    vbAttrs.setVertexLayout(layout);
    mesh.setVertexBufferAttrs(vbAttrs);
    
    // indices
    IndexBufferAttrs ibAttrs;
    uint16 indices[] = {
        0, 2, 1,            // topleft -> bottomright -> topright
        0, 3, 2,            // topleft -> bottomleft -> bottomright
    };
    ibAttrs.setNumIndices(6);
    ibAttrs.setIndexType(IndexType::Index16);
    ibAttrs.setUsage(Usage::Immutable);
    mesh.setIndexBufferAttrs(ibAttrs);
    
    // primitive grous
    mesh.setNumPrimitiveGroups(1);
    mesh.setPrimitiveGroup(0, PrimitiveGroup(PrimitiveType::Triangles, 0, 6));
    
    this->createVertexBuffer(vertices, sizeof(vertices), mesh);
    this->createIndexBuffer(indices, sizeof(indices), mesh);
    this->attachInstanceBuffer(mesh);
    this->createVertexLayout(mesh);
    
    mesh.setState(Resource::State::Valid);
}

//------------------------------------------------------------------------------
void
glMeshFactory::createEmptyMesh(mesh& mesh) {
    
    const MeshSetup& setup = mesh.GetSetup();
    o_assert(setup.GetNumVertices() > 0);
    
    const int32 numVertices = setup.GetNumVertices();
    const VertexLayout& layout = setup.GetVertexLayout();
    const int32 vbSize = numVertices * layout.GetByteSize();
    
    VertexBufferAttrs vbAttrs;
    vbAttrs.setNumVertices(numVertices);
    vbAttrs.setVertexLayout(layout);
    vbAttrs.setUsage(setup.GetVertexUsage());
    mesh.setVertexBufferAttrs(vbAttrs);
    
    const int32 numIndices = setup.GetNumIndices();
    const IndexType::Code indexType = setup.GetIndexType();
    const int32 ibSize = numIndices * IndexType::ByteSize(indexType);
    
    IndexBufferAttrs ibAttrs;
    ibAttrs.setNumIndices(setup.GetNumIndices());
    ibAttrs.setIndexType(setup.GetIndexType());
    ibAttrs.setUsage(setup.GetIndexUsage());
    mesh.setIndexBufferAttrs(ibAttrs);
    
    const int32 numPrimGroups = setup.GetNumPrimitiveGroups();
    if (numPrimGroups > 0) {
        mesh.setNumPrimitiveGroups(numPrimGroups);
        for (int32 i = 0; i < numPrimGroups; i++) {
            mesh.setPrimitiveGroup(i, setup.GetPrimitiveGroup(i));
        }
    }
    
    this->createVertexBuffer(nullptr, vbSize, mesh);
    if (indexType != IndexType::None) {
        this->createIndexBuffer(nullptr, ibSize, mesh);
    }
    this->attachInstanceBuffer(mesh);
    this->createVertexLayout(mesh);
    
    mesh.setState(Resource::State::Valid);
}

} // namespace Render
} // namespace Oryol
