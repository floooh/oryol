#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::drawState
    @ingroup _priv
    @brief bundles pre-compiled state for drawing operations
*/
#include "Resource/Core/resourceBase.h"
#include "Gfx/Setup/DrawStateSetup.h"

namespace Oryol {
namespace _priv {

class programBundle;

class drawState : public resourceBase<DrawStateSetup> {
public:
    /// constructor
    drawState();
    /// destructor
    ~drawState();
    
    /// clear the object
    void Clear();
    
    /// mesh resource Id (cannot be a pointer since meshes might be loaded async)
    class Id msh;
    /// program bundle pointer
    programBundle* prog;
};

} // namespace _priv
} // namespace Oryol