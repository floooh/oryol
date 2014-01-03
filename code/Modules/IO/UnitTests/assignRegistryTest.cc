//------------------------------------------------------------------------------
//  assignRegistryTest.cc
//  Test assignRegistry singleton
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "IO/assignRegistry.h"
#include "Core/Ptr.h"

using namespace Oryol;
using namespace Core;
using namespace IO;

TEST(assignRegistryTest) {
    
    assignRegistry::CreateSingleton();
    assignRegistry* reg = assignRegistry::Instance();
    
    reg->SetAssign("home", "c:/program files/bla");
    reg->SetAssign("bla", "home:");
    reg->SetAssign("blub", "bla:blub");
    
    CHECK(reg->HasAssign("home"));
    CHECK(reg->HasAssign("bla"));
    CHECK(reg->HasAssign("blub"));
    CHECK(!reg->HasAssign("blob"));
    CHECK(reg->LookupAssign("home") == "c:/program files/bla");
    CHECK(reg->LookupAssign("bla") == "home:");
    CHECK(reg->LookupAssign("blub") == "bla:blub");
    CHECK(reg->LookupAssign("blob") == "");
    
    std::string res = reg->ResolveAssigns("blub:");
    CHECK(res == "c:/program files/bla/blub");
    reg->SetAssign("home", "http://www.flohofwoe.net");
    res = reg->ResolveAssigns("blub:");
    CHECK(res == "http://www.flohofwoe.net/blub");
}
