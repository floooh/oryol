//------------------------------------------------------------------------------
//  Clock.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Clock.h"
#if ORYOL_EMSCRIPTEN
#include <emscripten/emscripten.h>
#elif ORYOL_WINDOWS
#include <Windows.h>
#else
#include <chrono>
#endif

namespace Oryol {

#if ORYOL_WINDOWS
// query perf-freq before any threads can start
// and capture a start count
struct _perf {
    _perf() {
        ::QueryPerformanceFrequency(&this->freq);
        ::QueryPerformanceCounter(&this->start);
    };
    LARGE_INTEGER freq;
    LARGE_INTEGER start;
} perf;
#endif
    
//------------------------------------------------------------------------------
TimePoint
Clock::Now() {
    #if ORYOL_EMSCRIPTEN
    // get int64 time in microseconds (emscripten_now is ms)
    int64_t t = int64_t(emscripten_get_now() * 1000);
    #elif ORYOL_WINDOWS
    // VisualStudio2013's chrono::high_resolution_clock isn't actually high_resolution
    // VisualStudio2015: in VS2015-final this is fixed (might want to use std::chrono if 
    // VS version is >= VS2015?
    LARGE_INTEGER perfCount;
    QueryPerformanceCounter(&perfCount);
    int64_t t = ((perfCount.QuadPart - perf.start.QuadPart) * 1000000) / perf.freq.QuadPart;
    #else
    using namespace std;
    auto now = chrono::high_resolution_clock::now();
    int64_t t = chrono::duration_cast<chrono::microseconds>(now.time_since_epoch()).count();
    #endif
    return TimePoint(t);
}

} // namespace Oryol