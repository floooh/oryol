//------------------------------------------------------------------------------
//  mtlResource.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtl_impl.h"
#include "mtlResource.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
mtlMesh::buffer::buffer() :
updateFrameIndex(-1),
numSlots(1),
activeSlot(0) {
    this->mtlBuffers.Fill(nil);
}

//------------------------------------------------------------------------------
mtlMesh::~mtlMesh() {
    #if ORYOL_DEBUG
    for (const auto& buf : this->buffers) {
        for (int i = 0; i < NumSlots; i++) {
            o_assert_dbg(buf.mtlBuffers[i] == nil);
        }
    }
    #endif
}

//------------------------------------------------------------------------------
void
mtlMesh::Clear() {
    for (auto& buf : this->buffers) {
        buf = buffer();
    }
    meshBase::Clear();
}

//------------------------------------------------------------------------------
mtlPipeline::mtlPipeline() :
mtlRenderPipelineState(nil),
mtlDepthStencilState(nil) {
    // empty
}

//------------------------------------------------------------------------------
mtlPipeline::~mtlPipeline() {
    o_assert_dbg(nil == this->mtlRenderPipelineState);
    o_assert_dbg(nil == this->mtlDepthStencilState);
}

//------------------------------------------------------------------------------
void
mtlPipeline::Clear() {
    this->mtlRenderPipelineState = nil;
    this->mtlDepthStencilState = nil;
    pipelineBase::Clear();
}

//------------------------------------------------------------------------------
mtlShader::mtlShader() {
    this->Clear();
}

//------------------------------------------------------------------------------
mtlShader::~mtlShader() {
#if ORYOL_DEBUG
    o_assert_dbg(nil == this->mtlVertexShaderLibrary);
    o_assert_dbg(nil == this->mtlFragmentShaderLibrary);
    o_assert_dbg(nil == this->mtlVertexShader);
    o_assert_dbg(nil == this->mtlFragmentShader);
#endif
}

//------------------------------------------------------------------------------
void
mtlShader::Clear() {
    this->mtlVertexShaderLibrary = nil;
    this->mtlFragmentShaderLibrary = nil;
    this->mtlVertexShader = nil;
    this->mtlFragmentShader = nil;
    this->vsAttrIndices.Fill(InvalidIndex);
    shaderBase::Clear();
}

//------------------------------------------------------------------------------
mtlTexture::mtlTexture() :
updateFrameIndex(-1),
numSlots(1),
activeSlot(0),
mtlSamplerState(nil),
mtlDepthTex(nil) {
    this->mtlTextures.Fill(nil);
}

//------------------------------------------------------------------------------
mtlTexture::~mtlTexture() {
    o_assert_dbg(nil == this->mtlSamplerState);
    o_assert_dbg(nil == this->mtlDepthTex);
    #if ORYOL_DEBUG
    for (int i = 0; i < NumSlots; i++) {
        o_assert_dbg(this->mtlTextures[i] == nil);
    }
    #endif
}

//------------------------------------------------------------------------------
void
mtlTexture::Clear() {
    textureBase::Clear();
    this->updateFrameIndex = -1;
    this->numSlots = 1;
    this->activeSlot = 0;
    this->mtlTextures.Fill(nil);
    this->mtlSamplerState = nil;
    this->mtlDepthTex = nil;
    this->mtlMSAATex = nil;
}

} // namespace _priv
} // namespace Oryol

