//------------------------------------------------------------------------------
//  rtti.cc
//  Test standard C++ rtti stuff with oryol classes.
//------------------------------------------------------------------------------
#include "pre.h"
#include "UnitTest++/src/unittest++.h"
#include "core/macros.h"
#include "core/log.h"

using namespace std;
using namespace oryol;

class A {
    oryol_class(A);
public:
    /// constructor
    A() { };
    /// destructor
    virtual ~A() { };
};

class AA : public A {
    oryol_class(AA);
public:
    /// constructor
    AA() { };
    /// destructor
    virtual ~AA() { };
};

class AB : public A {
    oryol_class(AB);
public:
    /// constructor
    AB() { };
    /// destructor
    virtual ~AB() { };
};

//------------------------------------------------------------------------------
TEST(rtti) {
    
    shared_ptr<A> a = A::create_shared();
    shared_ptr<AA> aa = AA::create_shared();
    shared_ptr<AB> ab = AB::create_shared();
    
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
    
    shared_ptr<A> a1 = aa;
    CHECK(bool(a1));
    CHECK(a1 == aa);
    shared_ptr<A> a2 = ab;
    CHECK(bool(a2));
    CHECK(a2 == ab);
    shared_ptr<A> a3 = static_pointer_cast<A>(aa);
    CHECK(bool(a3));
    CHECK(a3 == aa);
    shared_ptr<A> a4 = static_pointer_cast<A>(ab);
    CHECK(bool(a4));
    CHECK(a4 == ab);
    
    shared_ptr<AA> a5 = dynamic_pointer_cast<AA>(ab);
    CHECK(!bool(a5));
    shared_ptr<AA> a6 = dynamic_pointer_cast<AA>(ab);
    CHECK(!bool(a6));
}