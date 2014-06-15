#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::blendState
    @brief resource class to encapsulate blend state
*/
#include "Resource/resourceBase.h"
#include "Render/Setup/BlendStateSetup.h"

namespace Oryol {
namespace Render {
    
class blendState : public Resource::resourceBase<BlendStateSetup> {
public:
    /// clear the object
    void clear();
};
    
} // namespace Render
} // namespace Oryol
