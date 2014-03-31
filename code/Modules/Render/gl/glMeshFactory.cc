//------------------------------------------------------------------------------
//  glMeshFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Render/base/meshLoaderBase.h"
#include "glMeshFactory.h"
#include "Render/gl/gl_impl.h"
#include "Render/Core/stateWrapper.h"
#include "Render/gl/glTypes.h"
#include "Render/gl/glExt.h"
#include "Resource/State.h"

namespace Oryol {
namespace Render {

//------------------------------------------------------------------------------
glMeshFactory::glMeshFactory() :
glStateWrapper(nullptr),
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
glMeshFactory::~glMeshFactory() {
    o_assert(!this->isValid);
}

//------------------------------------------------------------------------------
void
glMeshFactory::Setup(stateWrapper* stWrapper) {
    o_assert(!this->isValid);
    o_assert(nullptr != stWrapper);
    this->isValid = true;
    this->glStateWrapper = stWrapper;
}

//------------------------------------------------------------------------------
void
glMeshFactory::Discard() {
    o_assert(this->isValid);
    this->isValid = false;
    this->glStateWrapper = nullptr;
}

//------------------------------------------------------------------------------
bool
glMeshFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
void
glMeshFactory::DestroyResource(mesh& mesh) {
    o_assert(nullptr != this->glStateWrapper);
    this->glStateWrapper->InvalidateMeshState();
    
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
    o_assert(nullptr != this->glStateWrapper);
    o_assert(0 == outMesh.glGetVertexBuffer());
    o_assert(vertexDataSize > 0);
    
    this->glStateWrapper->InvalidateMeshState();
    GLuint vb = 0;
    ::glGenBuffers(1, &vb);
    ORYOL_GL_CHECK_ERROR();
    o_assert(0 != vb);
    GLenum glUsage = glTypes::AsGLUsage(outMesh.GetVertexBufferAttrs().GetUsage());
    this->glStateWrapper->BindVertexBuffer(vb);
    ::glBufferData(GL_ARRAY_BUFFER, vertexDataSize, vertexData, glUsage);
    ORYOL_GL_CHECK_ERROR();
    outMesh.glSetVertexBuffer(vb);
}

//------------------------------------------------------------------------------
/**
 NOTE: this method can be called with a nullptr for indexData, in this case
 an empty GL buffer object will be created with the requested size.
*/
void
glMeshFactory::createIndexBuffer(const void* indexData, uint32 indexDataSize, mesh& outMesh) {
    o_assert(outMesh.GetState() != Resource::State::Valid);
    o_assert(nullptr != this->glStateWrapper);
    o_assert(0 == outMesh.glGetIndexBuffer());
    o_assert(indexDataSize > 0);
    
    this->glStateWrapper->InvalidateMeshState();
    GLuint ib = 0;
    ::glGenBuffers(1, &ib);
    ORYOL_GL_CHECK_ERROR();
    o_assert(0 != ib);
    GLenum glUsage = glTypes::AsGLUsage(outMesh.GetVertexBufferAttrs().GetUsage());
    this->glStateWrapper->BindIndexBuffer(ib);
    ::glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSize, indexData, glUsage);
    ORYOL_GL_CHECK_ERROR();
    outMesh.glSetIndexBuffer(ib);
}

//------------------------------------------------------------------------------
void
glMeshFactory::createVertexLayout(mesh& outMesh) {
    o_assert(outMesh.GetState() != Resource::State::Valid);
    o_assert(nullptr != this->glStateWrapper);
    o_assert(0 != outMesh.glGetVertexBuffer());
    o_assert(0 == outMesh.glGetVertexArrayObject());
    
    // first convert the vertex layout to GL vertex attributes
    this->glSetupVertexAttrs(outMesh);
    
    // create and initialize vertex array object
    this->glStateWrapper->InvalidateMeshState();
    GLuint vao = 0;
    glExt::GenVertexArrays(1, &vao);
    this->glStateWrapper->BindVertexArrayObject(vao);
    this->glStateWrapper->BindIndexBuffer(outMesh.glGetIndexBuffer());
    this->glStateWrapper->BindVertexBuffer(outMesh.glGetVertexBuffer());
    
    for (int32 attrIndex = 0; attrIndex < VertexAttr::NumVertexAttrs; attrIndex++) {
        const glVertexAttr& glAttr = outMesh.glAttr(attrIndex);
        if (glAttr.enabled) {
            ::glVertexAttribPointer(glAttr.index,
                                    glAttr.size,
                                    glAttr.type,
                                    glAttr.normalized,
                                    glAttr.stride,
                                    (const GLvoid*) (GLintptr) glAttr.offset);
            ::glEnableVertexAttribArray(glAttr.index);
        }
        else {
            ::glDisableVertexAttribArray(glAttr.index);
        }
    }
    ORYOL_GL_CHECK_ERROR();
    outMesh.glSetVertexArrayObject(vao);
    this->glStateWrapper->InvalidateMeshState();
}

//------------------------------------------------------------------------------
void
glMeshFactory::glSetupVertexAttrs(mesh& mesh) {

    // first disable all attrs
    for (int32 i = 0; i < VertexAttr::NumVertexAttrs; i++) {
        mesh.glAttr(i).enabled = GL_FALSE;
        mesh.glAttr(i).index = i;
    }
    
    // now go through the vertex layout attributes and setup GL values
    const VertexLayout& layout = mesh.GetVertexBufferAttrs().GetVertexLayout();
    const int32 numComps = layout.GetNumComponents();
    for (int i = 0; i < numComps; i++) {
        const VertexComponent& comp = layout.GetComponent(i);
        glVertexAttr& glAttr = mesh.glAttr(comp.GetAttr());
        glAttr.enabled = GL_TRUE;
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

} // namespace Render
} // namespace Oryol
