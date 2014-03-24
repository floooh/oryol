//------------------------------------------------------------------------------
//  Module.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Module.h"
#include "Core/CoreFacade.h"
#include "IO/IOFacade.h"

namespace Oryol {
namespace Render {
    
//------------------------------------------------------------------------------
void
Module::Setup() {
    o_assert(Core::CoreFacade::HasInstance());
    o_assert(IO::IOFacade::HasInstance());
    RenderFacade::CreateSingleton();
}

//------------------------------------------------------------------------------
void
Module::Discard() {
    RenderFacade::DestroySingleton();
}

} // namespace Render
} // namespace Oryol