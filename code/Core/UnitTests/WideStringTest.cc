//------------------------------------------------------------------------------
//  WideStringTest.cc
//  Test WideString classes.
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/String/WideString.h"

using namespace Oryol;
using namespace Core;

TEST(WideStringTest) {
    
    const wchar_t* bla = L"Bla";
    
    // construction
    WideString str0;
    CHECK(!str0.IsValid());
    CHECK(str0.Empty());
    CHECK(str0.Length() == 0);
    CHECK(std::wcscmp(str0.AsCStr(), L"") == 0);
    CHECK(str0 == L"");
    CHECK(str0.RefCount() == 0);
    WideString str1(L"");
    CHECK(!str1.IsValid());
    CHECK(str1.Empty());
    CHECK(str1.Length() == 0);
    CHECK(std::wcscmp(str1.AsCStr(), L"") == 0);
    CHECK(str1.RefCount() == 0);
    CHECK(str0 == str1);
    CHECK(!(str0 != str1));
    CHECK(!(str0 < str1));
    CHECK(!(str0 > str1));
    CHECK(str0 <= str1);
    CHECK(str0 >= str1);
    
    WideString str2(bla);
    CHECK(str2.IsValid());
    CHECK(!str2.Empty());
    CHECK(str2.Length() == 3);
    CHECK(str2.RefCount() == 1);
    CHECK(std::wcscmp(str2.AsCStr(), bla) == 0);
    CHECK(str2 == bla);
    CHECK(bla == str2);
    CHECK(str0 != str2);
    CHECK(str2 == str2);
    
    std::wstring blub(L"Blub");
    WideString str3(blub);
    CHECK(str3.IsValid());
    CHECK(!str3.Empty());
    CHECK(str3.Length() == 4);
    CHECK(str3.RefCount() == 1);
    CHECK(std::wcscmp(str3.AsCStr(), blub.c_str()) == 0);
    CHECK(str3 == blub);
    CHECK(str3 == L"Blub");
    
    // copy-assignment
    str0 = str2;
    CHECK(str0 == L"Bla");
    CHECK(str0 == str2);
    CHECK(str0.RefCount() == 2);
    CHECK(str2.RefCount() == 2);
    CHECK(str0.AsCStr() == str2.AsCStr());  // tests for identical pointers!
    str2.Clear();
    CHECK(str0 == L"Bla");
    CHECK(str2.Empty());
    CHECK(str0.RefCount() == 1);
    CHECK(str2.RefCount() == 0);
    str0.Clear();
    CHECK(str0.Empty());
    
    // move-assignment
    str2 = std::move(str3);
    CHECK(str2.IsValid());
    CHECK(str2 == L"Blub");
    CHECK(!str3.IsValid());
    CHECK(str3.Empty());
    
    // raw string assignment
    str2 = bla;
    CHECK(str2.IsValid());
    CHECK(!str2.Empty());
    CHECK(str2.Length() == 3);
    CHECK(str2.RefCount() == 1);
    CHECK(std::wcscmp(str2.AsCStr(), bla) == 0);
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
    CHECK(std::wcscmp(str3.AsCStr(), blub.c_str()) == 0);
    CHECK(str3 == blub);
    CHECK(str3 == L"Blub");
    
    // getters
    CHECK(str3.AsStdWString() == blub);
}
