//------------------------------------------------------------------------------
// RttiTest.cc
// Test standard C++ rtti stuff with oryol classes.
/// @todo: C++ rtti may be switched off in the future.
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/Macros.h"
#include "Core/Log.h"
#include "Core/RefCounted.h"
#include "Core/Ptr.h"

#include <typeindex>

using namespace std;
using namespace Oryol;

class A : public RefCounted {
    OryolClassDecl(A);
public:
    /// constructor
    A() { Log::Info("constructor A called for '%p'\n", this); };
    /// destructor
    virtual ~A() { Log::Info("destructor ~A called for '%p'\n", this); };
};
OryolClassImpl(A);

class AA : public A {
    OryolClassDecl(AA);
public:
    /// constructor
    AA() { Log::Info("constructor AA called for '%p'\n", this); };
    /// destructor
    virtual ~AA() { Log::Info("destructor ~AA called for '%p'\n", this); };
};
OryolClassImpl(AA);

class AB : public A {
    OryolClassDecl(AB);
public:
    /// constructor
    AB() { Log::Info("constructor AB called for '%p'\n", this); };
    /// destructor
    virtual ~AB() { Log::Info("destructor ~AB called for '%p'\n", this); };
};
OryolClassImpl(AB);

//------------------------------------------------------------------------------
TEST(Rtti) {
    
    Ptr<A> a = A::Create();
    CHECK(a->GetRefCount() == 1);
    Ptr<AA> aa = AA::Create();
    CHECK(a->GetRefCount() == 1);
    Ptr<AB> ab = AB::Create();
    CHECK(a->GetRefCount() == 1);
    
    const type_info& classTypeA = typeid(A);
    const type_info& objTypeA = typeid(*a);
    const type_info& classTypeAA = typeid(AA);
    const type_info& objTypeAA = typeid(*aa);
    const type_info& classTypeAB = typeid(AB);
    const type_info& objTypeAB = typeid(*ab);
    
    CHECK(classTypeA == objTypeA);
    CHECK(classTypeAA == objTypeAA);
    CHECK(classTypeA != classTypeAA);
    CHECK(classTypeAB == objTypeAB);
    CHECK(classTypeAA != classTypeAB);
    CHECK(classTypeAB != classTypeA);
    CHECK(classTypeA.hash_code() == objTypeA.hash_code());
    CHECK(classTypeAA.hash_code() == objTypeAA.hash_code());
    CHECK(classTypeAB.hash_code() == objTypeAB.hash_code());
    CHECK(type_index(classTypeA) == type_index(objTypeA));
    CHECK(type_index(classTypeAA) == type_index(objTypeAA));
    CHECK(type_index(classTypeA) != type_index(classTypeAA));
    
    Ptr<A> a1(aa);
    CHECK(bool(a1));
    CHECK(a1->GetRefCount() == 2);
    CHECK(a1 == aa);
    
    Ptr<A> a2(ab);
    CHECK(bool(a2));
    CHECK(a2->GetRefCount() == 2);
    CHECK(a2 == ab);
    
    Ptr<A> a3 = aa.get();
    CHECK(bool(a3));
    CHECK(a3->GetRefCount() == 3);
    CHECK(a3 == aa);
    
    Ptr<A> a4 = ab.getUnsafe();
    CHECK(bool(a4));
    CHECK(a3->GetRefCount() == 3);
    CHECK(a4 == ab);
    
    a.invalidate();
    aa.invalidate();
    ab.invalidate();
    
    a1.invalidate();
    a2.invalidate();
    a3.invalidate();
    a4.invalidate();
    
}