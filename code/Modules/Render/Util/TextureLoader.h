#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::TextureLoader
    @ingroup Render
    @brief the default texture loader class
    
    TextureLoader is the default texture loader which can load the default
    texture file formats for the current target platform. These are usually
    compressed formats which can be direcly loaded without further conversion,
    such as DDS or PVR.
*/
#include "Core/Creator.h"
#if ORYOL_OPENGL
#include "Render/gl/glTextureLoader.h"
namespace Oryol {

class TextureLoader : public _priv::glTextureLoader {
    OryolClassDecl(TextureLoader);
    OryolClassCreator(TextureLoader);
};
    
} // namespace Oryol
#else
#error "TextureLoader not yet implemented for this platform!"
#endif
