//------------------------------------------------------------------------------
//  StringConverterTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/String/StringConverter.h"

using namespace Oryol;

TEST(StringConverterTest_UTF8) {

    // short string
    WideString src(L"Hällo Wörld");
    String utf8 = StringConverter::WideToUTF8(src);
    WideString dst = StringConverter::UTF8ToWide(utf8);
    CHECK(src == dst);
    CHECK(dst == L"Hällo Wörld");

    // long string to force dynamic memory allocation
    static const wchar_t* longString =
        L"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        L"abcdefghijklmnopqrstuvwxyz"
        L"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        L"abcdefghijklmnopqrstuvwxyz"
        L"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        L"abcdefghijklmnopqrstuvwxyz"
        L"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        L"abcdefghijklmnopqrstuvwxyz"
        L"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        L"abcdefghijklmnopqrstuvwxyz"
        L"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        L"abcdefghijklmnopqrstuvwxyz";
    WideString lsrc(longString);
    String lutf8 = StringConverter::WideToUTF8(lsrc);
    WideString ldst = StringConverter::UTF8ToWide(lutf8);
    CHECK(lsrc == ldst);
    CHECK(ldst == longString);
}

TEST(StringConverterTest_FromString) {

    // conversion to simple types
    int8 i8 = StringConverter::FromString<int8>("23");
    CHECK_EQUAL(i8, 23);
    i8 = StringConverter::FromString<int8>(String("23"));
    CHECK_EQUAL(i8, 23);
    i8 = StringConverter::FromString<int8>(StringAtom("23"));
    CHECK_EQUAL(i8, 23);

    uint8 ui8 = StringConverter::FromString<uint8>("5");
    CHECK_EQUAL(ui8, 5);
    ui8 = StringConverter::FromString<uint8>(String("5"));
    CHECK_EQUAL(ui8, 5);
    ui8 = StringConverter::FromString<uint8>(StringAtom("5"));
    CHECK_EQUAL(ui8, 5);

    int16 i16 = StringConverter::FromString<int16>("42");
    CHECK_EQUAL(i16, 42);
    i16 = StringConverter::FromString<int16>(String("42"));
    CHECK_EQUAL(i16, 42);
    i16 = StringConverter::FromString<int16>(StringAtom("42"));
    CHECK_EQUAL(i16, 42);

    uint16 ui16 = StringConverter::FromString<uint16>("451");
    CHECK_EQUAL(ui16, 451);
    ui16 = StringConverter::FromString<uint16>(String("451"));
    CHECK_EQUAL(ui16, 451);
    ui16 = StringConverter::FromString<uint16>(StringAtom("451"));
    CHECK_EQUAL(ui16, 451);

    int32 i32 = StringConverter::FromString<int32>("12345");
    CHECK_EQUAL(i32, 12345);
    i32 = StringConverter::FromString<int32>(String("12345"));
    CHECK_EQUAL(i32, 12345);
    i32 = StringConverter::FromString<int32>(StringAtom("12345"));
    CHECK_EQUAL(i32, 12345);

    uint32 ui32 = StringConverter::FromString<uint32>("54321");
    CHECK_EQUAL(ui32, 54321);
    ui32 = StringConverter::FromString<uint32>(String("54321"));
    CHECK_EQUAL(ui32, 54321);
    ui32 = StringConverter::FromString<uint32>(StringAtom("54321"));
    CHECK_EQUAL(ui32, 54321);

    float32 f32 = StringConverter::FromString<float32>("3.14");
    CHECK_CLOSE(f32, 3.14f, 0.000001f);
    f32 = StringConverter::FromString<float32>(String("3.14"));
    CHECK_CLOSE(f32, 3.14f, 0.000001f);
    f32 = StringConverter::FromString<float32>(StringAtom("3.14"));
    CHECK_CLOSE(f32, 3.14f, 0.000001f);

    float64 f64 = StringConverter::FromString<float64>("6.28");
    CHECK_CLOSE(f64, 6.28, 0.0000000001);
    f64 = StringConverter::FromString<float64>(String("6.28"));
    CHECK_CLOSE(f64, 6.28, 0.0000000001);
    f64 = StringConverter::FromString<float64>(StringAtom("6.28"));
    CHECK_CLOSE(f64, 6.28, 0.0000000001);
}
