#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::ResourceType
    @brief Render subsystem resource types
    
    These types are used for the type in Resource::Id for Render module
    resources.
*/
#include "Core/Types.h"

namespace Oryol {
namespace Render {
    
class ResourceType {
public:
    /// type enum
    enum Code {
        Texture,            ///< a texture
        Mesh,               ///< a mesh
        Program,            ///< shader program
        StateBlock,         ///< block of render states
        ConstantBlock,      ///< block constant shader uniforms
        
        NumResourceTypes,
        InvalidResourceType = 0xFFFF,
    };

    /// convert from to string
    static const char* ToString(Code c);
    /// convert from string
    static Code FromString(const char* str);
};
    
} // namespace Render
} // namespace Oryol