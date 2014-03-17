#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::transformMgr
    @brief manage model, view, projection transforms and their combinations
    
    Matrices are computed lazily, so for instance the inverse model matrix
    will only be computed in GetInvModel(), and only if the value is dirty.
*/
#include "Render/Types/TransformType.h"
#include "glm/mat4x4.hpp"

namespace Oryol {
namespace Render {

class transformMgr {
public:
    /// constructor
    transformMgr();
    
    /// set the model transform
    void SetModel(const glm::mat4& m);
    /// set the view transform
    void SetView(const glm::mat4& m);
    /// set the projection transform
    void SetProj(const glm::mat4& m);
    
    /// get the Model transform
    const glm::mat4& GetModel() const;
    /// get the View transform
    const glm::mat4& GetView() const;
    /// get the Projection transform
    const glm::mat4& GetProj() const;
    /// get the ModelView transform
    const glm::mat4& GetModelView() const;
    /// get the ViewProj transform
    const glm::mat4& GetViewProj() const;
    /// get ModelViewProj transform
    const glm::mat4& GetModelViewProj() const;
    /// get the InvModel transform
    const glm::mat4& GetInvModel() const;
    /// get the InvView transform
    const glm::mat4& GetInvView() const;
    /// get the InvProj transform
    const glm::mat4& GetInvProj() const;
    /// get the InvModelView transform
    const glm::mat4& GetInvModelView() const;
    /// get the InvModelViewProj transform
    const glm::mat4& GetInvModelViewProj() const;
    /// get the InvViewProj transform
    const glm::mat4& GetInvViewProj() const;
    
    /// get transform by transform type
    const glm::mat4& GetTransform(TransformType::Code type) const;

private:
    /// set dirty flags
    void setDirty(uint32 mask);
    /// clear a dirty flag
    void clearDirty(TransformType::Code t) const;
    /// test if a matrix is dirty
    bool isDirty(TransformType::Code t) const;
    /// update the ModelView matrix
    void updateModelView() const;
    /// update the ViewProj matrix
    void updateViewProj() const;
    /// update the ModelViewProj matrix
    void updateModelViewProj() const;
    /// update the InvModel matrix
    void updateInvModel() const;
    /// update the InvView matrix
    void updateInvView() const;
    /// update the InvProj matrix
    void updateInvProj() const;
    /// update the InvModelView matrix
    void updateInvModelView() const;
    /// update the InvModelViewProj matrix
    void updateInvModelViewProj() const;
    /// update the InvViewProj matrix
    void updateInvViewProj() const;
    
    mutable glm::mat4 transforms[TransformType::NumTransformTypes];
    mutable uint32 dirtyMask;
    glm::mat4 dummyIdentity;
};

//------------------------------------------------------------------------------
inline void
transformMgr::setDirty(uint32 mask) {
    this->dirtyMask |= mask;
}

//------------------------------------------------------------------------------
inline void
transformMgr::clearDirty(TransformType::Code t) const {
    this->dirtyMask &= ~(1<<t);
}

//------------------------------------------------------------------------------
inline bool
transformMgr::isDirty(TransformType::Code t) const {
    return 0 != (this->dirtyMask & (1<<t));
}

} // namespace Render
} // namespace Oryol
 