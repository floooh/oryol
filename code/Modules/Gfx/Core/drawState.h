#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::drawState
    @ingroup _priv
    @brief bundles pre-compiled state for drawing operations
*/
#include "Resource/resourceBase.h"
#include "Gfx/Setup/DrawStateSetup.h"

namespace Oryol {
namespace _priv {

class mesh;
class programBundle;

class drawState : public resourceBase<DrawStateSetup> {
public:
    /// constructor
    drawState();
    /// destructor
    ~drawState();
    
    /// clear the object
    void clear();
    
    /// mesh pointer
    class mesh* mesh;
    /// program bundle pointer
    class programBundle* programBundle;
};

} // namespace _priv
} // namespace Oryol