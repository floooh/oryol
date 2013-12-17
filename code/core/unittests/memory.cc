//------------------------------------------------------------------------------
//  memory.cc
//  Test low-level memory functions.
//------------------------------------------------------------------------------
#include "pre.h"
#include "UnitTest++/src/unittest++.h"
#include "core/memory/memory.h"

using namespace std;
using namespace oryol;

//------------------------------------------------------------------------------
/**
*/
TEST(memory) {

    // allocate memory
    int32 byte_size = 64;
    uint8* p0 = (uint8*) memory::alloc(byte_size);
    CHECK(nullptr != p0);
    
    // clear memory
    int32 i;
    memory::clear(p0, byte_size);
    bool check = true;
    for (i = 0; i < byte_size; i++) {
        if (0 != p0[i]) {
            check = false;
        }
    }
    CHECK(check);
    
    // realloc to bigger size
    byte_size = 128;
    p0 = (uint8*) memory::realloc(p0, byte_size);
    
    // fill with data
    for (i = 0; i < byte_size; i++) {
        p0[i] = i;
    }
    
    // test memory copy
    uint8* p1 = (uint8*) memory::alloc(byte_size);
    CHECK(nullptr != p1);
    memory::copy(p0, p1, byte_size);
    check = true;
    for (i = 0; i < byte_size; i++) {
        if (p1[i] != i) {
            check = false;
        }
    }
    CHECK(check);
    
    // test memory move (with overlap)
    memory::move(p1 + 1, p1, byte_size - 1);
    check = true;
    for (i = 0; i < byte_size - 1; i++) {
        if (p1[i] != (i + 1)) {
            check = false;
        }
    }
    CHECK(check);
    CHECK(p1[byte_size - 1] == byte_size - 1);
    
    // free memory
    memory::free(p0);
    memory::free(p1);
}


