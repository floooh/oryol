//------------------------------------------------------------------------------
// RttiTest.cc
// Test standard C++ rtti stuff with oryol classes.
/// @todo: C++ rtti may be switched off in the future.
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/Log.h"
#include "Core/RefCounted.h"
#include "Core/Ptr.h"

#include <typeindex>

using namespace std;
using namespace Oryol;

namespace {
class A : public RefCounted {
    OryolClassDecl(A);
    OryolBaseTypeDecl(A);
public:
    /// constructor
    A() { Log::Info("constructor A called for '%p'\n", this); };
    /// destructor
    virtual ~A() { Log::Info("destructor ~A called for '%p'\n", this); };
};

class AA : public A {
    OryolClassDecl(AA);
    OryolTypeDecl(AA, A);
public:
    /// constructor
    AA() { Log::Info("constructor AA called for '%p'\n", this); };
    /// destructor
    virtual ~AA() { Log::Info("destructor ~AA called for '%p'\n", this); };
};

class AB : public A {
    OryolClassDecl(AB);
    OryolTypeDecl(AB, A);
public:
    /// constructor
    AB() { Log::Info("constructor AB called for '%p'\n", this); };
    /// destructor
    virtual ~AB() { Log::Info("destructor ~AB called for '%p'\n", this); };
};
}

//------------------------------------------------------------------------------
TEST(Rtti) {
    
    auto a = A::Create();
    CHECK(a->GetRefCount() == 1);
    CHECK(a->IsA<A>());
    CHECK(!a->IsA<AA>());
    CHECK(!a->IsA<AB>());
    auto aa = AA::Create();
    CHECK(aa->GetRefCount() == 1);
    CHECK(aa->IsA<A>());
    CHECK(aa->IsA<AA>());
    CHECK(!aa->IsA<AB>());
    auto ab = AB::Create();
    CHECK(ab->GetRefCount() == 1);
    CHECK(ab->IsA<A>());
    CHECK(!ab->IsA<AA>());
    CHECK(ab->IsA<AB>());
    
    Ptr<A> a1(aa);
    CHECK(bool(a1));
    CHECK(a1->GetRefCount() == 2);
    CHECK(a1->IsA<A>());
    CHECK(a1->IsA<AA>());
    CHECK(!a1->IsA<AB>());
    CHECK(a1 == aa);
    
    Ptr<A> a2(ab);
    CHECK(bool(a2));
    CHECK(a2->GetRefCount() == 2);
    CHECK(a2->IsA<A>());
    CHECK(!a2->IsA<AA>());
    CHECK(a2->IsA<AB>());
    CHECK(a2 == ab);
    
    Ptr<A> a3 = aa.get();
    CHECK(bool(a3));
    CHECK(a3->GetRefCount() == 3);
    CHECK(a3 == aa);
    
    Ptr<A> a4 = ab.getUnsafe();
    CHECK(bool(a4));
    CHECK(a3->GetRefCount() == 3);
    CHECK(a4 == ab);

    auto a5 = a1->DynamicCast<AA>();
    CHECK(a5.isValid());
    CHECK(a5->IsA<A>());
    CHECK(a5->IsA<AA>());
    CHECK(!a5->IsA<AB>());

    auto a6 = a2->DynamicCast<AB>();
    CHECK(a6.isValid());
    CHECK(a6->IsA<A>());
    CHECK(!a6->IsA<AA>());
    CHECK(a6->IsA<AB>());

    auto a7 = a->DynamicCast<A>();
    auto a8 = a->DynamicCast<AA>();
    auto a9 = a->DynamicCast<AB>();
    auto a10 = ab->DynamicCast<AA>();
    auto a11 = aa->DynamicCast<AB>();
    CHECK(a7.isValid());
    CHECK(!a8.isValid());
    CHECK(!a9.isValid());
    CHECK(!a10.isValid());
    CHECK(!a11.isValid());

    a.invalidate();
    aa.invalidate();
    ab.invalidate();
    
    a1.invalidate();
    a2.invalidate();
    a3.invalidate();
    a4.invalidate();
    a5.invalidate();
    a6.invalidate();
    a7.invalidate();
    a8.invalidate();
    a9.invalidate();
    a10.invalidate();
    a11.invalidate();
}


