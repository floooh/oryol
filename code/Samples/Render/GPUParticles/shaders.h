#pragma once
//-----------------------------------------------------------------------------
/*  #version:2#
    machine generated, do not edit!
*/
#include "Render/Setup/ProgramBundleSetup.h"
namespace Oryol {
namespace Shaders {
    class InitParticles {
    public:
        static const int32 BufferDims = 0;
        static const int32 Time = 1;
        static Render::ProgramBundleSetup CreateSetup();
    };
    class DrawParticles {
    public:
        static const int32 ModelViewProjection = 0;
        static const int32 BufferDims = 1;
        static const int32 ParticleState = 2;
        static Render::ProgramBundleSetup CreateSetup();
    };
    class UpdateParticles {
    public:
        static const int32 BufferDims = 0;
        static const int32 PrevState = 1;
        static const int32 NumParticles = 2;
        static Render::ProgramBundleSetup CreateSetup();
    };
}
}

