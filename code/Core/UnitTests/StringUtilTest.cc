//------------------------------------------------------------------------------
//  StringAtomTest.cc
//  Test string atom system.
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/String/StringUtil.h"

using namespace Oryol;
using namespace Core;

TEST(StringUtilTest) {
    
    // test tokenize
    Array<String> tokens;
    int32 numTokens = StringUtil::Tokenize("One Two Three", " ", tokens);
    CHECK(numTokens == tokens.Size());
    CHECK(tokens.Size() == 3);
    CHECK(tokens[0] == "One");
    CHECK(tokens[1] == "Two");
    CHECK(tokens[2] == "Three");
    
    StringUtil::Tokenize("  One   Two Three    ", " ", tokens);
    CHECK(tokens.Size() == 3);
    CHECK(numTokens == 3);
    CHECK(tokens[0] == "One");
    CHECK(tokens[1] == "Two");
    CHECK(tokens[2] == "Three");
    
    StringUtil::Tokenize(", One, \t Two \r Three", ", \t\r", tokens);
    CHECK(tokens.Size() == 3);
    CHECK(tokens[0] == "One");
    CHECK(tokens[1] == "Two");
    CHECK(tokens[2] == "Three");
    
    StringUtil::Tokenize("One", " ", tokens);
    CHECK(tokens.Size() == 1);
    CHECK(tokens[0] == "One");
    
    StringUtil::Tokenize("  ,;  ", " ,;", tokens);
    CHECK(tokens.Size() == 0);
    
    // test bisecting
    String str = "Left:Right";
    String left, right;
    StringUtil::Bisect(str, ":", left, right);
    CHECK(left == "Left");
    CHECK(right == "Right");
    
    str = ":Right";
    StringUtil::Bisect(str, ":", left, right);
    CHECK(left.Empty());
    CHECK(right == "Right");
    
    str = "Left:";
    StringUtil::Bisect(str, ":", left, right);
    CHECK(left == "Left");
    CHECK(right.Empty());
    
    str = "Left";
    StringUtil::Bisect(str, ":", left, right);
    CHECK(left == "Left");
    CHECK(right.Empty());
    
    str = ":";
    StringUtil::Bisect(str, ":", left, right);
    CHECK(left.Empty());
    CHECK(right.Empty());
    
    str = "bla://blub";
    StringUtil::Bisect(str, ":/", left, right);
    CHECK(left == "bla");
    CHECK(right == "blub");
    
    StringUtil::Bisect("", ":", left, right);
    CHECK(left.Empty());
    CHECK(right.Empty());
    
    // test concatenate
    str = StringUtil::Concatenate('/', { "bla", "blub", "blob" } );
    CHECK(str == "bla/blub/blob");
    
    // test append
    str = StringUtil::Append({ "One", "Two", "Three"} );
    CHECK(str == "OneTwoThree");
    
    // test truncate
    str = StringUtil::Truncate(str, 32);
    CHECK(str == "OneTwoThree");
    str = StringUtil::Truncate(str, 3);
    CHECK(str == "One");
    
}