//------------------------------------------------------------------------------
//  CreatorTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/Creator.h"
#include "Core/RefCounted.h"
#include "Core/String/String.h"
#include "Core/Containers/Array.h"

using namespace Oryol;

class CreatorTestClass : public RefCounted {
    OryolClassDecl(CreatorTestClass);
    OryolClassCreator(CreatorTestClass);
public:
    CreatorTestClass() : i0(0) {};
    CreatorTestClass(int i) : i0(i) {};
    CreatorTestClass(int i, String str) : i0(i), str0(str) {};
    CreatorTestClass(int i, String str, Array<int> arr) : i0(i), str0(str), array0(arr) {};
    virtual int Bla() const {
        return 0;
    };
    
    int i0;
    String str0;
    Array<int> array0;
};

class CreatorDerivedClass : public CreatorTestClass {
    OryolClassDecl(CreatorDerivedClass);
    OryolClassCreator(CreatorDerivedClass);
public:
    CreatorDerivedClass() : v0(0) {};
    CreatorDerivedClass(int64_t v0_) : v0(v0_) {};
    virtual int Bla() const override {
        return 1;
    };
    
    int64_t v0;
};

void TestFunc(std::function<Ptr<CreatorTestClass>()> bla) {
    auto obj = bla();
    CHECK(obj->Bla() == 1);
}

TEST(CreatorTest) {
    auto creat0 = CreatorTestClass::Creator();

    #if !(defined(__GNUC__) && !defined(__clang__))
    /** WORKAROUND for GCC bug 'lambda argument pack' **/
    auto creat1 = CreatorTestClass::Creator(10);
    auto creat2 = CreatorTestClass::Creator(20, "Bla");
    auto creat3 = CreatorTestClass::Creator(30, "Blub", Array<int>( { 1, 2, 3, 4 } ));
    #endif
    
    const auto obj0 = creat0();

    #if !(defined(__GNUC__) && !defined(__clang__))
    /** WORKAROUND for GCC bug 'lambda argument pack' **/
    const auto obj1 = creat1();
    const auto obj2 = creat2();
    const auto obj3 = creat3();
    #endif
    
    CHECK(obj0.isValid());
    CHECK(obj0->i0 == 0);
    CHECK(obj0->str0.Empty());
    CHECK(obj0->array0.Empty());
    
    #if !(defined(__GNUC__) && !defined(__clang__))
    /** WORKAROUND for GCC bug 'lambda argument pack' **/
    CHECK(obj1.isValid());
    CHECK(obj1->i0 == 10);
    CHECK(obj1->str0.Empty());
    CHECK(obj0->array0.Empty());
    
    CHECK(obj2.isValid());
    CHECK(obj2->i0 == 20);
    CHECK(obj2->str0 == "Bla");
    CHECK(obj0->array0.Empty());

    CHECK(obj3.isValid());
    CHECK(obj3->i0 == 30);
    CHECK(obj3->str0 == "Blub");
    CHECK(obj3->array0.Size() == 4);
    CHECK(obj3->array0[0] == 1);
    CHECK(obj3->array0[1] == 2);
    CHECK(obj3->array0[2] == 3);
    CHECK(obj3->array0[3] == 4);
    
    TestFunc(CreatorDerivedClass::Creator(10));
    #endif
}

