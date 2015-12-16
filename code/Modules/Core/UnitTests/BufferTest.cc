//------------------------------------------------------------------------------
//  BufferTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/Containers/Buffer.h"

using namespace Oryol;

TEST(BufferTest) {

    Buffer buf0;
    CHECK(buf0.Size() == 0);
    CHECK(buf0.Empty());
    CHECK(buf0.Capacity() == 0);
    CHECK(buf0.Spare() == 0);

    buf0.Reserve(12);
    CHECK(buf0.Size() == 0);
    CHECK(buf0.Empty());
    CHECK(buf0.Capacity() == 12);
    CHECK(buf0.Spare() == 12);

    static const uint8 bla[] = { 1, 2, 3, 4, 5, 6, 7 };
    buf0.Add(bla, sizeof(bla));
    CHECK(buf0.Size() == 7);
    CHECK(!buf0.Empty());
    CHECK(buf0.Capacity() == 12);
    CHECK(buf0.Spare() == 5);
    for (int i = 0; i < int(sizeof(bla)); i++) {
        CHECK(i+1 == buf0.Data()[i]);
    }

    buf0.Add(bla, sizeof(bla));
    CHECK(buf0.Size() == 14);
    CHECK(!buf0.Empty());
    CHECK(buf0.Capacity() == 14);
    CHECK(buf0.Spare() == 0);
    for (int i = 0; i < int(sizeof(bla)); i++) {
        CHECK(i+1 == buf0.Data()[i]);
        CHECK(i+1 == buf0.Data()[i+sizeof(bla)]);
    }

    Buffer buf1(std::move(buf0));
    CHECK(buf1.Size() == 14);
    CHECK(!buf1.Empty());
    CHECK(buf1.Capacity() == 14);
    CHECK(buf1.Spare() == 0);
    for (int i = 0; i < int(sizeof(bla)); i++) {
        CHECK(i+1 == buf1.Data()[i]);
        CHECK(i+1 == buf1.Data()[i+sizeof(bla)]);
    }
    CHECK(buf0.Empty());
    CHECK(buf0.Capacity() == 0);

    Buffer buf2;
    buf2 = std::move(buf1);
    CHECK(buf2.Size() == 14);
    CHECK(!buf2.Empty());
    CHECK(buf2.Capacity() == 14);
    CHECK(buf2.Spare() == 0);
    for (int i = 0; i < int(sizeof(bla)); i++) {
        CHECK(i+1 == buf2.Data()[i]);
        CHECK(i+1 == buf2.Data()[i+sizeof(bla)]);
    }
    CHECK(buf1.Empty());
    CHECK(buf1.Capacity() == 0);
    uint8* ptr = buf2.Add(sizeof(bla));
    for (uint8 c : bla) {
        *ptr++ = c;
    }
    CHECK(ptr == buf2.Data() + 21);
    CHECK(buf2.Size() == 21);
    CHECK(!buf2.Empty());
    CHECK(buf2.Capacity() == 21);
    CHECK(buf2.Spare() == 0);
    for (int i = 0; i < int(sizeof(bla)); i++) {
        CHECK(i+1 == buf2.Data()[i]);
        CHECK(i+1 == buf2.Data()[i+sizeof(bla)]);
        CHECK(i+1 == buf2.Data()[i+2*sizeof(bla)]);
    }
    buf2.Clear();
    CHECK(buf2.Size() == 0);
    CHECK(buf2.Empty());
    CHECK(buf2.Capacity() == 21);
    CHECK(buf2.Spare() == 21);
}
