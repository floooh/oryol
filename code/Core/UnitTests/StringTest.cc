//------------------------------------------------------------------------------
//  StringTest.cc
//  Test String class.
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/String/String.h"
#include "Core/String/StringAtom.h"

using namespace Oryol;
using namespace Core;

TEST(StringTest) {

    const char* bla = "Bla";
    
    // construction
    String str0;
    CHECK(!str0.IsValid());
    CHECK(str0.Empty());
    CHECK(str0.Length() == 0);
    CHECK(std::strcmp(str0.AsCStr(), "") == 0);
    CHECK(str0 == "");
    CHECK(str0.RefCount() == 0);
    String str1("");
    CHECK(!str1.IsValid());
    CHECK(str1.Empty());
    CHECK(str1.Length() == 0);
    CHECK(std::strcmp(str1.AsCStr(), "") == 0);
    CHECK(str1.RefCount() == 0);
    CHECK(str0 == str1);
    CHECK(!(str0 != str1));
    CHECK(!(str0 < str1));
    CHECK(!(str0 > str1));
    CHECK(str0 <= str1);
    CHECK(str0 >= str1);
    
    String str2(bla);
    CHECK(str2.IsValid());
    CHECK(!str2.Empty());
    CHECK(str2.Length() == 3);
    CHECK(str2.RefCount() == 1);
    CHECK(std::strcmp(str2.AsCStr(), bla) == 0);
    CHECK(str2 == bla);
    CHECK(bla == str2);
    CHECK(str0 != str2);
    CHECK(str2 == str2);
    
    std::string blub("Blub");
    String str3(blub);
    CHECK(str3.IsValid());
    CHECK(!str3.Empty());
    CHECK(str3.Length() == 4);
    CHECK(str3.RefCount() == 1);
    CHECK(std::strcmp(str3.AsCStr(), blub.c_str()) == 0);
    CHECK(str3 == blub);
    CHECK(str3 == "Blub");
    
    StringAtom blob("Blob");
    String str4(blob);
    CHECK(str4.IsValid());
    CHECK(!str4.Empty());
    CHECK(str4.Length() == 4);
    CHECK(str4.RefCount() == 1);
    CHECK(std::strcmp(str4.AsCStr(), blob.AsCStr()) == 0);
    CHECK(str4 == blob);
    CHECK(str4 == "Blob");
    
    // copy-assignment
    str0 = str2;
    CHECK(str0 == "Bla");
    CHECK(str0 == str2);
    CHECK(str0.RefCount() == 2);
    CHECK(str2.RefCount() == 2);
    CHECK(str0.AsCStr() == str2.AsCStr());  // tests for identical pointers!
    str2.Clear();
    CHECK(str0 == "Bla");
    CHECK(str2.Empty());
    CHECK(str0.RefCount() == 1);
    CHECK(str2.RefCount() == 0);
    str0.Clear();
    CHECK(str0.Empty());
    
    // move-assignment
    str2 = std::move(str3);
    CHECK(str2.IsValid());
    CHECK(str2 == "Blub");
    CHECK(!str3.IsValid());
    CHECK(str3.Empty());
    
    // raw string assignment
    str2 = bla;
    CHECK(str2.IsValid());
    CHECK(!str2.Empty());
    CHECK(str2.Length() == 3);
    CHECK(str2.RefCount() == 1);
    CHECK(std::strcmp(str2.AsCStr(), bla) == 0);
    CHECK(str2 == bla);
    CHECK(bla == str2);
    CHECK(str0 != str2);
    CHECK(str2 == str2);

    // std::string assignment
    str3 = blub;
    CHECK(str3.IsValid());
    CHECK(!str3.Empty());
    CHECK(str3.Length() == 4);
    CHECK(str3.RefCount() == 1);
    CHECK(std::strcmp(str3.AsCStr(), blub.c_str()) == 0);
    CHECK(str3 == blub);
    CHECK(str3 == "Blub");

    // StringAtom assignment
    str4 = blob;
    CHECK(str4.IsValid());
    CHECK(!str4.Empty());
    CHECK(str4.Length() == 4);
    CHECK(str4.RefCount() == 1);
    CHECK(std::strcmp(str4.AsCStr(), blob.AsCStr()) == 0);
    CHECK(str4 == blob);
    CHECK(str4 == "Blob");
    
    // getters
    CHECK(str3.AsStdString() == blub);
    CHECK(str4.AsStringAtom() == blob);
}
