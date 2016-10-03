//------------------------------------------------------------------------------
//  RenderPassSetup.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "RenderPassSetup.h"

namespace Oryol {

//------------------------------------------------------------------------------
RenderPassSetup
RenderPassSetup::From(std::initializer_list<Id> colorTextures, Id depthStencilTexture) {
    RenderPassSetup setup;
    int i = 0;
    for (const auto& id : colorTextures) {
        setup.ColorAttachments[i++].Texture = id;
    }
    setup.DepthStencilAttachment.Texture = depthStencilTexture;
    return setup;
}

} // namespace Oryol
