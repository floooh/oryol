//------------------------------------------------------------------------------
//  Clock.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Clock.h"
#if ORYOL_EMSCRIPTEN
#include <emscripten/emscripten.h>
#elif ORYOL_WINDOWS
#include <Windows.h>
#include <atomic>
#else
#include <chrono>
#endif

namespace Oryol {
namespace Time {
    
//------------------------------------------------------------------------------
TimePoint
Clock::Now() {
    #if ORYOL_EMSCRIPTEN
    // get int64 time in nanosecs (emscripten_now is ms)
    int64 t = int64(emscripten_get_now() * 1000 * 1000);
    #elif ORYOL_WINDOWS
    // VisualStudio2013's chrono::high_resolution_clock isn't actually high_resolution
    LARGE_INTEGER perfCount;
    static LARGE_INTEGER perfFreq;
    static std::atomic_flag perfFreqValid{ false };
    if (!perfFreqValid.test_and_set())
    {
        QueryPerformanceFrequency(&perfFreq);
    }
    QueryPerformanceCounter(&perfCount);
    int64 t = (perfCount.QuadPart * 1000000000) / perfFreq.QuadPart;
    #else
    using namespace std;
    chrono::time_point<chrono::high_resolution_clock, chrono::nanoseconds> now = chrono::high_resolution_clock::now();
    int64 t = now.time_since_epoch().count();
    #endif
    return TimePoint(t);
}

} // namespace Clock
} // namespace Oryol