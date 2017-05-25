//------------------------------------------------------------------------------
//  elementBufferTest.cc
//  Test low-level elementBuffer class.
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/Containers/elementBuffer.h"

using namespace Oryol;
using namespace Oryol::_priv;

class _test
{
public:
    // this class requires that the underlying memory is filled with BB
    // for the default, copy and move constructors!
    _test() {
        o_assert(ORYOL_MEMORY_DEBUG_INT == this->canary);
        this->canary = 0xABBAABBA;
        this->value  = 0;
    };
    _test(int val) {
        this->canary = 0xABBAABBA;
        this->value  = val;
    };
    _test(const _test& rhs) {
        o_assert(ORYOL_MEMORY_DEBUG_INT == this->canary);
        o_assert(0xABBAABBA == rhs.canary);
        this->canary = 0xABBAABBA;
        this->value  = rhs.value;
    }
    _test(_test&& rhs) {
        // NOTE: rhs must still be in the constructed state after this!
        o_assert(0xABBAABBA == rhs.canary);
        this->canary = 0xABBAABBA;
        this->value = rhs.value;
        rhs.value  = 0;
    }
    ~_test() {
        o_assert(0xABBAABBA == this->canary);
        this->canary = ORYOL_MEMORY_DEBUG_INT;
        this->value  = ORYOL_MEMORY_DEBUG_INT;
    };
    void operator=(int val) {
        o_assert(0xABBAABBA == this->canary);
        this->value = val;
    };
    void operator=(const _test& rhs) {
        o_assert(0xABBAABBA == this->canary);
        o_assert(0xABBAABBA == rhs.canary);
        this->value = rhs.value;
    };
    void operator=(_test&& rhs) {
        // NOTE: rhs must still be in the constructed state after this!
        o_assert(0xABBAABBA == this->canary);
        o_assert(0xABBAABBA == rhs.canary);
        this->value = rhs.value;
        this->canary = 0xABBAABBA;
    };
    bool operator==(const _test& rhs) const {
        o_assert(0xABBAABBA == this->canary);
        o_assert(0xABBAABBA == rhs.canary);
        return this->value == rhs.value;
    };
    bool operator!=(const _test& rhs) const {
        o_assert(0xABBAABBA == this->canary);
        o_assert(0xABBAABBA == rhs.canary);
        return this->value != rhs.value;
    };
    bool operator<(const _test& rhs) const {
        o_assert(0xABBAABBA == this->canary);
        o_assert(0xABBAABBA == rhs.canary);
        return this->value < rhs.value;
    };
    bool operator>(const _test& rhs) const {
        o_assert(0xABBAABBA == this->canary);
        o_assert(0xABBAABBA == rhs.canary);
        return this->value > rhs.value;
    };
    bool operator<=(const _test& rhs) const {
        o_assert(0xABBAABBA == this->canary);
        o_assert(0xABBAABBA == rhs.canary);
        return this->value <= rhs.value;
    };
    bool operator>=(const _test& rhs) const {
        o_assert(0xABBAABBA == this->canary);
        o_assert(0xABBAABBA == rhs.canary);
        return this->value >= rhs.value;
    };
    
    uint32_t canary;
    int value;
};

//------------------------------------------------------------------------------
bool
TestMemory(const elementBuffer<_test>& buf) {
    // test that front-spare is completely destructed
    if (buf.buf) {
        for (_test* ptr = buf.buf; ptr < &buf.buf[buf.start]; ptr++) {
            if (ptr->canary != ORYOL_MEMORY_DEBUG_INT) {
                return false;
            }
        }
        // test that elements are all constructed
        for (_test* ptr = &buf.buf[buf.start]; ptr < &buf.buf[buf.end]; ptr++) {
            if (ptr->canary != 0xABBAABBA) {
                return false;
            }
        }
        // test that back-spare is completely destructed
        for (_test* ptr = &buf.buf[buf.end]; ptr < buf.buf+buf.cap; ptr++) {
            if (ptr->canary != ORYOL_MEMORY_DEBUG_INT) {
                return false;
            }
        }
    }
    return true;
}

