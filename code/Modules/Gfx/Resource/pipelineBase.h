#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::pipelineBase
    @ingroup _priv
    @brief base class for pipeline implementations
*/
#include "Resource/Core/resourceBase.h"
#include "Gfx/Setup/PipelineSetup.h"

namespace Oryol {
namespace _priv {

class shader;
class mesh;

class pipelineBase : public resourceBase<PipelineSetup> {
public:
    /// destructor
    ~pipelineBase();
    /// clear the object
    void Clear();    
    /// shader pointer
    shader* shd = nullptr;
};

} // namespace _priv
} // namespace Oryol
