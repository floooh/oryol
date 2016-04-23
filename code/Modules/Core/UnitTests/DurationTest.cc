//------------------------------------------------------------------------------
//  DurationTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/Time/Duration.h"

using namespace Oryol;

TEST(DurationTest) {
    Duration d0;
    CHECK(d0.getRaw() == 0);
    CHECK(d0.AsSeconds() == 0.0);
    CHECK(d0.AsMilliSeconds() == 0.0);
    CHECK(d0.AsMicroSeconds() == 0.0);
    CHECK(d0.AsTicks() == 0);
    Duration d1(d0);
    CHECK(d0 == d1);
    CHECK(!(d0 != d1));
    
    Duration d2(1000);
    Duration d3(2000);
    CHECK((d2 + d3).getRaw() == 3000);
    CHECK((d3 - d2).getRaw() == 1000);
    CHECK((d2 * 2.5).getRaw() == 2500);
    Duration d4;
    d4 = d3;
    CHECK(d4 == d3);
    d4 += d2;
    CHECK(d4 == 3000);
    d4 -= d2;
    CHECK(d4 == 2000);
    d4 *= 2.0;
    CHECK(d4 == 4000);
    CHECK(d2 < d4);
    CHECK(d4 > d2);
    CHECK(d2 <= d4);
    CHECK(d4 >= d2);
    d4 = d3;
    CHECK(d4 >= d3);
    CHECK(d4 <= d3);
    
    CHECK_CLOSE(d4.AsSeconds(), 0.002, 0.00000001);
    CHECK_CLOSE(d4.AsMilliSeconds(), 2.0, 0.00000001);
    CHECK_CLOSE(d4.AsMicroSeconds(), 2000.0, 0.00000001);
    CHECK(d4.AsTicks() == 2000);
    
    CHECK_CLOSE(Duration::FromSeconds(1.0).AsSeconds(), 1.0, 0.0000001);
    CHECK_CLOSE(Duration::FromMilliSeconds(1.0).AsMilliSeconds(), 1.0, 0.0000001);
    CHECK_CLOSE(Duration::FromMicroSeconds(1.0).AsMicroSeconds(), 1.0, 0.0000001);

    CHECK_CLOSE(Duration::FromSeconds(1.0).AsMilliSeconds(), 1000.0, 0.0000001);
    CHECK_CLOSE(Duration::FromSeconds(1.0).AsMicroSeconds(), 1000000.0, 0.0000001);
}