//------------------------------------------------------------------------------
TEST(elementBufferTest) {

    elementBuffer<_test> buf;
    buf.alloc(128, 64);
    CHECK(TestMemory(buf));
    CHECK(buf.capacity() == 128);
    CHECK(buf.size() == 0);
    CHECK(buf.frontSpare() == 64);
    CHECK(buf.backSpare() == 64);
    
    const _test _0(0);
    const _test _1(1);
    const _test _2(2);
    const _test _3(3);
    const _test _4(4);
    const _test _5(5);
    const _test _6(6);
    const _test _7(7);
    const _test _8(8);
    const _test _9(9);
    const _test _10(10);
    const _test _11(11);
    const _test _12(12);
    const _test _13(13);
    const _test _14(14);
    const _test _15(15);
    const _test _16(16);
    const _test _17(17);
    
    // this should invoke the copy-method
    buf.pushBack(_2);
    buf.pushFront(_1);
    CHECK(TestMemory(buf));
    CHECK(buf.size() == 2);
    CHECK(buf.frontSpare() == 63);
    CHECK(buf.backSpare() == 63);
    CHECK(buf[0] == _1);
    CHECK(buf[1] == _2);

    // this should invoke the move-method
    buf.pushBack(_3);
    buf.pushFront(_4);
    CHECK(TestMemory(buf));
    CHECK(buf.size() == 4);
    CHECK(buf.frontSpare() == 62);
    CHECK(buf.backSpare() == 62);
    CHECK(buf[0] == _4);
    CHECK(buf[1] == _1);
    CHECK(buf[2] == _2);
    CHECK(buf[3] == _3);
    
    buf.alloc(256, 64);
    CHECK(TestMemory(buf));
    CHECK(buf.capacity() == 256);
    CHECK(buf.size() == 4);
    CHECK(buf.frontSpare() == 64);
    CHECK(buf.backSpare() == 188);
    CHECK(buf[0] == _4);
    CHECK(buf[1] == _1);
    CHECK(buf[2] == _2);
    CHECK(buf[3] == _3);
    
    buf.emplaceBack(5);
    buf.emplaceFront(6);
    CHECK(TestMemory(buf));
    CHECK(buf.capacity() == 256);
    CHECK(buf.size() == 6);
    CHECK(buf.frontSpare() == 63);
    CHECK(buf.backSpare() == 187);
    CHECK(buf[0] == _6);
    CHECK(buf[1] == _4);
    CHECK(buf[2] == _1);
    CHECK(buf[3] == _2);
    CHECK(buf[4] == _3);
    CHECK(buf[5] == _5);
    
    // copy constructor
    elementBuffer<_test> buf1(buf);
    CHECK(TestMemory(buf1));
    CHECK(buf1.capacity() == 6);
    CHECK(buf1.size() == 6);
    CHECK(buf1.frontSpare() == 0);
    CHECK(buf1.backSpare() == 0);
    CHECK(buf1[0] == _6);
    CHECK(buf1[1] == _4);
    CHECK(buf1[2] == _1);
    CHECK(buf1[3] == _2);
    CHECK(buf1[4] == _3);
    CHECK(buf1[5] == _5);
    
    // move constructor
    elementBuffer<_test> buf2(std::move(buf1));
    CHECK(TestMemory(buf1));
    CHECK(TestMemory(buf2));
    CHECK(buf1.capacity() == 0);
    CHECK(buf1.size() == 0);
    CHECK(buf2.capacity() == 6);
    CHECK(buf2.size() == 6);
    CHECK(buf2.frontSpare() == 0);
    CHECK(buf2.backSpare() == 0);
    CHECK(buf2[0] == _6);
    CHECK(buf2[1] == _4);
    CHECK(buf2[2] == _1);
    CHECK(buf2[3] == _2);
    CHECK(buf2[4] == _3);
    CHECK(buf2[5] == _5);
    
    // assign empty array
    buf2 = buf1;
    CHECK(buf2.capacity() == 0);
    CHECK(buf2.size() == 0);
    CHECK(TestMemory(buf1));
    CHECK(TestMemory(buf2));
    
    // copy assignment
    buf2 = buf;
    CHECK(TestMemory(buf));
    CHECK(TestMemory(buf2));
    CHECK(buf2.size() == 6);
    CHECK(buf2.frontSpare() == 0);
    CHECK(buf2.backSpare() == 0);
    CHECK(buf2[0] == _6);
    CHECK(buf2[1] == _4);
    CHECK(buf2[2] == _1);
    CHECK(buf2[3] == _2);
    CHECK(buf2[4] == _3);
    CHECK(buf2[5] == _5);

    // move assignment
    buf1 = std::move(buf2);
    CHECK(TestMemory(buf2));
    CHECK(TestMemory(buf1));
    CHECK(buf2.capacity() == 0);
    CHECK(buf2.size() == 0);
    CHECK(buf1.capacity() == 6);
    CHECK(buf1.size() == 6);
    CHECK(buf1.frontSpare() == 0);
    CHECK(buf1.backSpare() == 0);
    CHECK(buf1[0] == _6);
    CHECK(buf1[1] == _4);
    CHECK(buf1[2] == _1);
    CHECK(buf1[3] == _2);
    CHECK(buf1[4] == _3);
    CHECK(buf1[5] == _5);
    
    // erase element at front
    buf1.erase(0);
    CHECK(TestMemory(buf1));
    CHECK(buf1.size() == 5);
    CHECK(buf1[0] == _4);
    CHECK(buf1[1] == _1);
    CHECK(buf1[2] == _2);
    CHECK(buf1[3] == _3);
    CHECK(buf1[4] == _5);
    
    // erase element at back
    buf1.erase(4);
    CHECK(TestMemory(buf1));
    CHECK(buf1.size() == 4);
    CHECK(buf1[0] == _4);
    CHECK(buf1[1] == _1);
    CHECK(buf1[2] == _2);
    CHECK(buf1[3] == _3);
    
    // erase element near front
    buf1.erase(1);
    CHECK(TestMemory(buf1));
    CHECK(buf1.size() == 3);
    CHECK(buf1[0] == _4);
    CHECK(buf1[1] == _2);
    CHECK(buf1[2] == _3);
    
    // erase element in the middle
    buf1.erase(1);
    CHECK(TestMemory(buf1));
    CHECK(buf1.size() == 2);
    CHECK(buf1[0] == _4);
    CHECK(buf1[1] == _3);
    
    // and fully erase all
    buf1.erase(0);
    CHECK(TestMemory(buf1));
    CHECK(buf1.size() == 1);
    CHECK(buf1[0] == _3);
    buf1.erase(0);
    CHECK(TestMemory(buf1));
    CHECK(buf1.size() == 0);
    
    // test insertion
    buf1.alloc(16, 0);
    for (int i = 0; i < 8; i++) {
        buf1.pushBack(i);
    }
    CHECK(TestMemory(buf1));
    CHECK(buf1.size() == 8);
    
    // insert at front
    _test _33(33);
    buf1.insert(0, 33);
    CHECK(TestMemory(buf1));
    CHECK(buf1.size() == 9);
    CHECK(buf1[0] == _33);
    CHECK(buf1[1] == _0);
    CHECK(buf1[2] == _1);
    CHECK(buf1[8] == _7);
    
    // insert at end
    _test _34(34);
    buf1.insert(9, 34);
    CHECK(TestMemory(buf1));
    CHECK(buf1.size() == 10);
    CHECK(buf1[0] == _33);
    CHECK(buf1[1] == _0);
    CHECK(buf1[9] == _34);
    CHECK(buf1[8] == _7);
    
    // insert near front
    _test _35(35);
    buf1.insert(2, 35);
    CHECK(TestMemory(buf1));
    CHECK(buf1.size() == 11);
    CHECK(buf1[0] == _33);
    CHECK(buf1[1] == _0);
    CHECK(buf1[2] == _35);
    CHECK(buf1[3] == _1);
    CHECK(buf1[4] == _2);
    CHECK(buf1[9] == _7);
    CHECK(buf1[10] == _34);
    
    // insert near end
    _test _36(36);
    buf1.insert(8, 36);
    CHECK(TestMemory(buf1));
    CHECK(buf1.size() == 12);
    CHECK(buf1[0] == _33);
    CHECK(buf1[1] == _0);
    CHECK(buf1[2] == _35);
    CHECK(buf1[3] == _1);
    CHECK(buf1[4] == _2);
    CHECK(buf1[7] == _5);
    CHECK(buf1[8] == _36);
    CHECK(buf1[9] == _6);
    CHECK(buf1[10] == _7);
    CHECK(buf1[11] == _34);
    
    // allocate a new buffer with 1 free slot at the front
    elementBuffer<_test> buf3;
    buf3.alloc(16, 1);
    buf3.pushBack(_1);
    buf3.pushBack(_2);
    buf3.pushBack(_3);
    buf3.pushBack(_4);
    // first insert at front should use the empty slot
    buf3.insert(0, _5);
    // second insert at front should move content towards back
    buf3.insert(0, _6);
    CHECK(TestMemory(buf3));
    CHECK(buf3.size() == 6);
    CHECK(buf3[0] == _6);
    CHECK(buf3[1] == _5);
    CHECK(buf3[2] == _1);
    CHECK(buf3[3] == _2);
    CHECK(buf3[4] == _3);
    CHECK(buf3[5] == _4);
    
    // and test the same the other way around
    elementBuffer<_test> buf4;
    buf4.alloc(16, 10);
    CHECK(TestMemory(buf4));
    buf4.pushBack(_10);
    buf4.pushBack(_11);
    buf4.pushBack(_12);
    buf4.pushBack(_13);
    buf4.pushBack(_14);
    buf4.pushBack(_15);

    // this insert should move the array towards the front
    buf4.insert(6, 16);
    CHECK(TestMemory(buf4));
    CHECK(buf4.size() == 7);
    CHECK(buf4[0] == _10);
    CHECK(buf4[1] == _11);
    CHECK(buf4[2] == _12);
    CHECK(buf4[3] == _13);
    CHECK(buf4[4] == _14);
    CHECK(buf4[5] == _15);
    CHECK(buf4[6] == _16);
    
    // check normal erase
    // erase from front-area
    buf4.erase(1);
    CHECK(TestMemory(buf4));
    CHECK(buf4.size() == 6);
    CHECK(buf4[0] == _10);
    CHECK(buf4[1] == _12);
    CHECK(buf4[2] == _13);
    CHECK(buf4[3] == _14);
    CHECK(buf4[4] == _15);
    CHECK(buf4[5] == _16);
    
    // erase from back area
    buf4.erase(3);
    CHECK(TestMemory(buf4));
    CHECK(buf4.size() == 5);
    CHECK(buf4[0] == _10);
    CHECK(buf4[1] == _12);
    CHECK(buf4[2] == _13);
    CHECK(buf4[3] == _15);
    CHECK(buf4[4] == _16);

    // erase from front
    buf4.erase(0);
    CHECK(TestMemory(buf4));
    CHECK(buf4.size() == 4);
    CHECK(buf4[0] == _12);
    CHECK(buf4[1] == _13);
    CHECK(buf4[2] == _15);
    CHECK(buf4[3] == _16);
    
    // erase from back
    buf4.erase(3);
    CHECK(TestMemory(buf4));
    CHECK(buf4.size() == 3);
    CHECK(buf4[0] == _12);
    CHECK(buf4[1] == _13);
    CHECK(buf4[2] == _15);
    
    // and erase the rest
    buf4.erase(1);
    CHECK(TestMemory(buf4));
    CHECK(buf4.size() == 2);
    CHECK(buf4[0] == _12);
    CHECK(buf4[1] == _15);
    buf4.erase(1);
    CHECK(TestMemory(buf4));
    CHECK(buf4.size() == 1);
    CHECK(buf4[0] == _12);
    buf4.erase(0);
    CHECK(TestMemory(buf4));
    CHECK(buf4.size() == 0);
    
    // eraseSwap tests
    elementBuffer<_test> buf5;
    buf5.alloc(8, 0);
    for (int i = 0; i < 8; i++) {
        buf5.pushBack(i);
    }
    CHECK(TestMemory(buf5));
    
    // eraseSwap from front
    buf5.eraseSwap(0);
    CHECK(TestMemory(buf5));
    CHECK(buf5.size() == 7);
    CHECK(buf5[0] == _1);
    CHECK(buf5[1] == _2);
    CHECK(buf5[2] == _3);
    CHECK(buf5[3] == _4);
    CHECK(buf5[4] == _5);
    CHECK(buf5[5] == _6);
    CHECK(buf5[6] == _7);
    
    // eraseSwap from end
    buf5.eraseSwap(6);
    CHECK(TestMemory(buf5));
    CHECK(buf5.size() == 6);
    CHECK(buf5[0] == _1);
    CHECK(buf5[1] == _2);
    CHECK(buf5[2] == _3);
    CHECK(buf5[3] == _4);
    CHECK(buf5[4] == _5);
    CHECK(buf5[5] == _6);
    
    // eraseSwap from near front
    buf5.eraseSwap(2);
    CHECK(TestMemory(buf5));
    CHECK(buf5.size() == 5);
    CHECK(buf5[0] == _2);
    CHECK(buf5[1] == _1);
    CHECK(buf5[2] == _4);
    CHECK(buf5[3] == _5);
    CHECK(buf5[4] == _6);
    buf5.pushBack(7);
    CHECK(TestMemory(buf5));
    CHECK(buf5.size() == 6);
    CHECK(buf5[0] == _2);
    CHECK(buf5[1] == _1);
    CHECK(buf5[2] == _4);
    CHECK(buf5[3] == _5);
    CHECK(buf5[4] == _6);
    CHECK(buf5[5] == _7);
    
    // eraseSwap from near back
    buf5.eraseSwap(3);
    CHECK(TestMemory(buf5));
    CHECK(buf5.size() == 5);
    CHECK(buf5[0] == _2);
    CHECK(buf5[1] == _1);
    CHECK(buf5[2] == _4);
    CHECK(buf5[3] == _7);
    CHECK(buf5[4] == _6);
    
    // test eraseSwapBack
    buf5.eraseSwapBack(1);
    CHECK(TestMemory(buf5));
    CHECK(buf5.size() == 4);
    CHECK(buf5[0] == _2);
    CHECK(buf5[1] == _6);
    CHECK(buf5[2] == _4);
    CHECK(buf5[3] == _7);
    buf5.eraseSwapBack(3);
    CHECK(buf5.size() == 3);
    CHECK(buf5[0] == _2);
    CHECK(buf5[1] == _6);
    CHECK(buf5[2] == _4);
    
    // test eraseSwapFront
    buf5.eraseSwapFront(2);
    CHECK(buf5.size() == 2);
    CHECK(buf5[0] == _6);
    CHECK(buf5[1] == _2);
    buf5.eraseSwapFront(0);
    CHECK(buf5.size() == 1);
    CHECK(buf5[0] == _2);
    
    // test popFront/popBack
    buf5.clear();
    buf5.pushBack(1);
    buf5.pushBack(2);
    buf5.pushBack(3);
    buf5.pushBack(4);
    CHECK(buf5.popFront() == 1);
    CHECK(buf5.size() == 3);
    CHECK(buf5[0] == 2);
    CHECK(buf5[1] == 3);
    CHECK(buf5[2] == 4);
    CHECK(buf5.popBack() == 4);
    CHECK(buf5.size() == 2);
    CHECK(buf5[0] == 2);
    CHECK(buf5[1] == 3);
    CHECK(buf5.popFront() == 2);
    CHECK(buf5.size() == 1);
    CHECK(buf5[0] == 3);
    CHECK(buf5.popBack() == 3);
    CHECK(buf5.size() == 0);

    // eraseRange
    elementBuffer<_test> buf6;
    buf6.alloc(64, 0);
    for (int i = 0; i < 16; i++) {
        buf6.pushBack(i);
    }
    CHECK(buf6.size() == 16);
    buf6.eraseRange(0, 3);
    CHECK(buf6.size() == 13);
    CHECK(buf6[0] == 3);
    CHECK(buf6[1] == 4);
    CHECK(TestMemory(buf6));
    buf6.eraseRange(2, 3);
    CHECK(buf6.size() == 10);
    CHECK(buf6[0] == 3);
    CHECK(buf6[1] == 4);
    CHECK(buf6[2] == 8);
    CHECK(buf6[3] == 9);
    CHECK(TestMemory(buf6));
    buf6.eraseRange(7, 3);
    CHECK(buf6.size() == 7);
    CHECK(buf6[0] == 3);
    CHECK(buf6[1] == 4);
    CHECK(buf6[2] == 8);
    CHECK(buf6[3] == 9);
    CHECK(buf6[6] == 12);
    CHECK(TestMemory(buf6));
}
