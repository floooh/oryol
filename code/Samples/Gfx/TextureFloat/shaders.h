#pragma once
//-----------------------------------------------------------------------------
/*  #version:14#
    machine generated, do not edit!
*/
#include "Gfx/Setup/ProgramBundleSetup.h"
namespace Oryol {
namespace Shaders {
    class Offscreen {
    public:
        static const int32 Time = 0;
        static ProgramBundleSetup CreateSetup();
    };
    class Copy {
    public:
        static const int32 Texture = 0;
        static ProgramBundleSetup CreateSetup();
    };
}
}

