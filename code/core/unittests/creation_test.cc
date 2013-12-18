//------------------------------------------------------------------------------
//  creation.cc
//  Test object creation for shared_ptr.
//------------------------------------------------------------------------------
#include "pre.h"
#include "UnitTest++/src/unittest++.h"
#include "core/log.h"
#include "core/types.h"
#include "core/macros.h"

using namespace std;
using namespace oryol;

// define a custom class
class test_class {
    oryol_class(test_class);
public:
    test_class() : val(0) {
        core::log::info("constructor called!\n");
    };
    virtual ~test_class() {
        core::log::info("destructor called!\n");
    };
    void set(int32 i) { val = i; };
    int32 get() const { return val; };
private:
    int32 val;
};

TEST(create_shared) {
    shared_ptr<test_class> ptr0 = test_class::create_shared();
    CHECK(bool(ptr0));
    CHECK(ptr0.use_count() == 1);
    CHECK(ptr0.unique());
    
    shared_ptr<test_class> ptr1 = ptr0;
    CHECK(ptr0 == ptr1);
    CHECK(ptr0 && ptr1);
    CHECK(ptr0.use_count() == 2);
    CHECK(ptr1.use_count() == 2);
    CHECK(!ptr0.unique());
    
    ptr0.reset();
    CHECK(!bool(ptr0));
    CHECK(bool(ptr1));
    CHECK(ptr1.use_count() == 1);
}

TEST(create_unique) {
    unique_ptr<test_class> ptr0 = test_class::create_unique();
    CHECK(bool(ptr0));
    unique_ptr<test_class> ptr1(move(ptr0));
    CHECK(bool(ptr1));
    CHECK(!bool(ptr0));
}
