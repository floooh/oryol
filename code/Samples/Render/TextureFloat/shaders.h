#pragma once
//-----------------------------------------------------------------------------
/*  #version:4#
    machine generated, do not edit!
*/
#include "Render/Setup/ProgramBundleSetup.h"
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

