//------------------------------------------------------------------------------
//  StaticArrayTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/Containers/StaticArray.h"

using namespace Oryol;

TEST(StaticArrayTest) {

    StaticArray<int32, 16> array0;
    StaticArray<int32, 16> array1;
    CHECK(array0.Size() == 16);
    CHECK(array1.Size() == 16);
    array0.Fill(1);
    for (int32 v : array0) {
        CHECK(v == 1);
    }
    for (int32 i = 0; i < 16; i++) {
        CHECK(array0[0] == 1);
    }
    for (int32 i = 0; i < 16; i++) {
        array1[i] = i;
    }
    int32 i = 0;
    for (int32 v : array1) {
        CHECK(v == i++);
    }
    array0 = array1;
    for (i = 0; i < 16; i++) {
        CHECK(array0[i] == array1[i]);
        CHECK(array0[i] == i);
    }
    array1.Fill(2);
    for (int32 v : array1) {
        CHECK(v == 2);
    }
    array1 = std::move(array0);
    for (i = 0; i < 16; i++) {
        CHECK(array1[i] == i);
    }
    StaticArray<int32, 16> array2(array0);
    StaticArray<int32, 16> array3(std::move(array0));
    for (i = 0; i < 16; i++) {
        CHECK(array2[i] == i);
        CHECK(array3[i] == i);
    }
}
