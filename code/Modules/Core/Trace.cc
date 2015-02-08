//------------------------------------------------------------------------------
//  Trace.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Trace.h"

namespace Oryol {

//------------------------------------------------------------------------------
Trace::Trace() {
    #if ORYOL_USE_REMOTERY
    rmt_CreateGlobalInstance(&this->rmt);
    rmt_SetCurrentThreadName("MainThread");
    #endif
}

//------------------------------------------------------------------------------
Trace::~Trace() {
    #if ORYOL_USE_REMOTERY
    rmt_DestroyGlobalInstance(this->rmt);
    this->rmt = nullptr;
    #endif
}
    
} // namespace Oryol