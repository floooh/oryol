//------------------------------------------------------------------------------
//  Module.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Module.h"
#include "Core/CoreFacade.h"
#include "String/stringAtomTable.h"

namespace Oryol {
namespace Core {
    
//------------------------------------------------------------------------------
void
Module::Setup() {
    // create the main-thread string atom table
    stringAtomTable::CreateSingleton();
    // create the CoreFacade global singleton
    CoreFacade::CreateSingleton();
}

//------------------------------------------------------------------------------
void
Module::Discard() {

    CoreFacade::DestroySingleton();
    // do NOT destroy the thread-local string atom table to
    // ensure that string atom data pointers still point to valid data!!!
}

//------------------------------------------------------------------------------
void
Module::EnterThread() {
    // create thread-local string atom table
    stringAtomTable::CreateSingleton();
    // notify CoreFacade
    CoreFacade::Instance()->enterThread();
}

//------------------------------------------------------------------------------
void
Module::LeaveThread() {
    CoreFacade::Instance()->leaveThread();

    // do NOT destroy the thread-local string atom table to
    // ensure that string atom data pointers still point to valid data
}

} // namespace Core
} // namespace Oryol
