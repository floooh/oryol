//------------------------------------------------------------------------------
//  StringBuilderTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/String/StringBuilder.h"

using namespace Oryol;
using namespace Core;

TEST(StringBuilderTest) {
    
    StringBuilder builder;
    CHECK(builder.GetString().Empty());
    CHECK(builder.GetSubString(0, 0).Empty());
    CHECK(builder.Capacity() == 0);
    CHECK(builder.Length() == 0);
    
    builder.Append('x');
    CHECK(builder.Capacity() > 0);
    CHECK(builder.Length() == 1);
    CHECK(builder.GetString() == "x");
    CHECK(builder.GetSubString(0, 1) == "x");
    CHECK(builder.GetSubString(0, 0) == "x");
    
    builder.Append("Bla");
    CHECK(builder.Length() == 4);
    CHECK(builder.GetString() == "xBla");
    CHECK(builder.GetSubString(0, 1) == "x");
    CHECK(builder.GetSubString(1, 0) == "Bla");
    CHECK(builder.GetSubString(1, 2) == "Bl");
    
    const char* blubber = "BlubberZZZ";
    builder.Append(blubber, 7);
    CHECK(builder.Length() == 11);
    CHECK(builder.GetString() == "xBlaBlubber");
    
    const String blob("Blob");
    builder.Append(blob);
    CHECK(builder.Length() == 15);
    CHECK(builder.GetString() == "xBlaBlubberBlob");
    
    const String xxx("xxxBlabYYY");
    builder.Append(xxx, 3, 7);
    CHECK(builder.Length() == 19);
    CHECK(builder.GetString() == "xBlaBlubberBlobBlab");
    
    const String yyy("yyyBlub");
    builder.Append(yyy, 3, 0);
    CHECK(builder.Length() == 23);
    CHECK(builder.GetString() == "xBlaBlubberBlobBlabBlub");
    
    builder.Clear();
    CHECK(builder.Capacity() > 0);
    CHECK(builder.Length() == 0);
    CHECK(builder.GetString().Empty());
    builder.Append({ "Bla", "Blub", "Blob", "Blubber" });
    CHECK(builder.GetString() == "BlaBlubBlobBlubber");
    
    builder.Clear();
    builder.Append(':', { "Bla", "Blub", "Blob", "Blubber" });
    CHECK(builder.GetString() == "Bla:Blub:Blob:Blubber");
    builder.Truncate(3);
    CHECK(builder.Length() == 3);
    CHECK(builder.GetString() == "Bla");
    CHECK('a' == builder.PopBack());
    CHECK(builder.Length() == 2);
    CHECK(builder.GetString() == "Bl");
}

