//------------------------------------------------------------------------------
//  OpenModeTest.cc
//  Test OpenMode enum.
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "IO/Core/OpenMode.h"
#include "Core/String/String.h"

using namespace Oryol::Core;
using namespace Oryol::IO;

TEST(OpenModeTest) {
    
    CHECK(OpenMode::InvalidOpenMode == 5);  // this will fail if we need to add more modes
    CHECK(String(OpenMode::ToString(OpenMode::ReadOnly)) == "ReadOnly");
    CHECK(String(OpenMode::ToString(OpenMode::WriteOnly)) == "WriteOnly");
    CHECK(String(OpenMode::ToString(OpenMode::WriteAppend)) == "WriteAppend");
    CHECK(String(OpenMode::ToString(OpenMode::ReadWrite)) == "ReadWrite");
    CHECK(String(OpenMode::ToString(OpenMode::ReadWriteAppend)) == "ReadWriteAppend");
    CHECK(String(OpenMode::ToString(OpenMode::InvalidOpenMode)) == "InvalidOpenMode");
    
    CHECK(OpenMode::FromString("ReadOnly") == OpenMode::ReadOnly);
    CHECK(OpenMode::FromString("WriteOnly") == OpenMode::WriteOnly);
    CHECK(OpenMode::FromString("WriteAppend") == OpenMode::WriteAppend);
    CHECK(OpenMode::FromString("ReadWrite") == OpenMode::ReadWrite);
    CHECK(OpenMode::FromString("ReadWriteAppend") == OpenMode::ReadWriteAppend);
    CHECK(OpenMode::FromString("InvalidOpenMode") == OpenMode::InvalidOpenMode);
}
