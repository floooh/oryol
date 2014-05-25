//------------------------------------------------------------------------------
//  TimePointTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Time/TimePoint.h"
#include "Time/Duration.h"

using namespace Oryol;
using namespace Oryol::Time;

TEST(TimePointTest) {
    TimePoint t0;
    CHECK(t0.getRaw() == 0);
    TimePoint t1;
    CHECK(t0 == t1);
    TimePoint t2(1000);
    CHECK(t2.getRaw() == 1000);
    TimePoint t3(t2);
    CHECK(t3 == t2);
    CHECK(t3 != t0);
    CHECK(t0 < t3);
    CHECK(t3 > t0);
    CHECK(t0 <= t3);
    CHECK(t3 >= t0);
    CHECK(t2 <= t3);
    CHECK(t3 >= t2);
    TimePoint t4(2000);
    Duration d0 = t4 - t3;
    CHECK(d0.getRaw() == 1000);
    CHECK((t3 + d0) == t4);
    CHECK((d0 + t3) == t4);
    CHECK((t4 - d0) == t3);
}