//------------------------------------------------------------------------------
//  stringatom_test.cc
//  Test stringatom system.
//------------------------------------------------------------------------------
#include "pre.h"
#include "UnitTest++/src/unittest++.h"
#include "core/string/stringatom.h"

using namespace oryol;
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

TEST(stringatom_multithreaded) {
    // FIXME FIXME!!!
}
