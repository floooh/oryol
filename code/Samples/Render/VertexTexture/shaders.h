#pragma once
//-----------------------------------------------------------------------------
/*  #version:2#
    machine generated, do not edit!
*/
#include "Render/Setup/ProgramBundleSetup.h"
namespace Oryol {
namespace Shaders {
    class Plane {
    public:
        static const int32 ModelViewProjection = 0;
        static const int32 Texture = 1;
        static Render::ProgramBundleSetup CreateSetup();
    };
    class Plasma {
    public:
        static const int32 Time = 0;
        static Render::ProgramBundleSetup CreateSetup();
    };
}
}

