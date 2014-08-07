#pragma once
//-----------------------------------------------------------------------------
/*  #version:2#
    machine generated, do not edit!
*/
#include "Core/Types.h"
namespace Oryol {
struct Paclone {
    static const uint32 Width;
    static const uint32 Height;
    static const uint32 Pixels[12288];
    enum Sprite {
        PacRight,
        PacDown,
        PacLeft,
        PacUp,
        Pinky,
        Blinky,
        Clyde,
        Inky,
        Wall_000_111_000,
        Wall_010_010_010,
        Wall_000_110_000,
        Wall_000_011_000,
        Wall_000_010_010,
        Wall_010_010_000,
        Wall_010_111_010,
        Wall_000_010_000,
        Pill,

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
    static const struct {
        int32 X;
        int32 Y;
        int32 W;
        int32 H;
        int32 NumFrames;
        Anim::Code AnimType;
    } Sprites[NumSprites];
};
}

