#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::TransformType
    @brief transform matrix types
*/
#include "Core/Types.h"

namespace Oryol {
namespace Render {
    
class TransformType {
public:
    /// transform type enum
    enum Code {
        Model,              ///< model space -> world space
        View,               ///< world space -> view space
        Proj,               ///< view space -> projection space

        ModelView,          ///< model space -> view space
        ViewProj,           ///< world space -> projection space
        ModelViewProj,      ///< model space -> projection space

        InvModel,           ///< world space -> model space
        InvView,            ///< view space -> model space
        InvProj,            ///< projection space -> view space
        
        InvModelView,       ///< view space -> model space
        InvModelViewProj,   ///< projection space -> model space
        InvViewProj,        ///< projection space -> world space
        
        NumTransformTypes,
        InvalidTransformType = 0xFFFF,
    };
    
    /// convert to string
    static const char* ToString(Code c);
    /// convert from string
    static Code FromString(const char* str);
};
    
} // namespace Render
} // namespace Oryol