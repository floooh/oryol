//------------------------------------------------------------------------------
//  ArgsTest.cc
//  Test Args class.
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/Args.h"

using namespace Oryol;
using namespace Oryol::Core;

TEST(Args) {
    
    // construct from String
    Args args("-name \"John Shaftoe\"\t\t-age 30 -male -height 6.1\r\n");
    CHECK(args.HasArg("-name"));
    CHECK(args.HasArg("John Shaftoe"));
    CHECK(args.HasArg("-age"));
    CHECK(args.HasArg("30"));
    CHECK(args.HasArg("-male"));
    CHECK(args.HasArg("-height"));
    CHECK(args.HasArg("6.1"));
    CHECK(args.GetString("-name") == "John Shaftoe");
    CHECK(args.GetInt("-age") == 30);
    CHECK_CLOSE(args.GetFloat("-height"), 6.1f, 0.001f);
    
    // construct from WideString
    Args argsw(L"-name \"John Shaftoe\" -age 30 -male -height 6.1");
    CHECK(argsw.HasArg("-name"));
    CHECK(argsw.HasArg("John Shaftoe"));
    CHECK(argsw.HasArg("-age"));
    CHECK(argsw.HasArg("30"));
    CHECK(argsw.HasArg("-male"));
    CHECK(argsw.HasArg("-height"));
    CHECK(argsw.HasArg("6.1"));
    CHECK(argsw.GetString("-name") == "John Shaftoe");
    CHECK(argsw.GetInt("-age") == 30);
    CHECK_CLOSE(argsw.GetFloat("-height"), 6.1f, 0.001f);

    // construct from argc and argv
    const char* argv[] = { "-name", "John Shaftoe", "-age", "30", "-male", "-height", "6.1" };
    int argc = 7;
    Args argsCStyle(argc, argv);
    CHECK(argsCStyle.HasArg("-name"));
    CHECK(argsCStyle.HasArg("John Shaftoe"));
    CHECK(argsCStyle.HasArg("-age"));
    CHECK(argsCStyle.HasArg("30"));
    CHECK(argsCStyle.HasArg("-male"));
    CHECK(argsCStyle.HasArg("-height"));
    CHECK(argsCStyle.HasArg("6.1"));
    CHECK(argsCStyle.GetString("-name") == "John Shaftoe");
    CHECK(argsCStyle.GetInt("-age") == 30);
    CHECK_CLOSE(argsCStyle.GetFloat("-height"), 6.1f, 0.001f);
}