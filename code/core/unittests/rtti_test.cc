//------------------------------------------------------------------------------
//  rtti.cc
//  Test standard C++ rtti stuff with oryol classes.
//------------------------------------------------------------------------------
#include "pre.h"
#include "UnitTest++/src/unittest++.h"
#include "core/macros.h"
#include "core/log.h"
#include "core/refcounted.h"
#include "core/ptr.h"

using namespace std;
using namespace oryol;
using namespace core;

class A : public core::refcounted {
    oryol_class_decl(A, 10);
public:
    /// constructor
    A() { };
    /// destructor
    virtual ~A() { };
};
oryol_class_impl(A, 10);

class AA : public A {
    oryol_class_decl(AA, 10);
public:
    /// constructor
    AA() { };
    /// destructor
    virtual ~AA() { };
};
oryol_class_impl(AA, 10);

class AB : public A {
    oryol_class_decl(AB, 10);
public:
    /// constructor
    AB() { };
    /// destructor
    virtual ~AB() { };
};
oryol_class_impl(AB, 10);

//------------------------------------------------------------------------------
TEST(rtti) {
    
    ptr<A> a = A::create();
    CHECK(a->get_refcount() == 1);
    ptr<AA> aa = AA::create();
    CHECK(a->get_refcount() == 1);
    ptr<AB> ab = AB::create();
    CHECK(a->get_refcount() == 1);
    
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
    
    ptr<A> a1 = aa;
    CHECK(bool(a1));
    CHECK(a1->get_refcount() == 2);
    CHECK(a1 == aa);
    
    ptr<A> a2 = ab;
    CHECK(bool(a2));
    CHECK(a2->get_refcount() == 2);
    CHECK(a2 == ab);
    
    ptr<A> a3 = aa.get();
    CHECK(bool(a3));
    CHECK(a3->get_refcount() == 3);
    CHECK(a3 == aa);
    
    ptr<A> a4 = ab.get_unsafe();
    CHECK(bool(a4));
    CHECK(a3->get_refcount() == 3);
    CHECK(a4 == ab);
}