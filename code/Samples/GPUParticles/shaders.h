#pragma once
//-----------------------------------------------------------------------------
/*  #version:17#
    machine generated, do not edit!
*/
#include "Gfx/Setup/ProgramBundleSetup.h"
namespace Oryol {
namespace Shaders {
    class InitParticles {
    public:
        static const int32 BufferDims = 0;
        static const int32 Time = 1;
        static ProgramBundleSetup CreateSetup();
    };
    class DrawParticles {
    public:
        static const int32 ModelViewProjection = 0;
        static const int32 BufferDims = 1;
        static const int32 ParticleState = 2;
        static ProgramBundleSetup CreateSetup();
    };
    class UpdateParticles {
    public:
        static const int32 BufferDims = 0;
        static const int32 PrevState = 1;
        static const int32 NumParticles = 2;
        static ProgramBundleSetup CreateSetup();
    };
}
}

