//------------------------------------------------------------------------------
//  ClockTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/Time/Clock.h"
#include "Core/Log.h"
#include <cmath>

using namespace Oryol;

TEST(ClockTest) {
    TimePoint t0 = Clock::Now();
    double a = 0.0, b = 0.0;
    for (int i = 0; i < 1000000; i++) {
        a += std::sin(b) + std::cos(b);
        b += 0.1;
    }
    CHECK(a > 0.0);
    Duration d0 = Clock::Since(t0);
    TimePoint t1 = Clock::Now();
    CHECK(t1 > t0);
    Log::Info("duration (sec): %f\n", d0.AsSeconds());
    Log::Info("duration (ms): %f\n", d0.AsMilliSeconds());
    Log::Info("duration (us): %f\n", d0.AsMicroSeconds());
}