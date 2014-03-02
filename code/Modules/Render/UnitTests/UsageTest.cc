//------------------------------------------------------------------------------
//  UsageTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Render/Usage.h"
#include "Core/String/String.h"

using namespace Oryol::Core;
using namespace Oryol::Render;

TEST(UsageTest) {
    CHECK(Usage::NumUsages == 3);
    CHECK(Usage::FromString("Immutable") == Usage::Immutable);
    CHECK(Usage::FromString("DynamicWrite") == Usage::DynamicWrite);
    CHECK(Usage::FromString("DynamicStream") == Usage::DynamicStream);
    CHECK(Usage::FromString("Bla") == Usage::InvalidUsage);
    CHECK(String(Usage::ToString(Usage::Immutable)) == "Immutable");
    CHECK(String(Usage::ToString(Usage::DynamicWrite)) == "DynamicWrite");
    CHECK(String(Usage::ToString(Usage::DynamicStream)) == "DynamicStream");
}
