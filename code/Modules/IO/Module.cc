//------------------------------------------------------------------------------
//  Module.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Module.h"
#include "Core/CoreFacade.h"

namespace Oryol {
namespace IO {
    
//------------------------------------------------------------------------------
void
Module::Setup() {
    o_assert(Core::CoreFacade::HasInstance());
    IOFacade::CreateSingleton();
}

//------------------------------------------------------------------------------
void
Module::Discard() {
    IOFacade::DestroySingleton();
}

} // namespace IO
} // namespace Oryol