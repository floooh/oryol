//------------------------------------------------------------------------------
//  ContentTypeTest.cc
//  Test ContentType
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "IO/Core/ContentType.h"
#include <cstring>

using namespace Oryol::Core;
using namespace Oryol::IO;

TEST(ContentTypeTest) {
    
    // empty ContentType
    ContentType ct;
    CHECK(ct.Empty());
    CHECK(!ct.IsValid());
    CHECK(ct.Get() == "");
    CHECK(std::strcmp(ct.AsCStr(), "") == 0);
    CHECK(ct.Type() == "");
    CHECK(ct.SubType() == "");
    CHECK(ct.TypeAndSubType() == "");
    CHECK(ct.Params().Empty());
    CHECK(!ct.HasType());
    CHECK(!ct.HasSubType());
    CHECK(!ct.HasParams());

    // simple content-type (no params)
    ContentType ct0("image/png");
    CHECK(!ct0.Empty());
    CHECK(ct0.IsValid());
    CHECK(ct0.Get() == "image/png");
    CHECK(std::strcmp(ct0.AsCStr(), "image/png") == 0);
    CHECK(ct0.HasType());
    CHECK(ct0.HasSubType());
    CHECK(!ct0.HasParams());
    CHECK(ct0.Type() == "image");
    CHECK(ct0.SubType() == "png");
    CHECK(ct0.TypeAndSubType() == "image/png");
    
    // copy construction
    ContentType ct1(ct0);
    CHECK(ct0 == ct1);
    CHECK(ct1.HasType());
    CHECK(ct1.HasSubType());
    CHECK(!ct1.HasParams());
    CHECK(ct1.Type() == "image");
    CHECK(ct1.SubType() == "png");
    
    // copy-assign
    ContentType ct2;
    ct2 = ct1;
    CHECK(ct0 == ct2);
    CHECK(ct2.HasType());
    CHECK(ct2.HasSubType());
    CHECK(!ct2.HasParams());
    CHECK(ct2.Type() == "image");
    CHECK(ct2.SubType() == "png");
    
    // move-construction
    ContentType ct3(std::move(ct1));
    CHECK(ct3 != ct1);
    CHECK(!ct1.IsValid());
    CHECK(ct1.Empty());
    CHECK(ct3.HasType());
    CHECK(ct3.HasSubType());
    CHECK(!ct3.HasParams());
    CHECK(ct3.Type() == "image");
    CHECK(ct3.SubType() == "png");
    
    // move-assignment
    ContentType ct4;
    ct4 = std::move(ct3);
    CHECK(ct4 != ct3);
    CHECK(!ct3.IsValid());
    CHECK(ct3.Empty());
    CHECK(ct4.HasType());
    CHECK(ct4.HasSubType());
    CHECK(!ct4.HasParams());
    CHECK(ct4.Type() == "image");
    CHECK(ct4.SubType() == "png");
    
    // construct from string, plus params
    String str("text/plain; charset=iso-8859-1");
    ContentType ct5(str);
    CHECK(ct5.Type() == "text");
    CHECK(ct5.SubType() == "plain");
    CHECK(ct5.TypeAndSubType() == "text/plain");
    CHECK(ct5.HasParams());
    Map<String,String> ct5Params = ct5.Params();
    CHECK(ct5Params.Size() == 1);
    CHECK(ct5Params["charset"] == "iso-8859-1");
    
    // construct from string-atom, multiple params
    StringAtom stra("text/plain; charset=utf-8; bla=blub");
    ContentType ct6(stra);
    CHECK(ct6.Type() == "text");
    CHECK(ct6.SubType() == "plain");
    Map<String,String> ct6Params = ct6.Params();
    CHECK(ct6Params.Size() == 2);
    CHECK(ct6Params["charset"] == "utf-8");
    CHECK(ct6Params["bla"] == "blub");
}
