//------------------------------------------------------------------------------
//  PoolAllocatorTest.cc
//  Test pool allocator functionality and performance.
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/Log.h"
#include "Core/Types.h"
#include "Core/RefCounted.h"
#include "Core/Ptr.h"
#include "Core/Memory/poolAllocator.h"

using namespace Oryol::Core;

TEST(PoolAllocator) {

    poolAllocator<RefCounted> allocatorOne;
    
    // allocating and releasing the same object twice should return the same pointer
    RefCounted* obj = allocatorOne.Create();
    CHECK(0 != obj);
    CHECK(obj->GetRefCount() == 0);
    allocatorOne.Destroy(obj);
    
    RefCounted* obj1 = allocatorOne.Create();
    CHECK(0 != obj1);
    CHECK(obj == obj1);
    allocatorOne.Destroy(obj1);
}
