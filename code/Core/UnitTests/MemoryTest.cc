//------------------------------------------------------------------------------
//  MemoryTest.cc
//  Test low-level memory functions.
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/Memory/Memory.h"

using namespace std;
using namespace Oryol;

//------------------------------------------------------------------------------
TEST(Memory) {

    // allocate memory
    int32 byteSize = 64;
    uint8* p0 = (uint8*) Memory::Alloc(byteSize);
    CHECK(nullptr != p0);
    
    // clear memory
    int32 i;
    Memory::Clear(p0, byteSize);
    bool check = true;
    for (i = 0; i < byteSize; i++) {
        if (0 != p0[i]) {
            check = false;
        }
    }
    CHECK(check);
    
    // realloc to bigger size
    byteSize = 128;
    p0 = (uint8*) Memory::ReAlloc(p0, byteSize);
    
    // fill with data
    for (i = 0; i < byteSize; i++) {
        p0[i] = i;
    }
    
    // test memory copy
    uint8* p1 = (uint8*) Memory::Alloc(byteSize);
    CHECK(nullptr != p1);
    Memory::Copy(p0, p1, byteSize);
    check = true;
    for (i = 0; i < byteSize; i++) {
        if (p1[i] != i) {
            check = false;
        }
    }
    CHECK(check);
    
    // test memory move (with overlap)
    Memory::Move(p1 + 1, p1, byteSize - 1);
    check = true;
    for (i = 0; i < byteSize - 1; i++) {
        if (p1[i] != (i + 1)) {
            check = false;
        }
    }
    CHECK(check);
    CHECK(p1[byteSize - 1] == byteSize - 1);
    
    // free memory
    Memory::Free(p0);
    Memory::Free(p1);
    
    // test pointer alignment
    void* ptr = (void*)0x1234567;
    ptr = Memory::Align(ptr, 4);
    CHECK((intptr(ptr) & 3) == 0);
    CHECK((intptr(ptr) == 0x1234568));
    
    ptr = (void*) 0x1234567;
    ptr = Memory::Align(ptr, ORYOL_MAX_PLATFORM_ALIGN);
    CHECK((intptr(ptr) & (ORYOL_MAX_PLATFORM_ALIGN - 1)) == 0);
}


