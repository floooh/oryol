#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::glInfo
    @ingroup _priv
    @brief dump information about the host's GL implementation
*/
#include "Core/Types.h"
#include "Gfx/gl/gl_decl.h"

namespace Oryol {
namespace _priv {

class glInfo {
public:
    enum Code {
        MaxTextureSize = 0,
        MaxCubeMapTextureSize,
        MaxViewPortWidth,
        MaxViewPortHeight,
        MaxVertexAttribs,
        MaxVertexUniformVectors,
        MaxVaryingVectors,
        MaxCombinedTextureImageUnits,
        MaxVertexTextureImageUnits,
        MaxFragmentUniformVectors,
        
        NumInfos,
        InvalidInfo,
    };

    /// setup the info values
    static void Setup();
    /// discard the info values
    static void Discard();
    /// check if object has been setup
    static bool IsValid();
    /// get an integer value
    static int32 Int(Code c);
    
private:
    /// dump general GL info (extensions, constants, ...)
    static void printInfo();
    /// print a GL string, optionally replace spaces with newlines
    static void printString(GLenum glEnum, const char* name, bool replaceSpaceWithNewLine);
    /// print a 1..4 dimensional integer value
    static void printInt(GLenum glEnum, const char* name, int32 dim);

    static int32 intValues[NumInfos];
    static bool isValid;
};

} // namespace _priv
} // namespace Oryol
