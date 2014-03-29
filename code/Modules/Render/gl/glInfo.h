#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::glInfo
    @brief dump information about the host's GL implementation
*/
#include "Core/Types.h"
#include "Render/gl/gl_decl.h"

namespace Oryol {
namespace Render {

class glInfo {
public:
    /// dump general GL info (extensions, constants, ...)
    static void PrintInfo();
    /// print a GL string, optionally replace spaces with newlines
    static void PrintString(GLenum glEnum, const char* name, bool replaceSpaceWithNewLine);
    /// print a 1..4 dimensional integer value
    static void PrintInt(GLenum glEnum, const char* name, int32 dim);
};

} // namespace Render
} // namespace Oryol
