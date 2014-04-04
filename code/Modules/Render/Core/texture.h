#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::texture
    @brief texture frontend class
    
    A texture object can be a normal 2D, 3D or cube texture, as well
    as a render target with optional depth buffer.
*/
#if ORYOL_OPENGL
#include "Render/gl/glTexture.h"
namespace Oryol {
namespace Render {
class texture : public glTexture { };
} }
#else
#error "Target platform not yet supported!"
#endif



