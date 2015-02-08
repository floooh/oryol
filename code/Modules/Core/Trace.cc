//------------------------------------------------------------------------------
//  Trace.cc
//------------------------------------------------------------------------------
#if ORYOL_PROFILING
#include "Pre.h"
#include "Trace.h"

namespace Oryol {

//------------------------------------------------------------------------------
Trace::Trace() {
    #if ORYOL_USE_REMOTERY
    rmt_CreateGlobalInstance(&this->rmt);
    rmt_SetCurrentThreadName("MainThread");
    #elif ORYOL_USE_EMSCTRACE
    emscripten_trace_configure("http://localhost:5000/", "OryolApp"); 
    #endif
}

//------------------------------------------------------------------------------
Trace::~Trace() {
    #if ORYOL_USE_REMOTERY
    rmt_DestroyGlobalInstance(this->rmt);
    this->rmt = nullptr;
    #elif ORYOL_USE_EMSCTRACE
    emscripten_trace_close();
    #endif
}
    
} // namespace Oryol
#endif
