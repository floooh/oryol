//------------------------------------------------------------------------------
//  StringAtomTest.cc
//  Test string atom system.
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/unittest++.h"
#include "Core/String/StringAtom.h"
#include "Core/Core.h"

#include <array>

using namespace Oryol;
using namespace Core;
using namespace String;
using namespace std;

TEST(StringAtomSingleThreaded) {
    
    StringAtom atom0;
    CHECK(!atom0.IsValid());
    CHECK(atom0.AsCStr()[0] == 0);
    CHECK(atom0.AsString() == std::string());
    
    StringAtom atom1("BLA!");
    CHECK(atom1.IsValid());
    StringAtom atom2(std::string("BLA!"));
    CHECK(atom2.IsValid());
    CHECK(atom1 == atom2);
    StringAtom atom3(atom2);
    CHECK(atom3.IsValid());
    CHECK(atom3 == atom1);
    
    StringAtom atom4("BLUB");
    CHECK(atom4 == "BLUB");
    CHECK(atom1 != atom4);
    atom2 = atom4;
    CHECK(atom1 != atom2);
    
    CHECK((0 == strcmp(atom1.AsCStr(), "BLA!")));
    CHECK(atom1.AsString() == "BLA!");
    
    atom0 = "";
    CHECK(!atom0.IsValid());
    atom1 = "";
    CHECK(atom0 == atom1);
    atom0.Clear();
    CHECK(!atom0.IsValid());
}

#if ORYOL_HAS_THREADS
// must use reference, the copy would fail since there is no thread-local
// string-atom table yet
static void threadFunc(const StringAtom& a0) {
    
    Core::EnterThread();
    
    // transfer into a thread-local string atom
    StringAtom a1(a0);
    StringAtom a2(a0);
    CHECK(a0 == a1);
    CHECK(a1 == a2);
    CHECK(a1.AsString() == "BLOB");
    CHECK(a0.AsString() == "BLOB");
    CHECK(a2.AsString() == "BLOB");
    
    Core::LeaveThread();
}

// test string atom transfer into other thread
TEST(StringAtomMultiThreaded) {
    
    StringAtom atom0("BLOB");
    std::thread t1(threadFunc, std::ref(atom0));
    t1.join();
}
#endif

// test string atom creation performance
TEST(StringAtomPerformance) {

    const int32 numUniqueStrings = 16;  // must be 2^N
    std::array<const char*, numUniqueStrings> uniqueStrings = {
        { "ONE", "TWO", "THREE", "FOUR", "FIVE", "SIX", "SEVEN", "EIGHT",
          "NINE", "TEN", "ELEVEL", "TWELVE", "THIRTEEN", "FOURTEEN", "FIFTEEN", "SIXTEEN"
        }
    };
    const int32 mask = numUniqueStrings - 1;
    
    for (int32 i = 0; i < 3; i++) {
        chrono::time_point<chrono::system_clock> start, end;
        start = chrono::system_clock::now();
        
        const int32 numStringAtoms = 1000000;
        std::vector<StringAtom> stringAtoms;
        stringAtoms.reserve(numStringAtoms);
        for (int32 i = 0; i < numStringAtoms; i++) {
            stringAtoms.emplace_back(uniqueStrings[i & mask]);
        }
        
        end = chrono::system_clock::now();
        chrono::duration<double> dur = end - start;
        Log::Info("run %d: %dx StringAtoms created: %f sec\n", i, numStringAtoms, dur.count());
    }
}