#pragma once
//-----------------------------------------------------------------------------
/*  #version:15#
    machine generated, do not edit!
*/
#include "Gfx/Setup/ProgramBundleSetup.h"
namespace Oryol {
namespace Shaders {
    class Lambert {
    public:
        static const int32 ModelViewProjection = 0;
        static const int32 Diffuse = 1;
        static ProgramBundleSetup CreateSetup();
    };
    class Normals {
    public:
        static const int32 ModelViewProjection = 0;
        static ProgramBundleSetup CreateSetup();
    };
}
}

