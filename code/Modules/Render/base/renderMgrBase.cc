//------------------------------------------------------------------------------
//  renderMgrBase.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "renderMgrBase.h"

namespace Oryol {
namespace Render {
    
//------------------------------------------------------------------------------
renderMgrBase::renderMgrBase() :
isValid(false),
stateWrapper(nullptr),
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
renderMgrBase::Setup(class stateWrapper* stWrapper) {
    o_assert(!this->isValid);
    o_assert(stWrapper);
    this->isValid = true;
    this->stateWrapper = stWrapper;
}

//------------------------------------------------------------------------------
void
renderMgrBase::Discard() {
    o_assert(this->isValid);
    this->stateWrapper->InvalidateMeshState();
    this->stateWrapper->InvalidateProgramState();
    this->stateWrapper = nullptr;
    this->curMesh = nullptr;
    this->curProgramBundle = nullptr;
    this->isValid = false;
}
    
} // namespace Render
} // namespace Render