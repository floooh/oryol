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
    int8_t i8 = StringConverter::FromString<int8_t>("23");
    CHECK_EQUAL(i8, 23);
    i8 = StringConverter::FromString<int8_t>(String("23"));
    CHECK_EQUAL(i8, 23);
    i8 = StringConverter::FromString<int8_t>(StringAtom("23"));
    CHECK_EQUAL(i8, 23);

    uint8_t ui8 = StringConverter::FromString<uint8_t>("5");
    CHECK_EQUAL(ui8, 5);
    ui8 = StringConverter::FromString<uint8_t>(String("5"));
    CHECK_EQUAL(ui8, 5);
    ui8 = StringConverter::FromString<uint8_t>(StringAtom("5"));
    CHECK_EQUAL(ui8, 5);

    int16_t i16 = StringConverter::FromString<int16_t>("42");
    CHECK_EQUAL(i16, 42);
    i16 = StringConverter::FromString<int16_t>(String("42"));
    CHECK_EQUAL(i16, 42);
    i16 = StringConverter::FromString<int16_t>(StringAtom("42"));
    CHECK_EQUAL(i16, 42);

    uint16_t ui16 = StringConverter::FromString<uint16_t>("451");
    CHECK_EQUAL(ui16, 451);
    ui16 = StringConverter::FromString<uint16_t>(String("451"));
    CHECK_EQUAL(ui16, 451);
    ui16 = StringConverter::FromString<uint16_t>(StringAtom("451"));
    CHECK_EQUAL(ui16, 451);

    int i32 = StringConverter::FromString<int>("12345");
    CHECK_EQUAL(i32, 12345);
    i32 = StringConverter::FromString<int>(String("12345"));
    CHECK_EQUAL(i32, 12345);
    i32 = StringConverter::FromString<int>(StringAtom("12345"));
    CHECK_EQUAL(i32, 12345);

    uint32_t ui32 = StringConverter::FromString<uint32_t>("54321");
    CHECK_EQUAL(ui32, 54321u);
    ui32 = StringConverter::FromString<uint32_t>(String("54321"));
    CHECK_EQUAL(ui32, 54321u);
    ui32 = StringConverter::FromString<uint32_t>(StringAtom("54321"));
    CHECK_EQUAL(ui32, 54321u);

    float f32 = StringConverter::FromString<float>("3.14");
    CHECK_CLOSE(f32, 3.14f, 0.000001f);
    f32 = StringConverter::FromString<float>(String("3.14"));
    CHECK_CLOSE(f32, 3.14f, 0.000001f);
    f32 = StringConverter::FromString<float>(StringAtom("3.14"));
    CHECK_CLOSE(f32, 3.14f, 0.000001f);

    double f64 = StringConverter::FromString<double>("6.28");
    CHECK_CLOSE(f64, 6.28, 0.0000000001);
    f64 = StringConverter::FromString<double>(String("6.28"));
    CHECK_CLOSE(f64, 6.28, 0.0000000001);
    f64 = StringConverter::FromString<double>(StringAtom("6.28"));
    CHECK_CLOSE(f64, 6.28, 0.0000000001);
}
