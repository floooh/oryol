#pragma once
//-----------------------------------------------------------------------------
/*  #version:2#
    machine generated, do not edit!
*/
#include "Core/Types.h"
namespace Paclone {
struct Sheet {
    static const Oryol::int32 Width{128};
    static const Oryol::int32 Height{128};
    static const Oryol::int32 NumBytes{65536};
    static const Oryol::uint32 Pixels[16384];
    enum SpriteId {
        Space,
        TestWall,
        TestPill,
        TestPacman,
        TestGhost0,
        TestGhost1,
        TestGhost2,
        TestGhost3,

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
    static const SpriteId CharMap[256];
    static const struct sprite {
        SpriteId id;
        Oryol::int32 X;
        Oryol::int32 Y;
        Oryol::int32 W;
        Oryol::int32 H;
        Oryol::int32 NumFrames;
        Anim::Code AnimType;
        Oryol::uint32 Mask;
        Oryol::uint8 Char;
    } Sprite[NumSprites];
};
}

