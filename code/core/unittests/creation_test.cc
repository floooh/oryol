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

    for (int i = 0; i < 3; i++) {
        chrono::time_point<chrono::system_clock> start, end;
        start = chrono::system_clock::now();
        
        const int32 num_objects = 1000000;
        std::vector<ptr<refcounted>> objs;
        objs.reserve(num_objects);
        for (int32 i = 0; i < num_objects; i++) {
            objs.emplace_back(refcounted::create());
        }

        end = chrono::system_clock::now();
        chrono::duration<double> dur = end - start;
        log::info("run %d: %dx ptr<refcounted> created: %f sec\n", i, num_objects, dur.count());
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
        
        end = chrono::system_clock::now();
        chrono::duration<double> dur = end - start;
        log::info("run %d: %dx shared_ptr<refcounted> created: %f sec\n", i, num_objects, dur.count());
    }
}
