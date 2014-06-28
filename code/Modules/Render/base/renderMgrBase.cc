//------------------------------------------------------------------------------
//  renderMgrBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "renderMgrBase.h"
#include "Render/Core/displayMgr.h"
#include "Render/Core/stateWrapper.h"
#include "Render/Core/drawState.h"

namespace Oryol {
namespace Render {
    
//------------------------------------------------------------------------------
renderMgrBase::renderMgrBase() :
isValid(false),
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
renderMgrBase::ApplyRenderTarget(texture* rt) {
    o_assert_dbg(this->isValid);
    this->curRenderTarget = rt;
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
