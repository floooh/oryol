#pragma once
//-----------------------------------------------------------------------------
/*  #version:7#
    machine generated, do not edit!
*/
#include "Gfx/Setup/ProgramBundleSetup.h"
namespace Oryol {
namespace Shaders {
    class Canvas {
    public:
        static const int32 Texture = 0;
        static ProgramBundleSetup CreateSetup();
    };
    class CRT {
    public:
        static const int32 Resolution = 0;
        static const int32 Canvas = 1;
        static ProgramBundleSetup CreateSetup();
    };
}
}

