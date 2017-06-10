//------------------------------------------------------------------------------
//  schemeRegistryTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/Core.h"
#include "IO/FileSystemBase.h"
#include "IO/private/schemeRegistry.h"

using namespace Oryol;
using namespace Oryol::_priv;

class TestFS_A : public FileSystemBase {
    OryolClassDecl(TestFS_A);
    OryolClassCreator(TestFS_A);
public:
    virtual int GetType() const {
        return 0;
    };
};

class TestFS_B : public TestFS_A {
    OryolClassDecl(TestFS_B);
    OryolClassCreator(TestFS_B);
public:
    virtual int GetType() const override {
        return 1;
    };
};

class TestFS_C : public TestFS_A {
    OryolClassDecl(TestFS_C);
    OryolClassCreator(TestFS_C);
public:
    virtual int GetType() const override {
        return 2;
    };
};

TEST(schemeRegistryTest) {
    
    Core::Setup();

    const StringAtom http("http");
    const StringAtom ftp("ftp");
    const StringAtom file("file");
    const StringAtom bla("bla");

    schemeRegistry reg;
    
    // @todo: hrmpf, this is ugly ...
    reg.RegisterFileSystem(http, TestFS_A::Creator());
    reg.RegisterFileSystem(ftp, TestFS_B::Creator());
    reg.RegisterFileSystem(file, TestFS_C::Creator());
    
    CHECK(reg.IsFileSystemRegistered(http));
    CHECK(reg.IsFileSystemRegistered(ftp));
    CHECK(reg.IsFileSystemRegistered(file));
    CHECK(!reg.IsFileSystemRegistered(bla));
    
    Ptr<TestFS_A> fsa = reg.CreateFileSystem(http).unsafeCast<TestFS_A>();
    CHECK(fsa);
    CHECK(fsa->GetType() == 0);
    Ptr<TestFS_B> fsb = reg.CreateFileSystem(ftp).unsafeCast<TestFS_B>();
    CHECK(fsb);
    CHECK(fsb->GetType() == 1);
    Ptr<TestFS_C> fsc = reg.CreateFileSystem(file).unsafeCast<TestFS_C>();
    CHECK(fsc);
    CHECK(fsc->GetType() == 2);

    Core::Discard();
}

