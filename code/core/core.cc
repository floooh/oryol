//------------------------------------------------------------------------------
//  core.cc
//------------------------------------------------------------------------------
#include "pre.h"
#include "core.h"
#include "string/stringatom_table.h"

namespace oryol {
namespace core {
    
using namespace string;
    
//------------------------------------------------------------------------------
void setup() {

    // create the main-thread string atom table
    stringatom_table::create_singleton();
}

//------------------------------------------------------------------------------
void discard() {

    // do NOT destroy the thread-local string atom table to
    // ensure that string atom data pointers still point to valid data!!!
}

//------------------------------------------------------------------------------
void enter_thread() {

    // create thread-local string atom table
    stringatom_table::create_singleton();
}

//------------------------------------------------------------------------------
void leave_thread() {

    // do NOT destroy the thread-local string atom table to
    // ensure that string atom data pointers still point to valid data
}

} // namespace core
} // namespace oryol
