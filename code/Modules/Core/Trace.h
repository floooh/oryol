#pragma once
#if ORYOL_PROFILING
//------------------------------------------------------------------------------
/**
    @class Oryol::Trace
    @brief tracing support when ORYOL_PROFILING is enabled

    This file implements various macros that hook Oryol into
    profiling/tracing tools.
 */
#include "Core/Types.h"
#if ORYOL_LINUX || ORYOL_OSX || ORYOL_WINDOWS
#define ORYOL_USE_REMOTERY (1)
#endif

#if ORYOL_USE_REMOTERY
#include "Remotery.h"
#endif

namespace Oryol {

class Trace {
public:
    /// constructor (setup tracing system)
    Trace();
    /// destructor (discard tracing system)
    ~Trace();

private:
    #if ORYOL_USE_REMOTERY
    Remotery* rmt;
    #endif
};
    
// trace macros
#if ORYOL_USE_REMOTERY
#define o_trace_begin(name) rmt_BeginCPUSample(name)
#define o_trace_end() rmt_EndCPUSample()
#define o_trace_scoped(name) rmt_ScopedCPUSample(name)
#endif

} // namespace Oryol
#else
#define o_trace_begin(name) ((void)0)
#define o_trace_end() ((void)0)
#define o_trace_scoped(name) ((void)0)
#endif
