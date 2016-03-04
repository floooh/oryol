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
#include "Gfx/Resource/shader.h"

namespace Oryol {
namespace _priv {

const glDrawStateFactory::VertexFormatTable glDrawStateFactory::vertexFormatTable[VertexFormat::NumVertexFormats] = {
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
    o_assert_dbg(ds.shd);
    // FIXME: remove selection mask
    ds.shdProgIndex = ds.shd->getProgIndexByMask(0);
    o_assert_dbg(InvalidIndex != ds.shdProgIndex);
    this->glSetupVertexAttrs(ds);

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
glDrawStateFactory::DestroyResource(drawState& ds) {
    this->pointers.renderer->invalidateMeshState();
    ds.Clear();
}

//------------------------------------------------------------------------------
void
glDrawStateFactory::glSetupVertexAttrs(drawState& ds) {

    // first disable all vertex attrs
    for (int attrIndex = 0; attrIndex < VertexAttr::NumVertexAttrs; attrIndex++) {
        ds.glAttrs[attrIndex] = glVertexAttr();
        ds.glAttrs[attrIndex].index = attrIndex;
    }

    // convert input mesh vertex layout components to glVertexAttrs
    for (int layoutIndex = 0; layoutIndex < GfxConfig::MaxNumInputMeshes; layoutIndex++) {
        const VertexLayout& layout = ds.Setup.Layouts[layoutIndex];

        const int numComps = layout.NumComponents();
        if (numComps > 0) {
            o_assert_dbg(VertexStepFunction::PerVertex==layout.StepFunction ? 0==layout.StepRate:1<=layout.StepRate);
            for (int compIndex = 0; compIndex < numComps; compIndex++) {
                const VertexLayout::Component& comp = layout.ComponentAt(compIndex);
                o_assert_dbg(comp.Format < VertexFormat::NumVertexFormats);
                glVertexAttr& glAttr = ds.glAttrs[comp.Attr];
                o_assert_dbg(!glAttr.enabled);
                glAttr.enabled = GL_TRUE;
                glAttr.vbIndex = layoutIndex;
                glAttr.divisor = layout.StepRate;
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
