//------------------------------------------------------------------------------
//  RenderPassSetup.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "RenderPassSetup.h"

namespace Oryol {

//------------------------------------------------------------------------------
RenderPassSetup
RenderPassSetup::FromColorTexture(Id colorTex) {
    RenderPassSetup setup;
    setup.ColorAttachments[0].Texture = colorTex;
    return setup;
}

//------------------------------------------------------------------------------
RenderPassSetup
RenderPassSetup::FromColorAndDepthStencilTextures(Id colorTex, Id depthStencilTex) {
    RenderPassSetup setup;
    setup.ColorAttachments[0].Texture = colorTex;
    setup.DepthStencilAttachment.Texture = depthStencilTex;
    return setup;
}

} // namespace Oryol
