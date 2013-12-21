//------------------------------------------------------------------------------
//  Core.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Core.h"
#include "String/stringAtomTable.h"

namespace Oryol {
namespace Core {
    
using namespace String;
    
//------------------------------------------------------------------------------
void Setup() {

    // create the main-thread string atom table
    stringAtomTable::CreateSingleton();
}

//------------------------------------------------------------------------------
void Discard() {

    // do NOT destroy the thread-local string atom table to
    // ensure that string atom data pointers still point to valid data!!!
}

//------------------------------------------------------------------------------
void EnterThread() {

    // create thread-local string atom table
    stringAtomTable::CreateSingleton();
}

//------------------------------------------------------------------------------
void LeaveThread() {

    // do NOT destroy the thread-local string atom table to
    // ensure that string atom data pointers still point to valid data
}

} // namespace Core
} // namespace Oryol
