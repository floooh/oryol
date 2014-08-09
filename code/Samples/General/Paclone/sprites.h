#pragma once
//-----------------------------------------------------------------------------
/*  #version:2#
    machine generated, do not edit!
*/
#include "Core/Types.h"
namespace Paclone {
struct Sheet {
    static const Oryol::int32 Width{64};
    static const Oryol::int32 Height{256};
    static const Oryol::int32 NumBytes{65536};
    static const Oryol::uint32 Pixels[16384];
    enum SpriteId {
        PacRight,
        PacDown,
        PacLeft,
        PacUp,
        Pinky,
        Blinky,
        Clyde,
        Inky,
        Wall_0_111_0,
        Wall_1_010_1,
        Wall_0_110_0,
        Wall_0_011_0,
        Wall_0_010_1,
        Wall_1_010_0,
        Wall_1_111_1,
        Wall_0_010_0,
        Wall_1_011_1,
        Wall_1_110_1,
        Wall_1_111_0,
        Wall_0_111_1,
        Wall_0_011_1,
        Wall_0_110_1,
        Wall_1_011_0,
        Wall_1_110_0,
        Pill,
        Energizer,

        NumSprites,
        InvalidSprite
    };
    class Anim {
    public:
        enum Code {
            None,
            Loop,
            PingPong,
            Clamp,
        };
    };
    static const struct sprite {
        SpriteId id;
        Oryol::int32 X;
        Oryol::int32 Y;
        Oryol::int32 W;
        Oryol::int32 H;
        Oryol::int32 NumFrames;
        Anim::Code AnimType;
        Oryol::uint32 Mask;
    } Sprite[NumSprites];
};
}

