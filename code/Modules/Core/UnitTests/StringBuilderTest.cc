//------------------------------------------------------------------------------
//  StringBuilderTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Core/String/StringBuilder.h"

using namespace Oryol;
using namespace Oryol::Core;

TEST(StringBuilderTest) {
    
    StringBuilder builder;
    CHECK(builder.GetString().Empty());
    CHECK(builder.GetSubString(0, EndOfString).Empty());
    CHECK(builder.Capacity() == 0);
    CHECK(builder.Length() == 0);
    
    builder.Append('x');
    CHECK(builder.Capacity() > 0);
    CHECK(builder.Length() == 1);
    CHECK(builder.GetString() == "x");
    CHECK(builder.GetSubString(0, 1) == "x");
    CHECK(builder.GetSubString(0, EndOfString) == "x");
    
    builder.Append("Bla");
    CHECK(builder.Length() == 4);
    CHECK(builder.GetString() == "xBla");
    CHECK(builder.GetSubString(0, 1) == "x");
    CHECK(builder.GetSubString(1, EndOfString) == "Bla");
    CHECK(builder.GetSubString(1, 3) == "Bl");
    
    // appending nullptr is allowed
    builder.Append(nullptr);
    CHECK(builder.GetString() == "xBla");
    
    const char* blubber = "BlubberZZZ";
    builder.Append(blubber, 0, 7);
    CHECK(builder.Length() == 11);
    CHECK(builder.GetString() == "xBlaBlubber");
    
    const String blob("Blob");
    builder.Append(blob);
    CHECK(builder.Length() == 15);
    CHECK(builder.GetString() == "xBlaBlubberBlob");
    
    const String xxx("xxxBlabYYY");
    builder.Append(xxx, 3, 7);
    CHECK(builder.Length() == 19);
    CHECK(builder.GetString() == "xBlaBlubberBlobBlab");
    
    const String yyy("yyyBlub");
    builder.Append(yyy, 3, EndOfString);
    CHECK(builder.Length() == 23);
    CHECK(builder.GetString() == "xBlaBlubberBlobBlabBlub");
    
    builder.Clear();
    CHECK(builder.Capacity() > 0);
    CHECK(builder.Length() == 0);
    CHECK(builder.GetString().Empty());
    builder.Append({ "Bla", "Blub", "Blob", "Blubber" });
    CHECK(builder.GetString() == "BlaBlubBlobBlubber");
    
    builder.Clear();
    builder.Append(':', { "Bla", "Blub", "Blob", "Blubber" });
    CHECK(builder.GetString() == "Bla:Blub:Blob:Blubber");
    builder.Truncate(3);
    CHECK(builder.Length() == 3);
    CHECK(builder.GetString() == "Bla");
    CHECK('a' == builder.PopBack());
    CHECK(builder.Length() == 2);
    CHECK(builder.GetString() == "Bl");
    
    // Substitute
    builder.Clear();
    // try to substitute on empty builder
    CHECK(!builder.SubstituteFirst("Bla", "Blub"));
    CHECK(builder.Length() == 0);
    CHECK(builder.GetString().Empty());
    builder.Append("OneTwoThreeFourFiveSixSevenEight");
    // substitute non-existing string
    CHECK(!builder.SubstituteFirst("Bla", "Blub"));
    CHECK(builder.GetString() == "OneTwoThreeFourFiveSixSevenEight");
    // substitute in middle of string with longer substitute
    CHECK(builder.SubstituteFirst("Four", "Blubber"));
    CHECK(builder.GetString() == "OneTwoThreeBlubberFiveSixSevenEight");
    CHECK(builder.Length() == 35);
    // substitute in middle with smaller substitute
    CHECK(builder.SubstituteFirst("Blubber", "Four"));
    CHECK(builder.GetString() == "OneTwoThreeFourFiveSixSevenEight");
    CHECK(builder.Length() == 32);
    // subsitute with same-length string
    CHECK(builder.SubstituteFirst("Four", "Vier"));
    CHECK(builder.GetString() == "OneTwoThreeVierFiveSixSevenEight");
    CHECK(builder.Length() == 32);
    // substitute with empty string
    CHECK(builder.SubstituteFirst("Vier", ""));
    CHECK(builder.GetString() == "OneTwoThreeFiveSixSevenEight");
    CHECK(builder.Length() == 28);
    // substitute at end of string
    CHECK(builder.SubstituteFirst("Eight", "Acht"));
    CHECK(builder.GetString() == "OneTwoThreeFiveSixSevenAcht");
    CHECK(builder.Length() == 27);
    CHECK(builder.SubstituteFirst("Acht", "Eight"));
    CHECK(builder.GetString() == "OneTwoThreeFiveSixSevenEight");
    CHECK(builder.Length() == 28);
    CHECK(builder.SubstituteFirst("Eight", ""));
    CHECK(builder.GetString() == "OneTwoThreeFiveSixSeven");
    CHECK(builder.Length() == 23);
    // substitute at front of string
    CHECK(builder.SubstituteFirst("One", "Eins"));
    CHECK(builder.Length() == 24);
    CHECK(builder.GetString() == "EinsTwoThreeFiveSixSeven");
    CHECK(builder.SubstituteFirst("Eins", "One"));
    CHECK(builder.GetString() == "OneTwoThreeFiveSixSeven");
    CHECK(builder.Length() == 23);
    CHECK(builder.SubstituteFirst("One", ""));
    CHECK(builder.GetString() == "TwoThreeFiveSixSeven");
    CHECK(builder.Length() == 20);
    
    // SubstituteRange
    builder.SubstituteRange(0, 3, "Zwei");
    CHECK(builder.GetString() == "ZweiThreeFiveSixSeven");
    builder.SubstituteRange(16, EndOfString, "Sieben");
    CHECK(builder.GetString() == "ZweiThreeFiveSixSieben");
    builder.SubstituteRange(4, 9, "Drei");
    CHECK(builder.GetString() == "ZweiDreiFiveSixSieben");

    // SubstituteAll
    builder.Set(" One Two Three Four ");
    CHECK(builder.SubstituteAll(" ", "XXX") == 5);
    CHECK(builder.GetString() == "XXXOneXXXTwoXXXThreeXXXFourXXX");

    // test tokenize
    Array<String> tokens;
    builder.Set("One Two Three");
    int32 numTokens = builder.Tokenize(" ", tokens);
    CHECK(builder.Length() == 0);
    CHECK(numTokens == tokens.Size());
    CHECK(tokens.Size() == 3);
    CHECK(tokens[0] == "One");
    CHECK(tokens[1] == "Two");
    CHECK(tokens[2] == "Three");
    
    builder.Set("  One   Two Three    ");
    builder.Tokenize(" ", tokens);
    CHECK(tokens.Size() == 3);
    CHECK(numTokens == 3);
    CHECK(tokens[0] == "One");
    CHECK(tokens[1] == "Two");
    CHECK(tokens[2] == "Three");
    
    builder.Set(", One, \t Two \r Three");
    builder.Tokenize(", \t\r", tokens);
    CHECK(tokens.Size() == 3);
    CHECK(tokens[0] == "One");
    CHECK(tokens[1] == "Two");
    CHECK(tokens[2] == "Three");

    builder.Set("One");
    builder.Tokenize(" ", tokens);
    CHECK(tokens.Size() == 1);
    CHECK(tokens[0] == "One");
    
    builder.Set("  ,;  ");
    builder.Tokenize(" ,;", tokens);
    CHECK(tokens.Size() == 0);
    
    // FindFirstOf, FindFirstNotOf, FindSubString
    builder.Set("http://bla.blob.com:8000");
    CHECK(builder.FindFirstOf(0, EndOfString, ":") == 4);
    CHECK(builder.FindFirstNotOf(0, EndOfString, "htp:/") == 7);
    CHECK(builder.FindFirstOf(0, 4, ":") == InvalidIndex);
    CHECK(builder.FindFirstOf(7, 12, ".") == 10);
    CHECK(builder.FindSubString(0, EndOfString, "://") == 4);
    CHECK(builder.Contains("bla.blob"));
    CHECK(!builder.Contains("blub"));
    
    // test Format
    CHECK(!builder.Format(4, "One: %d, Two: %d, Three: %d", 1, 2, 3));   // this should fail because of no room
    CHECK(builder.GetString() == "");
    CHECK(builder.Format(256, "One: %d, Two: %d, Three: %d", 1, 2, 3));
    CHECK(builder.GetString() == "One: 1, Two: 2, Three: 3");
    CHECK(builder.AppendFormat(256, " Bla: %d", 46));
    CHECK(builder.GetString() == "One: 1, Two: 2, Three: 3 Bla: 46");
    CHECK(!builder.AppendFormat(4, " BLUB: %s", "bla"));
    CHECK(builder.GetString() == "One: 1, Two: 2, Three: 3 Bla: 46");
}

