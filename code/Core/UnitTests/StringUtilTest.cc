//------------------------------------------------------------------------------
//  StringAtomTest.cc
//  Test string atom system.
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/String/StringUtil.h"

using namespace Oryol;
using namespace String;

TEST(StringUtilTest) {
    
    // test tokenize
    std::vector<std::string> tokens = StringUtil::Tokenize("One Two Three", " ");
    CHECK(tokens.size() == 3);
    CHECK(tokens[0] == "One");
    CHECK(tokens[1] == "Two");
    CHECK(tokens[2] == "Three");
    
    tokens = StringUtil::Tokenize("  One   Two Three    ", " ");
    CHECK(tokens.size() == 3);
    CHECK(tokens[0] == "One");
    CHECK(tokens[1] == "Two");
    CHECK(tokens[2] == "Three");
    
    tokens = StringUtil::Tokenize(", One, \t Two \r Three", ", \t\r");
    CHECK(tokens.size() == 3);
    CHECK(tokens[0] == "One");
    CHECK(tokens[1] == "Two");
    CHECK(tokens[2] == "Three");
    
    tokens = StringUtil::Tokenize("One", " ");
    CHECK(tokens.size() == 1);
    CHECK(tokens[0] == "One");
    
    tokens = StringUtil::Tokenize("  ,;  ", " ,;");
    CHECK(tokens.size() == 0);
    
    // test bisecting
    std::string str = "Left:Right";
    std::pair<std::string, std::string> res = StringUtil::Bisect(str, ":");
    CHECK(res.first == "Left");
    CHECK(res.second == "Right");
    
    str = ":Right";
    res = StringUtil::Bisect(str, ":");
    CHECK(res.first.empty());
    CHECK(res.second == "Right");
    
    str = "Left:";
    res = StringUtil::Bisect(str, ":");
    CHECK(res.first == "Left");
    CHECK(res.second.empty());
    
    str = ":";
    res = StringUtil::Bisect(str, ":");
    CHECK(res.first.empty());
    CHECK(res.second.empty());
    
    str = "bla://blub";
    res = StringUtil::Bisect(str, ":/");
    CHECK(res.first == "bla");
    CHECK(res.second == "blub");
    
    res = StringUtil::Bisect(std::string(), ":");
    CHECK(res.first.empty());
    CHECK(res.second.empty());
}