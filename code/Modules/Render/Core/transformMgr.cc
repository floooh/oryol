//------------------------------------------------------------------------------
//  transformMgr.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "transformMgr.h"
#include "glm/matrix.hpp"
#include "glm/gtc/matrix_inverse.hpp"

namespace Oryol {
namespace Render {
    
//------------------------------------------------------------------------------
transformMgr::transformMgr() :
dirtyMask(0) {
    // empty
}

//------------------------------------------------------------------------------
void
transformMgr::SetModel(const glm::mat4& m) {
    this->transforms[TransformType::Model] = m;
    this->setDirty(1<<TransformType::ModelView |
                   1<<TransformType::ModelViewProj |
                   1<<TransformType::InvModel |
                   1<<TransformType::InvModelView |
                   1<<TransformType::InvModelViewProj);
}
    
//------------------------------------------------------------------------------
void
transformMgr::SetView(const glm::mat4& m) {
    this->transforms[TransformType::View] = m;
    this->setDirty(1<<TransformType::ModelView |
                   1<<TransformType::ViewProj |
                   1<<TransformType::ModelViewProj |
                   1<<TransformType::InvView |
                   1<<TransformType::InvModelView |
                   1<<TransformType::InvModelViewProj |
                   1<<TransformType::InvViewProj);
}

//------------------------------------------------------------------------------
void
transformMgr::SetProj(const glm::mat4& m) {
    this->transforms[TransformType::Proj] = m;
    this->setDirty(1<<TransformType::ViewProj |
                   1<<TransformType::ModelViewProj |
                   1<<TransformType::InvProj |
                   1<<TransformType::InvModelViewProj |
                   1<<TransformType::InvViewProj);
}
        
//------------------------------------------------------------------------------
const glm::mat4&
transformMgr::GetModel() const {
    return this->transforms[TransformType::Model];
}

//------------------------------------------------------------------------------
const glm::mat4&
transformMgr::GetView() const {
    return this->transforms[TransformType::View];
}

//------------------------------------------------------------------------------
const glm::mat4&
transformMgr::GetProj() const {
    return this->transforms[TransformType::Proj];
}

//------------------------------------------------------------------------------
void
transformMgr::updateModelView() const {
    const glm::mat4& model = this->transforms[TransformType::Model];
    const glm::mat4& view  = this->transforms[TransformType::View];
    this->transforms[TransformType::ModelView] = view * model;
    this->clearDirty(TransformType::ModelView);
}
                            
//------------------------------------------------------------------------------
const glm::mat4&
transformMgr::GetModelView() const {
    if (this->isDirty(TransformType::ModelView)) {
        this->updateModelView();
    }
    return this->transforms[TransformType::ModelView];
}

//------------------------------------------------------------------------------
void
transformMgr::updateViewProj() const {
    const glm::mat4& view = this->transforms[TransformType::View];
    const glm::mat4& proj = this->transforms[TransformType::Proj];
    this->transforms[TransformType::ViewProj] = proj * view;
    this->clearDirty(TransformType::ViewProj);
}
                            
//------------------------------------------------------------------------------
const glm::mat4&
transformMgr::GetViewProj() const {
    if (this->isDirty(TransformType::ViewProj)) {
        this->updateViewProj();
    }
    return this->transforms[TransformType::ViewProj];
}

//------------------------------------------------------------------------------
void
transformMgr::updateModelViewProj() const {
    const glm::mat4& modelView = this->GetModelView();
    const glm::mat4& proj = this->transforms[TransformType::Proj];
    this->transforms[TransformType::ModelViewProj] = proj * modelView;
    this->clearDirty(TransformType::ModelViewProj);
}

//------------------------------------------------------------------------------
const glm::mat4&
transformMgr::GetModelViewProj() const {
    if (this->isDirty(TransformType::ModelViewProj)) {
        this->updateModelViewProj();
    }
    return this->transforms[TransformType::ModelViewProj];
}

//------------------------------------------------------------------------------
void
transformMgr::updateInvModel() const {
    const glm::mat4& model = this->transforms[TransformType::Model];
    this->transforms[TransformType::InvModel] = glm::affineInverse(model);
    this->clearDirty(TransformType::InvModel);
}

//------------------------------------------------------------------------------
const glm::mat4&
transformMgr::GetInvModel() const {
    if (this->isDirty(TransformType::InvModel)) {
        this->updateInvModel();
    }
    return this->transforms[TransformType::InvModel];
}

//------------------------------------------------------------------------------
void
transformMgr::updateInvView() const {
    const glm::mat4& view = this->transforms[TransformType::View];
    this->transforms[TransformType::InvView] = glm::affineInverse(view);
    this->clearDirty(TransformType::InvView);
}

//------------------------------------------------------------------------------
const glm::mat4&
transformMgr::GetInvView() const {
    if (this->isDirty(TransformType::InvView)) {
        this->updateInvView();
    }
    return this->transforms[TransformType::InvView];
}

//------------------------------------------------------------------------------
void
transformMgr::updateInvProj() const {
    const glm::mat4& proj = this->transforms[TransformType::Proj];
    this->transforms[TransformType::InvProj] = glm::inverse(proj);
    this->clearDirty(TransformType::InvProj);
}

//------------------------------------------------------------------------------
const glm::mat4&
transformMgr::GetInvProj() const {
    if (this->isDirty(TransformType::InvProj)) {
        this->updateInvProj();
    }
    return this->transforms[TransformType::InvProj];
}

//------------------------------------------------------------------------------
void
transformMgr::updateInvModelView() const {
    const glm::mat4& modelView = this->GetModelView();
    this->transforms[TransformType::InvModelView] = glm::affineInverse(modelView);
    this->clearDirty(TransformType::InvModelView);
}

//------------------------------------------------------------------------------
const glm::mat4&
transformMgr::GetInvModelView() const {
    if (this->isDirty(TransformType::InvModelView)) {
        this->updateInvModelView();
    }
    return this->transforms[TransformType::InvModelView];
}

//------------------------------------------------------------------------------
void
transformMgr::updateInvModelViewProj() const {
    const glm::mat4& modelViewProj = this->GetModelViewProj();
    this->transforms[TransformType::InvModelViewProj] = glm::inverse(modelViewProj);
    this->clearDirty(TransformType::InvModelViewProj);
}

//------------------------------------------------------------------------------
const glm::mat4&
transformMgr::GetInvModelViewProj() const {
    if (this->isDirty(TransformType::InvModelViewProj)) {
        this->updateInvModelViewProj();
    }
    return this->transforms[TransformType::InvModelViewProj];
}

//------------------------------------------------------------------------------
void
transformMgr::updateInvViewProj() const {
    const glm::mat4& viewProj = this->GetViewProj();
    this->transforms[TransformType::InvViewProj] = glm::inverse(viewProj);
    this->clearDirty(TransformType::InvViewProj);
}

//------------------------------------------------------------------------------
const glm::mat4&
transformMgr::GetInvViewProj() const {
    if (this->isDirty(TransformType::InvViewProj)) {
        this->updateInvViewProj();
    }
    return this->transforms[TransformType::InvViewProj];
}

//------------------------------------------------------------------------------
const glm::mat4&
transformMgr::GetTransform(TransformType::Code type) const {
    // hrmpf
    switch (type) {
        case TransformType::Model:              return this->GetModel();
        case TransformType::View:               return this->GetView();
        case TransformType::Proj:               return this->GetProj();
        case TransformType::ModelView:          return this->GetModelView();
        case TransformType::ViewProj:           return this->GetViewProj();
        case TransformType::ModelViewProj:      return this->GetModelViewProj();
        case TransformType::InvModel:           return this->GetInvModel();
        case TransformType::InvView:            return this->GetInvView();
        case TransformType::InvProj:            return this->GetInvProj();
        case TransformType::InvModelView:       return this->GetInvModelView();
        case TransformType::InvModelViewProj:   return this->GetInvModelViewProj();
        case TransformType::InvViewProj:        return this->GetInvViewProj();
        default:                                return this->dummyIdentity;
    }
}

} // namespace Render
} // namespace Oryol