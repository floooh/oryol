#pragma once
//-----------------------------------------------------------------------------
/*  #version:2#
    machine generated, do not edit!
*/
#include "Render/Setup/ProgramBundleSetup.h"
namespace Oryol {
namespace Shaders {
    class Offscreen {
    public:
        static const int32 Time = 0;
        static Render::ProgramBundleSetup CreateSetup();
    };
    class Copy {
    public:
        static const int32 Texture = 0;
        static Render::ProgramBundleSetup CreateSetup();
    };
}
}

