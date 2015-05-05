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
        static const int32 Model = 1;
        static const int32 LightDir = 2;
        static const int32 LightColor = 3;
        static const int32 MatDiffuse = 4;
        static ProgramBundleSetup CreateSetup();
    };
    class Phong {
    public:
        static const int32 ModelViewProjection = 0;
        static const int32 Model = 1;
        static const int32 LightDir = 2;
        static const int32 EyePos = 3;
        static const int32 LightColor = 4;
        static const int32 MatDiffuse = 5;
        static const int32 MatSpecular = 6;
        static const int32 MatSpecularPower = 7;
        static ProgramBundleSetup CreateSetup();
    };
    class Normals {
    public:
        static const int32 ModelViewProjection = 0;
        static ProgramBundleSetup CreateSetup();
    };
}
}

