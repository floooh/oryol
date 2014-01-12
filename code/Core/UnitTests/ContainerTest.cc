//------------------------------------------------------------------------------
//  ContainerTest.cc
//  Test container classes
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/Containers/elementBuffer.h"

using namespace Oryol;
using namespace Core;

//------------------------------------------------------------------------------
TEST(elementBufferTest) {

    elementBuffer<int> intBuf;
    intBuf.allocate(128, 64);
    CHECK(intBuf.capacity() == 128);
    CHECK(intBuf.size() == 0);
    CHECK(intBuf.frontSpare() == 64);
    CHECK(intBuf.backSpare() == 64);
    
    const int one = 1;
    const int two = 2;
    
    // this should invoke the copy-method
    intBuf.pushBack(two);
    intBuf.pushFront(one);
    CHECK(intBuf.size() == 2);
    CHECK(intBuf.frontSpare() == 63);
    CHECK(intBuf.backSpare() == 63);
    CHECK(intBuf[0] == 1);
    CHECK(intBuf[1] == 2);

    // this should invoke the move-method
    intBuf.pushBack(3);
    intBuf.pushFront(4);
    CHECK(intBuf.size() == 4);
    CHECK(intBuf.frontSpare() == 62);
    CHECK(intBuf.backSpare() == 62);
    CHECK(intBuf[0] == 4);
    CHECK(intBuf[1] == 1);
    CHECK(intBuf[2] == 2);
    CHECK(intBuf[3] == 3);
    
    intBuf.reallocate(256, 64);
    CHECK(intBuf.capacity() == 256);
    CHECK(intBuf.size() == 4);
    CHECK(intBuf.frontSpare() == 64);
    CHECK(intBuf.backSpare() == 188);
    CHECK(intBuf[0] == 4);
    CHECK(intBuf[1] == 1);
    CHECK(intBuf[2] == 2);
    CHECK(intBuf[3] == 3);
    
    intBuf.emplaceBack(5);
    intBuf.emplaceFront(6);
    CHECK(intBuf.capacity() == 256);
    CHECK(intBuf.size() == 6);
    CHECK(intBuf.frontSpare() == 63);
    CHECK(intBuf.backSpare() == 187);
    CHECK(intBuf[0] == 6);
    CHECK(intBuf[1] == 4);
    CHECK(intBuf[2] == 1);
    CHECK(intBuf[3] == 2);
    CHECK(intBuf[4] == 3);
    CHECK(intBuf[5] == 5);
    
    // copy constructor
    elementBuffer<int> intBuf1(intBuf);
    CHECK(intBuf1.capacity() == 256);
    CHECK(intBuf1.size() == 6);
    CHECK(intBuf1.frontSpare() == 63);
    CHECK(intBuf1.backSpare() == 187);
    CHECK(intBuf1[0] == 6);
    CHECK(intBuf1[1] == 4);
    CHECK(intBuf1[2] == 1);
    CHECK(intBuf1[3] == 2);
    CHECK(intBuf1[4] == 3);
    CHECK(intBuf1[5] == 5);
    
    // move constructor
    elementBuffer<int> intBuf2(std::move(intBuf1));
    CHECK(intBuf1.capacity() == 0);
    CHECK(intBuf1.size() == 0);
    CHECK(intBuf2.capacity() == 256);
    CHECK(intBuf2.size() == 6);
    CHECK(intBuf2.frontSpare() == 63);
    CHECK(intBuf2.backSpare() == 187);
    CHECK(intBuf2[0] == 6);
    CHECK(intBuf2[1] == 4);
    CHECK(intBuf2[2] == 1);
    CHECK(intBuf2[3] == 2);
    CHECK(intBuf2[4] == 3);
    CHECK(intBuf2[5] == 5);
    
    // copy assignment
    intBuf2 = intBuf;
    CHECK(intBuf2.size() == 6);
    CHECK(intBuf2.frontSpare() == 63);
    CHECK(intBuf2.backSpare() == 187);
    CHECK(intBuf2[0] == 6);
    CHECK(intBuf2[1] == 4);
    CHECK(intBuf2[2] == 1);
    CHECK(intBuf2[3] == 2);
    CHECK(intBuf2[4] == 3);
    CHECK(intBuf2[5] == 5);

    // move assignment
    intBuf1 = std::move(intBuf2);
    CHECK(intBuf2.capacity() == 0);
    CHECK(intBuf2.size() == 0);
    CHECK(intBuf1.capacity() == 256);
    CHECK(intBuf1.size() == 6);
    CHECK(intBuf1.frontSpare() == 63);
    CHECK(intBuf1.backSpare() == 187);
    CHECK(intBuf1[0] == 6);
    CHECK(intBuf1[1] == 4);
    CHECK(intBuf1[2] == 1);
    CHECK(intBuf1[3] == 2);
    CHECK(intBuf1[4] == 3);
    CHECK(intBuf1[5] == 5);
}
