//------------------------------------------------------------------------------
//  glPipelineFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "glPipelineFactory.h"
#include "Core/Assertion.h"
#include "Gfx/gl/gl_impl.h"
#include "Gfx/gl/glTypes.h"
#include "Gfx/Resource/resource.h"
#include "Gfx/Core/GfxTypes.h"
#include "Gfx/Core/renderer.h"

namespace Oryol {
namespace _priv {

// map Oryol VertexFormats to GL vertex attributes
struct {
    GLint size;
    GLenum type;
    GLboolean normalized;
} const vertexFormatTable[VertexFormat::NumVertexFormats] = {
    { 1, GL_FLOAT, GL_FALSE },                  // VertexFormat::Float
    { 2, GL_FLOAT, GL_FALSE },                  // VertexFormat::Float2
    { 3, GL_FLOAT, GL_FALSE },                  // VertexFormat::Float3
    { 4, GL_FLOAT, GL_FALSE },                  // VertexFormat::Float4
    { 4, GL_BYTE, GL_FALSE },                   // VertexFormat::Byte4
    { 4, GL_BYTE, GL_TRUE },                    // VertexFormat::Byte4N
    { 4, GL_UNSIGNED_BYTE, GL_FALSE },          // VertexFormat::UByte4
    { 4, GL_UNSIGNED_BYTE, GL_TRUE },           // VertexFormat::UByte4N
    { 2, GL_SHORT, GL_FALSE },                  // VertexFormat::Short2
    { 2, GL_SHORT, GL_TRUE },                   // VertexFormat::Short2N
    { 4, GL_SHORT, GL_FALSE },                  // VertexFormat::Short4
    { 4, GL_SHORT, GL_TRUE },                   // VertexFormat::Short4N
    { 4, GL_UNSIGNED_INT_2_10_10_10_REV, GL_TRUE },     // VertexFormat::UInt10_2N
};

//------------------------------------------------------------------------------
ResourceState::Code
glPipelineFactory::SetupResource(pipeline& pip) {
    o_assert_dbg(this->isValid);

    pipelineFactoryBase::SetupResource(pip);
    o_assert_dbg(pip.shd);
    this->glSetupVertexAttrs(pip);
    pip.glPrimType = glTypes::asGLPrimitiveType(pip.Setup.PrimType);

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
glPipelineFactory::DestroyResource(pipeline& pip) {
    this->pointers.renderer->invalidateMeshState();
    pip.Clear();
}

//------------------------------------------------------------------------------
void
glPipelineFactory::glSetupVertexAttrs(pipeline& pip) {

    // first disable all vertex attrs
    for (int attrIndex = 0; attrIndex < VertexAttr::NumVertexAttrs; attrIndex++) {
        pip.glAttrs[attrIndex] = glVertexAttr();
        pip.glAttrs[attrIndex].index = attrIndex;
    }

    // convert input mesh vertex layout components to glVertexAttrs
    for (int layoutIndex = 0; layoutIndex < GfxConfig::MaxNumInputMeshes; layoutIndex++) {
        const VertexLayout& layout = pip.Setup.Layouts[layoutIndex];
        const int numComps = layout.NumComponents();
        if (numComps > 0) {
            for (int compIndex = 0; compIndex < numComps; compIndex++) {
                const VertexLayout::Component& comp = layout.ComponentAt(compIndex);
                o_assert_dbg(comp.Format < VertexFormat::NumVertexFormats);
                glVertexAttr& glAttr = pip.glAttrs[comp.Attr];
                o_assert_dbg(!glAttr.enabled);
                glAttr.enabled = GL_TRUE;
                glAttr.vbIndex = layoutIndex;
                if (VertexStepFunction::PerVertex == layout.StepFunction) {
                    glAttr.divisor = 0;
                }
                else {
                    glAttr.divisor = layout.StepRate;
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

} // namespace _priv
} // namespace Oryol
