//------------------------------------------------------------------------------
//  programBundleFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "programBundleFactory.h"
#include "Render/Core/Enums.h"

namespace Oryol {
namespace Render {
    
//------------------------------------------------------------------------------
uint16
programBundleFactory::GetResourceType() const {
    return ResourceType::ProgramBundle;
}
    
} // namespace Render
} // namespace Oryol
