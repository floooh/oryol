//------------------------------------------------------------------------------
//  shaderFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "shaderFactory.h"

namespace Oryol {
namespace Render {
    
//------------------------------------------------------------------------------
uint16
shaderFactory::GetResourceType() const {
    return ResourceType::Shader;
}
    
} // namespace Render
} // namespace Oryol
