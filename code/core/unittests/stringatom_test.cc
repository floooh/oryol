//------------------------------------------------------------------------------
//  stringatom_test.cc
//  Test stringatom system.
//------------------------------------------------------------------------------
#include "pre.h"
#include "UnitTest++/src/unittest++.h"
#include "core/string/stringatom.h"
#include "core/core.h"

#include <array>

using namespace oryol;
using namespace core;
using namespace string;
using namespace std;

TEST(stringatom_singlethreaded) {
    
    stringatom atom0;
    CHECK(!atom0.isvalid());
    CHECK(atom0.as_cstr()[0] == 0);
    CHECK(atom0.as_string() == std::string());
    
    stringatom atom1("BLA!");
    CHECK(atom1.isvalid());
    stringatom atom2(std::string("BLA!"));
    CHECK(atom2.isvalid());
    CHECK(atom1 == atom2);
    stringatom atom3(atom2);
    CHECK(atom3.isvalid());
    CHECK(atom3 == atom1);
    
    stringatom atom4("BLUB");
    CHECK(atom4 == "BLUB");
    CHECK(atom1 != atom4);
    atom2 = atom4;
    CHECK(atom1 != atom2);
    
    CHECK((0 == strcmp(atom1.as_cstr(), "BLA!")));
    CHECK(atom1.as_string() == "BLA!");
    
    atom0 = "";
    CHECK(!atom0.isvalid());
    atom1 = "";
    CHECK(atom0 == atom1);
    atom0.clear();
    CHECK(!atom0.isvalid());
}

#if ORYOL_HAS_THREADS
// must use reference, the copy would fail since there is no thread-local
// string-atom table yet
static void thread_func(const stringatom& a0) {
    core::enter_thread();
    
    // transfer into a thread-local string atom
    stringatom a1(a0);
    stringatom a2(a0);
    CHECK(a0 == a1);
    CHECK(a1 == a2);
    CHECK(a1.as_string() == "BLOB");
    CHECK(a0.as_string() == "BLOB");
    CHECK(a2.as_string() == "BLOB");
    
    core::leave_thread();
}

// test stringatom transfer into other thread
TEST(stringatom_multithreaded) {
    
    stringatom atom0("BLOB");
    std::thread t1(thread_func, std::ref(atom0));
    t1.join();
}
#endif

// test string atom creation performance
TEST(stringatom_performance) {

    const int num_unique_strings = 16;  // must be 2^N
    std::array<const char*, num_unique_strings> unique_strings = {
        { "ONE", "TWO", "THREE", "FOUR", "FIVE", "SIX", "SEVEN", "EIGHT",
          "NINE", "TEN", "ELEVEL", "TWELVE", "THIRTEEN", "FOURTEEN", "FIFTEEN", "SIXTEEN"
        }
    };
    const int mask = num_unique_strings - 1;
    
    for (int i = 0; i < 3; i++) {
        chrono::time_point<chrono::system_clock> start, end;
        start = chrono::system_clock::now();
        
        const int num_stringatoms = 1000000;
        std::vector<stringatom> stringatoms;
        stringatoms.reserve(num_stringatoms);
        for (int i = 0; i < num_stringatoms; i++) {
            stringatoms.emplace_back(unique_strings[i & mask]);
        }
        
        end = chrono::system_clock::now();
        chrono::duration<double> dur = end - start;
        log::info("run %d: %dx stringatoms created: %f sec\n", i, num_stringatoms, dur.count());
    }
}