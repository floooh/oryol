//------------------------------------------------------------------------------
//	hello.cc
//------------------------------------------------------------------------------
#include "pre.h"
#include "core/types.h"
#include "core/macros.h"
#include "core/log.h"

using namespace oryol;
using namespace oryol::core;
using namespace std;

class myclass {
    oryol_class(myclass);
public:
    myclass() {  };
    ~myclass() {  };
    void set_test(int32 i) { test = i; };
    int32 get_test() const { return test; };
private:
    int32 test = 0;
};

int main(void) {
    shared_ptr<myclass> myobj = myclass::create_shared();
    myobj->set_test(16);
    int32 val = myobj->get_test();
    log::info("Hello World '%d'\n", val);
    return 0;
}