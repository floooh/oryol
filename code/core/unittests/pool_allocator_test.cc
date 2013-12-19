//------------------------------------------------------------------------------
//  pool_allocator_test.cc
//  Test pool allocator functionality and performance.
//------------------------------------------------------------------------------
#include "pre.h"
#include "UnitTest++/src/unittest++.h"
#include "core/log.h"
#include "core/types.h"
#include "core/macros.h"
#include "core/refcounted.h"
#include "core/ptr.h"
#include "core/memory/pool_allocator.h"

using namespace oryol;
using namespace core;
using namespace memory;

TEST(pool_allocator) {

    // an allocator with a pool size of one
    pool_allocator<refcounted, 1> allocator_one(false);
    
    // allocating and releasing the same object twice should return the same pointer
    refcounted* obj = allocator_one.create();
    CHECK(0 != obj);
    CHECK(obj->get_refcount() == 0);
    allocator_one.destroy(obj);
    
    refcounted* obj1 = allocator_one.create();
    CHECK(0 != obj1);
    CHECK(obj == obj1);
    allocator_one.destroy(obj1);
    
    // force the allocator to grow
    obj = allocator_one.create();
    obj1 = allocator_one.create();
    CHECK(0 != obj);
    CHECK(0 != obj1);
    CHECK(obj != obj1);
    allocator_one.destroy(obj);
    allocator_one.destroy(obj1);
}