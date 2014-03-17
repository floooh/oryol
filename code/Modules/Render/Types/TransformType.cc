//------------------------------------------------------------------------------
//  TransformType.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "TransformType.h"
#include "Core/Assert.h"
#include "Core/Macros.h"
#include <cstring>

namespace Oryol {
namespace Render {
    
//------------------------------------------------------------------------------
const char*
TransformType::ToString(Code c) {
    switch (c) {
        __ORYOL_TOSTRING(Model);
        __ORYOL_TOSTRING(View);
        __ORYOL_TOSTRING(Proj);
        __ORYOL_TOSTRING(ModelView);
        __ORYOL_TOSTRING(ViewProj);
        __ORYOL_TOSTRING(ModelViewProj);
        __ORYOL_TOSTRING(InvModel);
        __ORYOL_TOSTRING(InvView);
        __ORYOL_TOSTRING(InvProj);
        __ORYOL_TOSTRING(InvModelView);
        __ORYOL_TOSTRING(InvModelViewProj);
        __ORYOL_TOSTRING(InvViewProj);
        default: return "InvalidTransformType";
    }
}

//------------------------------------------------------------------------------
TransformType::Code
TransformType::FromString(const char* str) {
    o_assert(str);
    __ORYOL_FROMSTRING(Model);
    __ORYOL_FROMSTRING(View);
    __ORYOL_FROMSTRING(Proj);
    __ORYOL_FROMSTRING(ModelView);
    __ORYOL_FROMSTRING(ViewProj);
    __ORYOL_FROMSTRING(ModelViewProj);
    __ORYOL_FROMSTRING(InvModel);
    __ORYOL_FROMSTRING(InvView);
    __ORYOL_FROMSTRING(InvProj);
    __ORYOL_FROMSTRING(InvModelView);
    __ORYOL_FROMSTRING(InvModelViewProj);
    __ORYOL_FROMSTRING(InvViewProj);
    return InvalidTransformType;
}

} // namespace Render
} // namespace Oryol