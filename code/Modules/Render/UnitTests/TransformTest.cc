//------------------------------------------------------------------------------
//  transformMgrTest.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "UnitTest++/src/UnitTest++.h"
#include "Render/Core/transformMgr.h"
#include "Render/RenderFacade.h"
#define GLM_FORCE_RADIANS
#include "glm/gtx/matrix_query.hpp"
#include "glm/gtc/matrix_transform.hpp"

using namespace Oryol;
using namespace Oryol::Render;

//------------------------------------------------------------------------------
TEST(transformMgrTest) {
    const float32 epsilon = 0.00001f;
    transformMgr tformMgr;
    
    // check whether everything is initialized with the identity matrix
    CHECK(glm::isIdentity(tformMgr.GetModel(), epsilon));
    CHECK(glm::isIdentity(tformMgr.GetView(), epsilon));
    CHECK(glm::isIdentity(tformMgr.GetProj(), epsilon));
    CHECK(glm::isIdentity(tformMgr.GetModelView(), epsilon));
    CHECK(glm::isIdentity(tformMgr.GetViewProj(), epsilon));
    CHECK(glm::isIdentity(tformMgr.GetModelViewProj(), epsilon));
    CHECK(glm::isIdentity(tformMgr.GetInvModel(), epsilon));
    CHECK(glm::isIdentity(tformMgr.GetInvView(), epsilon));
    CHECK(glm::isIdentity(tformMgr.GetInvProj(), epsilon));
    CHECK(glm::isIdentity(tformMgr.GetInvModelView(), epsilon));
    CHECK(glm::isIdentity(tformMgr.GetInvModelViewProj(), epsilon));
    CHECK(glm::isIdentity(tformMgr.GetInvViewProj(), epsilon));
    for (uint32 i = 0; i < TransformType::NumTransformTypes; i++) {
        CHECK(glm::isIdentity(tformMgr.GetTransform((TransformType::Code)i), epsilon));
    }
    
    // build a model and a view matrix, and check their product
    glm::mat4 model = glm::translate(glm::mat4(), glm::vec3(1.0f, 2.0f, 3.0f));
    glm::mat4 view = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -1.0f));

    tformMgr.SetModel(model);
    tformMgr.SetView(view);
    glm::vec4 p0(0.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 p1 = tformMgr.GetModel() * p0;
    CHECK(p1.x == 1.0f && p1.y == 2.0f && p1.z == 3.0f && p1.w == 1.0f);
    glm::vec4 p2 = tformMgr.GetInvModel() * p1; // this should compute InvModel, since it's dirty
    CHECK(p2.x == p0.x && p2.y == p0.y && p2.z == p0.z && p2.w == 1.0f);
    glm::vec4 n0(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec4 n1 = tformMgr.GetModel() * n0;
    CHECK(n1.x == n0.x && n1.y == n0.y && n1.z == n0.z && n1.w == 0.0f);
    glm::vec4 n2 = tformMgr.GetInvModel() * n1;  // this shoud not compute InvModel, since it's clean
    CHECK(n2.x == n1.x && n2.y == n1.y && n2.z == n1.z && n2.w == 0.0f);
    
    glm::vec4 p3 = tformMgr.GetModelView() * p0;
    CHECK(p3.x == 1.0f && p3.y == 2.0f && p3.z == 2.0f && p3.w == 1.0f);
    glm::vec4 p4 = tformMgr.GetInvModelView() * p3;
    CHECK(p4.x == p0.x && p4.y == p0.y && p4.z == p0.z && p4.w == 1.0f);
}

//------------------------------------------------------------------------------
TEST(RenderFacadeTransformTest) {
    RenderFacade* renderFacade = RenderFacade::CreateSingleton();
    
    // this is copy-pasted from above
    glm::mat4 model = glm::translate(glm::mat4(), glm::vec3(1.0f, 2.0f, 3.0f));
    glm::mat4 view = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.0f, -1.0f));
    renderFacade->ApplyModelTransform(model);
    renderFacade->ApplyViewTransform(view);
    
    glm::vec4 p0(0.0f, 0.0f, 0.0f, 1.0f);
    glm::vec4 p1 = renderFacade->QueryTransform(TransformType::Model) * p0;
    CHECK(p1.x == 1.0f && p1.y == 2.0f && p1.z == 3.0f && p1.w == 1.0f);
    glm::vec4 p2 = renderFacade->QueryTransform(TransformType::InvModel) * p1; // this should compute InvModel, since it's dirty
    CHECK(p2.x == p0.x && p2.y == p0.y && p2.z == p0.z && p2.w == 1.0f);
    glm::vec4 n0(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec4 n1 = renderFacade->QueryTransform(TransformType::Model) * n0;
    CHECK(n1.x == n0.x && n1.y == n0.y && n1.z == n0.z && n1.w == 0.0f);
    glm::vec4 n2 = renderFacade->QueryTransform(TransformType::InvModel) * n1;  // this shoud not compute InvModel, since it's clean
    CHECK(n2.x == n1.x && n2.y == n1.y && n2.z == n1.z && n2.w == 0.0f);
    
    glm::vec4 p3 = renderFacade->QueryTransform(TransformType::ModelView) * p0;
    CHECK(p3.x == 1.0f && p3.y == 2.0f && p3.z == 2.0f && p3.w == 1.0f);
    glm::vec4 p4 = renderFacade->QueryTransform(TransformType::InvModelView) * p3;
    CHECK(p4.x == p0.x && p4.y == p0.y && p4.z == p0.z && p4.w == 1.0f);
    
    RenderFacade::DestroySingleton();
}


