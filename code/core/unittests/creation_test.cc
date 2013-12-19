//------------------------------------------------------------------------------
//  creation.cc
//  Test object creation for shared_ptr.
//------------------------------------------------------------------------------
#include "pre.h"
#include "UnitTest++/src/unittest++.h"
#include "core/log.h"
#include "core/types.h"
#include "core/macros.h"
#include "core/refcounted.h"
#include "core/ptr.h"
#include "core/core.h"

#include <array>

using namespace std;
using namespace oryol;
using namespace core;

// define a custom class
class test_class : public refcounted {
    oryol_class_decl(test_class, 128);
public:
    test_class() : val(0) {
        core::log::info("constructor called!\n");
    };
    test_class(int32 v) : val(v) {
        core::log::info("constructor with '%d' called!\n", val);
    }
    virtual ~test_class() {
        core::log::info("destructor called!\n");
    };
    void set(int32 i) { val = i; };
    int32 get() const { return val; };
private:
    int32 val;
};
oryol_class_impl(test_class, 128);

TEST(create_shared) {

    ptr<test_class> ptr0 = test_class::create();
    CHECK(bool(ptr0));
    CHECK(ptr0.isvalid());
    CHECK(ptr0->get_refcount() == 1);
    
    // assign from some pointer type
    ptr<test_class> ptr1 = ptr0;
    CHECK(ptr0 == ptr1);
    CHECK(ptr0 && ptr1);
    CHECK(ptr0->get_refcount() == 2);
    CHECK(ptr1->get_refcount() == 2);
    
    // non-default constructor
    ptr<test_class> ptr2 = test_class::create(128);
    CHECK(ptr2->get() == 128);
    
    // invalidation
    ptr0.invalidate();
    CHECK(!bool(ptr0));
    CHECK(bool(ptr1));
    CHECK(ptr1->get_refcount() == 1);
    
    // move semantics
    ptr0 = std::move(ptr1);
    CHECK(ptr0.isvalid());
    CHECK(!ptr1.isvalid());
    CHECK(ptr0->get_refcount() == 1);
}

TEST(create_ptr_benchmark) {

    for (int32 i = 0; i < 3; i++) {
        chrono::time_point<chrono::system_clock> start, end;
        start = chrono::system_clock::now();
        
        const int32 max_live_objects = refcounted::get_pool_size();
        const int32 num_objects = 1000000;
        const int32 num_outer_loop = num_objects / max_live_objects;
        for (int32 j = 0; j < num_outer_loop; j++) {
            std::vector<ptr<refcounted>> objs;
            objs.reserve(max_live_objects);
            for (int32 k = 0; k < max_live_objects; k++) {
                objs.emplace_back(refcounted::create());
            }
        }
        end = chrono::system_clock::now();
        chrono::duration<double> dur = end - start;
        log::info("run %d: %dx ptr<refcounted> created: %f sec\n", i, num_outer_loop * max_live_objects, dur.count());
    }

    for (int i = 0; i < 3; i++) {
        chrono::time_point<chrono::system_clock> start, end;
        start = chrono::system_clock::now();
        
        const int32 num_objects = 1000000;
        std::vector<shared_ptr<refcounted>> objs;
        objs.reserve(num_objects);
        for (int32 i = 0; i < num_objects; i++) {
            objs.emplace_back(std::make_shared<refcounted>());
        }
        objs.clear();   // important: also measure destruction
        
        end = chrono::system_clock::now();
        chrono::duration<double> dur = end - start;
        log::info("run %d: %dx shared_ptr<refcounted> created: %f sec\n", i, num_objects, dur.count());
    }
}

#if ORYOL_HAS_THREADS
const int num_inner = refcounted::get_pool_size() / 8;
const int num_outer = 1000000 / num_inner;

void thread_func() {
    core::enter_thread();
    
    log::info("create_multithreaded: thread '%d' entered!\n", this_thread::get_id());
    
    for (int i = 0; i < num_outer; i++) {
        std::vector<ptr<refcounted>> pointers;
        pointers.reserve(num_inner);
        for (int j = 0 ; j < num_inner; j++) {
            pointers.push_back(refcounted::create());
            o_assert(1 == pointers.back()->get_refcount());
        }
    }

    log::info("create_multithreaded: thread '%d' leaving!\n", this_thread::get_id());
    
    core::leave_thread();
}

TEST(create_multithreaded) {
    
    chrono::time_point<chrono::system_clock> start, end;
    start = chrono::system_clock::now();
    
    // create a few threads
    const int num_threads = 8;
    std::array<std::thread, num_threads> threads;
    for (int32 i = 0; i < num_threads; i++) {
        threads[i] = std::thread(thread_func);
    }
    for (int32 i = 0; i < num_threads; i++) {
        threads[i].join();
    }
    
    end = chrono::system_clock::now();
    chrono::duration<double> dur = end - start;
    log::info("%d objects created in %d threads: %f sec\n", num_outer * num_inner * num_threads, num_threads, dur.count());
}
#endif