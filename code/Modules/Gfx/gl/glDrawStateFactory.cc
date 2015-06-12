//------------------------------------------------------------------------------
//  glDrawStateFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glDrawStateFactory.h"
#include "Core/Assertion.h"
#include "Gfx/gl/gl_impl.h"
#include "Gfx/gl/glExt.h"
#include "Gfx/Resource/drawState.h"
#include "Gfx/Resource/mesh.h"
#include "Gfx/Core/VertexLayout.h"
#include "Gfx/Core/renderer.h"

namespace Oryol {
namespace _priv {

const glDrawStateFactory::VertexFormatTable glDrawStateFactory::vertexFormatTable[VertexAttr::NumVertexAttrs] = {
    { 1, GL_FLOAT, GL_FALSE },          // VetrexFormat::Float
    { 2, GL_FLOAT, GL_FALSE },          // VertexFormat::Float2
    { 3, GL_FLOAT, GL_FALSE },          // VertexFormat::Float3
    { 4, GL_FLOAT, GL_FALSE },          // VertexFormat::Float4
    { 4, GL_BYTE, GL_FALSE },           // VertexFormat::Byte4
    { 4, GL_BYTE, GL_TRUE },            // VertexFormat::Byte4N
    { 4, GL_UNSIGNED_BYTE, GL_FALSE },  // VertexFormat::UByte4
    { 4, GL_UNSIGNED_BYTE, GL_TRUE },   // VertexFormat::UByte4N
    { 2, GL_SHORT, GL_FALSE },          // VertexFormat::Short2
    { 2, GL_SHORT, GL_TRUE },           // VertexFormat::Short2N
    { 4, GL_SHORT, GL_FALSE },          // VertexFormat::Short4
    { 4, GL_SHORT, GL_TRUE },           // VertexFormat::Short4N
};

//------------------------------------------------------------------------------
ResourceState::Code
glDrawStateFactory::SetupResource(drawState& ds) {
    o_assert_dbg(this->isValid);

    drawStateFactoryBase::SetupResource(ds);
    this->glSetupVertexAttrs(ds);
    #if ORYOL_GL_USE_VERTEXARRAYOBJECT
    if (glExt::HasExtension(glExt::VertexArrayObject)) {
        this->glSetupVertexArrayObject(ds);
    }
    #endif


    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
glDrawStateFactory::DestroyResource(drawState& ds) {
    o_assert_dbg(this->renderer);

    this->renderer->invalidateMeshState();

    #if ORYOL_GL_USE_VERTEXARRAYOBJECT
    if (glExt::HasExtension(glExt::VertexArrayObject)) {
        if (0 != ds.glVAO) {
            glExt::DeleteVertexArrays(1, &ds.glVAO);
        }
    }
    #endif

    ds.Clear();
}

//------------------------------------------------------------------------------
void
glDrawStateFactory::glSetupVertexAttrs(drawState& ds) {
    o_assert_dbg(this->renderer);

    // first disable all vertex attrs
    for (int attrIndex = 0; attrIndex < VertexAttr::NumVertexAttrs; attrIndex++) {
        ds.glAttrs[attrIndex] = glVertexAttr();
        ds.glAttrs[attrIndex].index = attrIndex;
    }

    // convert input mesh vertex layout components to glVertexAttrs
    for (int mshIndex = 0; mshIndex < DrawStateSetup::MaxInputMeshes; mshIndex++) {
        const mesh* msh = ds.meshes[mshIndex];
        if (msh) {
            const VertexLayout& layout = msh->vertexBufferAttrs.Layout;
            const int numComps = layout.NumComponents();
            for (int compIndex = 0; compIndex < numComps; compIndex++) {
                const VertexLayout::Component& comp = layout.ComponentAt(compIndex);
                o_assert_dbg(comp.Format < VertexFormat::NumVertexFormats);
                glVertexAttr& glAttr = ds.glAttrs[comp.Attr];
                o_assert_dbg(!glAttr.enabled);
                glAttr.enabled = GL_TRUE;
                glAttr.vbIndex = mshIndex;
                glAttr.streaming = (Usage::Stream == msh->vertexBufferAttrs.BufferUsage);
                if (msh->vertexBufferAttrs.StepFunction == VertexStepFunction::PerVertex) {
                    glAttr.divisor = 0;
                }
                else {
                    o_assert2(msh->vertexBufferAttrs.StepRate == 1, "figure out GL vertex divisor for step-rate > 1!\n");
                    glAttr.divisor = msh->vertexBufferAttrs.StepRate;
                }
                glAttr.stride = layout.ByteSize();
                glAttr.offset = layout.ComponentByteOffset(compIndex);
                glAttr.size   = vertexFormatTable[comp.Format].size;
                glAttr.type   = vertexFormatTable[comp.Format].type;
                glAttr.normalized = vertexFormatTable[comp.Format].normalized;
            }
        }
    }
}

//------------------------------------------------------------------------------
#if ORYOL_GL_USE_VERTEXARRAYOBJECT
void
glDrawStateFactory::glSetupVertexArrayObject(drawState& ds) {
    o_assert_dbg(glExt::HasExtension(glExt::VertexArrayObject));
    o_assert_dbg(0 == ds.glVAO);

    // NOTE: if dynamic meshes with double-buffering are involved,
    // vertex attributes need to be re-bound during rendering,
    // since the actually bound vertex buffers will change after
    // each update.
    //
    // This would actually be a good reason to not use VAOs
    // at all since there are known performance issues
    // with VAOs on desktop, but on web platforms, with their
    // high call-overhead into GL, VAOs might still make sense!
    //
    GLuint curVB = 0;
    this->renderer->invalidateMeshState();
    glExt::GenVertexArrays(1, &ds.glVAO);
    this->renderer->bindVertexArrayObject(ds.glVAO);
    this->renderer->bindIndexBuffer(ds.meshes[0]->glIndexBuffer);   // can be 0
    for (const auto& attr : ds.glAttrs) {
        if (attr.enabled) {
            const mesh* msh = ds.meshes[attr.vbIndex];
            const GLuint glVB = msh->glVertexBuffers[msh->activeVertexBufferSlot];
            if (glVB != curVB) {
                curVB = glVB;
                this->renderer->bindVertexBuffer(glVB);
            }
            ::glVertexAttribPointer(attr.index, attr.size, attr.type, attr.normalized, attr.stride, (const GLvoid*) (GLintptr) attr.offset);
            glExt::VertexAttribDivisor(attr.index, attr.divisor);
            ::glEnableVertexAttribArray(attr.index);
        }
        else {
            ::glDisableVertexAttribArray(attr.index);
        }
    }
}
#endif

} // namespace _priv
} // namespace Oryol