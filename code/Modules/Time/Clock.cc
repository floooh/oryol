//------------------------------------------------------------------------------
//  Clock.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Clock.h"
#if ORYOL_EMSCRIPTEN
#include <emscripten/emscripten.h>
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
    #else
    using namespace std;
    chrono::time_point<chrono::high_resolution_clock, chrono::nanoseconds> now = chrono::high_resolution_clock::now();
    int64 t = now.time_since_epoch().count();
    #endif
    return TimePoint(t);
}

} // namespace Clock
} // namespace Oryol