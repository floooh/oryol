#pragma once
//-----------------------------------------------------------------------------
/*  #version:4#
    machine generated, do not edit!
*/
#include "Render/Setup/ProgramBundleSetup.h"
namespace Oryol {
namespace Shaders {
    class Main {
    public:
        static const int32 ModelViewProjection = 0;
        static const int32 Texture = 1;
        static ProgramBundleSetup CreateSetup();
    };
    class RenderTarget {
    public:
        static const int32 ModelViewProjection = 0;
        static ProgramBundleSetup CreateSetup();
    };
}
}

