#pragma once
//-----------------------------------------------------------------------------
/*  #version:1#
    machine generated, do not edit!
*/
#include "Render/Setup/ProgramBundleSetup.h"
namespace Oryol {
namespace Shaders {
    class Main {
    public:
        static const int32 ModelViewProjection = 0;
        static const int32 Texture = 1;
        static Render::ProgramBundleSetup CreateSetup();
    };
    class RenderTarget {
    public:
        static const int32 ModelViewProjection = 0;
        static Render::ProgramBundleSetup CreateSetup();
    };
}
}

