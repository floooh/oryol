//------------------------------------------------------------------------------
//  programBundleFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "programBundleFactory.h"
#include "Render/Types/ResourceType.h"

namespace Oryol {
namespace Render {
    
//------------------------------------------------------------------------------
uint16
programBundleFactory::GetResourceType() const {
    return ResourceType::ProgramBundle;
}
    
} // namespace Render
} // namespace Oryol
