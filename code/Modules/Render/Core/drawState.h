#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::drawState
    @brief bundles pre-compiled state for drawing operations
*/
#include "Resource/resourceBase.h"
#include "Render/Setup/DrawStateSetup.h"

namespace Oryol {
namespace Render {

class blendState;
class depthStencilState;
class mesh;
class programBundle;

class drawState : public Resource::resourceBase<DrawStateSetup> {
public:
    /// constructor
    drawState();
    /// destructor
    ~drawState();
    
    /// clear the object
    void clear();
    
    /// set blend state pointer
    void setBlendState(blendState* bs);
    /// get blend state pointer
    blendState* getBlendState() const;
    /// set depthStencilState pointer
    void setDepthStencilState(depthStencilState* dss);
    /// get depthStencilState pointer
    depthStencilState* getDepthStencilState() const;
    /// set mesh pointer
    void setMesh(mesh* msh);
    /// get mesh pointer
    mesh* getMesh() const;
    /// set program bundle pointer
    void setProgramBundle(programBundle* pb);
    /// get program bundle pointer
    programBundle* getProgramBundle() const;
    
private:
    class blendState* blendState;
    class depthStencilState* depthStencilState;
    class mesh* mesh;
    class programBundle* programBundle;
};

//------------------------------------------------------------------------------
inline blendState*
drawState::getBlendState() const {
    return this->blendState;
}

//------------------------------------------------------------------------------
inline depthStencilState*
drawState::getDepthStencilState() const {
    return this->depthStencilState;
}

//------------------------------------------------------------------------------
inline mesh*
drawState::getMesh() const {
    return this->mesh;
}

//------------------------------------------------------------------------------
inline programBundle*
drawState::getProgramBundle() const {
    return this->programBundle;
}

} // namespace Render
} // namespace Oryol