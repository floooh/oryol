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
    intBuf.alloc(128, 64);
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
    
    intBuf.alloc(256, 64);
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
    CHECK(intBuf1.capacity() == 6);
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
    
    // erase element at front
    intBuf1.erase(0);
    CHECK(intBuf1.size() == 5);
    CHECK(intBuf1[0] == 4);
    CHECK(intBuf1[1] == 1);
    CHECK(intBuf1[2] == 2);
    CHECK(intBuf1[3] == 3);
    CHECK(intBuf1[4] == 5);
    
    // erase element at back
    intBuf1.erase(4);
    CHECK(intBuf1.size() == 4);
    CHECK(intBuf1[0] == 4);
    CHECK(intBuf1[1] == 1);
    CHECK(intBuf1[2] == 2);
    CHECK(intBuf1[3] == 3);
    
    // erase element near front
    intBuf1.erase(1);
    CHECK(intBuf1.size() == 3);
    CHECK(intBuf1[0] == 4);
    CHECK(intBuf1[1] == 2);
    CHECK(intBuf1[2] == 3);
    
    // erase element in the middle
    intBuf1.erase(1);
    CHECK(intBuf1.size() == 2);
    CHECK(intBuf1[0] == 4);
    CHECK(intBuf1[1] == 3);
    
    // and fully erase all
    intBuf1.erase(0);
    CHECK(intBuf1.size() == 1);
    CHECK(intBuf1[0] == 3);
    intBuf1.erase(0);
    CHECK(intBuf1.size() == 0);
    
    // test insertion
    for (int i = 0; i < 8; i++) {
        intBuf1.pushBack(i);
    }
    CHECK(intBuf1.size() == 8);
    
    // insert at front
    intBuf1.insert(0, 33);
    CHECK(intBuf1.size() == 9);
    CHECK(intBuf1[0] == 33);
    CHECK(intBuf1[1] == 0);
    CHECK(intBuf1[2] == 1);
    CHECK(intBuf1[8] == 7);
    
    // insert at end
    intBuf1.insert(9, 34);
    CHECK(intBuf1.size() == 10);
    CHECK(intBuf1[0] == 33);
    CHECK(intBuf1[1] == 0);
    CHECK(intBuf1[9] == 34);
    CHECK(intBuf1[8] == 7);
    
    // insert near front
    intBuf1.insert(2, 35);
    CHECK(intBuf1.size() == 11);
    CHECK(intBuf1[0] == 33);
    CHECK(intBuf1[1] == 0);
    CHECK(intBuf1[2] == 35);
    CHECK(intBuf1[3] == 1);
    CHECK(intBuf1[4] == 2);
    CHECK(intBuf1[9] == 7);
    CHECK(intBuf1[10] == 34);
    
    // insert near end
    intBuf1.insert(8, 36);
    CHECK(intBuf1.size() == 12);
    CHECK(intBuf1[0] == 33);
    CHECK(intBuf1[1] == 0);
    CHECK(intBuf1[2] == 35);
    CHECK(intBuf1[3] == 1);
    CHECK(intBuf1[4] == 2);
    CHECK(intBuf1[7] == 5);
    CHECK(intBuf1[8] == 36);
    CHECK(intBuf1[9] == 6);
    CHECK(intBuf1[10] == 7);
    CHECK(intBuf1[11] == 34);
    
    // allocate a new buffer with 1 free slot at the front
    elementBuffer<int> intBuf3;
    intBuf3.alloc(16, 1);
    intBuf3.pushBack(1);
    intBuf3.pushBack(2);
    intBuf3.pushBack(3);
    intBuf3.pushBack(4);
    // first insert at front should use the empty slot
    intBuf3.insert(0, 5);
    // second insert at front should move content towards back
    intBuf3.insert(0, 6);
    CHECK(intBuf3.size() == 6);
    CHECK(intBuf3[0] == 6);
    CHECK(intBuf3[1] == 5);
    CHECK(intBuf3[2] == 1);
    CHECK(intBuf3[3] == 2);
    CHECK(intBuf3[4] == 3);
    CHECK(intBuf3[5] == 4);
    
    // and test the same the other way around
    elementBuffer<int> intBuf4;
    intBuf4.alloc(16, 10);
    intBuf4.pushBack(10);
    intBuf4.pushBack(11);
    intBuf4.pushBack(12);
    intBuf4.pushBack(13);
    intBuf4.pushBack(14);
    intBuf4.pushBack(15);

    // this insert should move the array towards the front
    intBuf4.insert(6, 16);
    CHECK(intBuf4.size() == 7);
    CHECK(intBuf4[0] == 10);
    CHECK(intBuf4[1] == 11);
    CHECK(intBuf4[2] == 12);
    CHECK(intBuf4[3] == 13);
    CHECK(intBuf4[4] == 14);
    CHECK(intBuf4[5] == 15);
    CHECK(intBuf4[6] == 16);
    
    // check normal erase
    // erase from front-area
    intBuf4.erase(1);
    CHECK(intBuf4.size() == 6);
    CHECK(intBuf4[0] == 10);
    CHECK(intBuf4[1] == 12);
    CHECK(intBuf4[2] == 13);
    CHECK(intBuf4[3] == 14);
    CHECK(intBuf4[4] == 15);
    CHECK(intBuf4[5] == 16);
    
    // erase from back area
    intBuf4.erase(3);
    CHECK(intBuf4.size() == 5);
    CHECK(intBuf4[0] == 10);
    CHECK(intBuf4[1] == 12);
    CHECK(intBuf4[2] == 13);
    CHECK(intBuf4[3] == 15);
    CHECK(intBuf4[4] == 16);

    // erase from front
    intBuf4.erase(0);
    CHECK(intBuf4.size() == 4);
    CHECK(intBuf4[0] == 12);
    CHECK(intBuf4[1] == 13);
    CHECK(intBuf4[2] == 15);
    CHECK(intBuf4[3] == 16);
    
    // erase from back
    intBuf4.erase(3);
    CHECK(intBuf4.size() == 3);
    CHECK(intBuf4[0] == 12);
    CHECK(intBuf4[1] == 13);
    CHECK(intBuf4[2] == 15);
    
    // and erase the rest
    intBuf4.erase(1);
    CHECK(intBuf4.size() == 2);
    CHECK(intBuf4[0] == 12);
    CHECK(intBuf4[1] == 15);
    intBuf4.erase(1);
    CHECK(intBuf4.size() == 1);
    CHECK(intBuf4[0] == 12);
    intBuf4.erase(0);
    CHECK(intBuf4.size() == 0);
    
    // eraseSwap tests
    elementBuffer<int> intBuf5;
    intBuf5.alloc(8, 0);
    for (int i = 0; i < 8; i++) {
        intBuf5.pushBack(i);
    }
    
    // eraseSwap from front
    intBuf5.eraseSwap(0);
    CHECK(intBuf5.size() == 7);
    CHECK(intBuf5[0] == 1);
    CHECK(intBuf5[1] == 2);
    CHECK(intBuf5[2] == 3);
    CHECK(intBuf5[3] == 4);
    CHECK(intBuf5[4] == 5);
    CHECK(intBuf5[5] == 6);
    CHECK(intBuf5[6] == 7);
    
    // eraseSwap from end
    intBuf5.eraseSwap(6);
    CHECK(intBuf5.size() == 6);
    CHECK(intBuf5[0] == 1);
    CHECK(intBuf5[1] == 2);
    CHECK(intBuf5[2] == 3);
    CHECK(intBuf5[3] == 4);
    CHECK(intBuf5[4] == 5);
    CHECK(intBuf5[5] == 6);
    
    // eraseSwap from near front
    intBuf5.eraseSwap(2);
    CHECK(intBuf5.size() == 5);
    CHECK(intBuf5[0] == 2);
    CHECK(intBuf5[1] == 1);
    CHECK(intBuf5[2] == 4);
    CHECK(intBuf5[3] == 5);
    CHECK(intBuf5[4] == 6);
    intBuf5.pushBack(7);
    CHECK(intBuf5.size() == 6);
    CHECK(intBuf5[0] == 2);
    CHECK(intBuf5[1] == 1);
    CHECK(intBuf5[2] == 4);
    CHECK(intBuf5[3] == 5);
    CHECK(intBuf5[4] == 6);
    CHECK(intBuf5[5] == 7);
    
    // eraseSwap from near back
    intBuf5.eraseSwap(3);
    CHECK(intBuf5.size() == 5);
    CHECK(intBuf5[0] == 2);
    CHECK(intBuf5[1] == 1);
    CHECK(intBuf5[2] == 4);
    CHECK(intBuf5[3] == 7);
    CHECK(intBuf5[4] == 6);
    
    // test if sorting works
    std::sort(intBuf5.elmStart, intBuf5.elmEnd);
    CHECK(intBuf5.size() == 5);
    CHECK(intBuf5[0] == 1);
    CHECK(intBuf5[1] == 2);
    CHECK(intBuf5[2] == 4);
    CHECK(intBuf5[3] == 6);
    CHECK(intBuf5[4] == 7);
}
