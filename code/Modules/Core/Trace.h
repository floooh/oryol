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
#if ORYOL_LINUX || ORYOL_MACOS || ORYOL_WINDOWS
#define ORYOL_USE_REMOTERY (1)
#endif
#if ORYOL_EMSCRIPTEN
#define ORYOL_USE_EMSCTRACE (1)
#endif

#if ORYOL_USE_REMOTERY
#include "Remotery.h"
#endif
#if ORYOL_USE_EMSCTRACE
#include "emscripten/trace.h"
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

#if ORYOL_USE_EMSCTRACE
struct emscScopedTrace {
    emscScopedTrace(const char* name) {
        emscripten_trace_enter_context(name);
    };
    ~emscScopedTrace() {
        emscripten_trace_exit_context();
    };
};
#endif
    
// trace macros
#if ORYOL_USE_REMOTERY
#define o_trace_begin_frame() ((void)0)
#define o_trace_end_frame() ((void)0)
#define o_trace_begin(name) rmt_BeginCPUSample(name)
#define o_trace_end() rmt_EndCPUSample()
#define o_trace_scoped(name) rmt_ScopedCPUSample(name)
#elif ORYOL_USE_EMSCTRACE
#define o_trace_begin_frame() emscripten_trace_record_frame_start()
#define o_trace_end_frame() emscripten_trace_record_frame_end()
#define o_trace_begin(name) emscripten_trace_enter_context(#name)
#define o_trace_end(name) emscripten_trace_exit_context()
#define o_trace_scoped(name) emscScopedTrace emscScopedTrace##name(#name)
#else
#define o_trace_begin_frame() ((void)0)
#define o_trace_end_frame() ((void)0)
#define o_trace_begin(name) ((void)0)
#define o_trace_end() ((void)0)
#define o_trace_scoped(name) ((void)0)
#endif

} // namespace Oryol
#else
#define o_trace_begin_frame() ((void)0)
#define o_trace_end_frame() ((void)0)
#define o_trace_begin(name) ((void)0)
#define o_trace_end() ((void)0)
#define o_trace_scoped(name) ((void)0)
#endif
