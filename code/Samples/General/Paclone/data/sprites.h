#pragma once
//-----------------------------------------------------------------------------
/*  #version:2#
    machine generated, do not edit!
*/
#include "Core/Types.h"
namespace Paclone {
struct Sheet {
    static const Oryol::int32 Width{128};
    static const Oryol::int32 Height{256};
    static const Oryol::int32 NumBytes{131072};
    static const Oryol::uint32 Pixels[32768];
    enum SpriteId {
        Space,
        TestWall,
        Pill,
        Energizer,
        PacmanNoDir,
        PacmanRight,
        PacmanDown,
        PacmanUp,
        PacmanLeft,
        PacmanDead,
        BlinkyNoDir,
        BlinkyRight,
        BlinkyLeft,
        BlinkyUp,
        BlinkyDown,
        PinkyNoDir,
        PinkyRight,
        PinkyLeft,
        PinkyUp,
        PinkyDown,
        InkyNoDir,
        InkyRight,
        InkyLeft,
        InkyUp,
        InkyDown,
        ClydeNoDir,
        ClydeRight,
        ClydeLeft,
        ClydeUp,
        ClydeDown,
        FrightenedGhost,
        FlashingGhost,
        EyesRight,
        EyesLeft,
        EyesUp,
        EyesDown,
        DoubleLeft,
        DoubleRight,
        DoubleUp,
        DoubleBottom,
        DoubleUpLeft,
        DoubleUpRight,
        DoubleBottomLeft,
        DoubleBottomRight,
        DoubleTUpLeft,
        DoubleTUpRight,
        DoubleTLeftUp,
        DoubleTLeftDown,
        DoubleTRightUp,
        DoubleTRightDown,
        WallLeft,
        WallRight,
        WallUp,
        WallBottom,
        InnerCorner0,
        InnerCorner1,
        InnerCorner2,
        InnerCorner3,
        OuterCorner0,
        OuterCorner1,
        OuterCorner2,
        OuterCorner3,
        HouseVert,
        HouseHori,
        HouseTopLeft,
        HouseTopRight,
        HouseBottomLeft,
        HouseBottomRight,
        HouseDoor,
        P200,
        P400,
        P800,
        P1600,

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

