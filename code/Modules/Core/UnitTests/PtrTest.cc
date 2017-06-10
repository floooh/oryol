//------------------------------------------------------------------------------
//  PtrTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/RefCounted.h"
#include "Core/Ptr.h"

using namespace Oryol;

namespace {
class A : public RefCounted {
    OryolClassDecl(A);
    void Incr() { i++; };
    int i = 0;
};
class B : public RefCounted {
    OryolClassDecl(B);
};
}

TEST(Ptr) {

    auto ptr = Ptr<RefCounted>();
    auto ptr0 = RefCounted::Create();
    auto ptr1 = RefCounted::Create();
    auto ptr2 = ptr0;
    auto ptr3(ptr0);

    CHECK(!ptr.isValid());
    CHECK(ptr0.isValid());
    CHECK(ptr1.isValid());
    CHECK(ptr2.isValid());
    CHECK(ptr3.isValid());
    CHECK(ptr < ptr0);
    CHECK(ptr0 > ptr);
    CHECK(ptr <= ptr0);
    CHECK(ptr0 >= ptr);
    CHECK(ptr2 == ptr0);
    CHECK(ptr3 == ptr0);
    CHECK(ptr2 >= ptr0);
    CHECK(ptr2 <= ptr0);
    CHECK(ptr1 != ptr0);
    CHECK(ptr == nullptr);
    CHECK(ptr0 != nullptr);
    CHECK(!ptr);
    CHECK(ptr0);

    CHECK(ptr.getUnsafe() == nullptr);
    CHECK(ptr0.getUnsafe() == ptr2.getUnsafe());
    CHECK(ptr1.getUnsafe() != ptr2.getUnsafe());

    Ptr<A> a = A::Create();
    // Ptr<B> b = A::Create();      // this should not compile
    // Ptr<B> b = a;                // this should not compile
    // Ptr<A> blob = new RefCounted();     // this should not compile
    // Ptr<A> blob = RefCounted::Create();  // this should not compile
    //Ptr<RefCounted> blub = new A();
    //Ptr<RefCounted> blob = A::Create();

    ptr3.invalidate();
    CHECK(ptr3.getUnsafe() == nullptr);
    ptr2.invalidate();
    CHECK(!ptr2);
    ptr1 = nullptr;
    CHECK(!ptr1.isValid());
}
