//------------------------------------------------------------------------------
//  renderMgrBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "renderMgrBase.h"
#include "Render/Core/displayMgr.h"
#include "Render/Core/stateWrapper.h"
#include "Render/Core/drawState.h"
#include "Render/Core/texture.h"

namespace Oryol {
namespace Render {
    
//------------------------------------------------------------------------------
renderMgrBase::renderMgrBase() :
isValid(false),
renderTargetValid(false),
inFrame(false),
displayManager(nullptr),
stateWrapper(nullptr),
curRenderTarget(nullptr),
curMesh(nullptr),
curProgramBundle(nullptr) {
    // empty
}

//------------------------------------------------------------------------------
renderMgrBase::~renderMgrBase() {
    o_assert(!this->isValid);
}

//------------------------------------------------------------------------------
void
renderMgrBase::Setup(class stateWrapper* stWrapper, displayMgr* dispMgr) {
    o_assert(!this->isValid);
    o_assert(stWrapper && dispMgr);
    this->isValid = true;
    this->displayManager = dispMgr;
    this->stateWrapper = stWrapper;
}

//------------------------------------------------------------------------------
void
renderMgrBase::Discard() {
    o_assert(this->isValid);
    this->stateWrapper->InvalidateMeshState();
    this->stateWrapper->InvalidateProgramState();
    this->stateWrapper->InvalidateTextureState();
    this->stateWrapper = nullptr;
    this->displayManager = nullptr;
    this->curRenderTarget = nullptr;
    this->curMesh = nullptr;
    this->curProgramBundle = nullptr;
    this->isValid = false;
}

//------------------------------------------------------------------------------
void
renderMgrBase::BeginFrame() {
    o_assert_dbg(this->isValid);
    o_assert_dbg(!this->inFrame);
    
    this->inFrame = true;
    this->renderTargetValid = false;
}

//------------------------------------------------------------------------------
void
renderMgrBase::EndFrame() {
    o_assert_dbg(this->isValid);
    o_assert_dbg(this->inFrame);
    
    this->inFrame = false;
    this->renderTargetValid = false;
}

//------------------------------------------------------------------------------
void
renderMgrBase::ApplyRenderTarget(texture* rt) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(nullptr != this->displayManager);
    o_assert_dbg(nullptr != this->stateWrapper);
    
    this->curRenderTarget = rt;
    this->renderTargetValid = true;
    
    // also update view port to cover full render target
    int32 width, height;
    if (nullptr == rt) {
        const DisplayAttrs& attrs = this->displayManager->GetDisplayAttrs();
        width = attrs.FramebufferWidth;
        height = attrs.FramebufferHeight;
    }
    else {
        const TextureAttrs& attrs = rt->GetTextureAttrs();
        width = attrs.Width();
        height = attrs.Height();
    }
    this->stateWrapper->ApplyState(State::ViewPort, 0, 0, width, height);
}
    
//------------------------------------------------------------------------------
void
renderMgrBase::ApplyDrawState(drawState* ds) {
    o_assert_dbg(this->isValid);
    
    this->curDrawState = ds;
    this->curProgramBundle = ds->getProgramBundle();
    this->curMesh = ds->getMesh();
}
    
} // namespace Render
} // namespace Render
