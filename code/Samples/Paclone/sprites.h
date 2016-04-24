#pragma once
//-----------------------------------------------------------------------------
/*  #version:7#
    machine generated, do not edit!
*/
#include "Core/Types.h"
namespace Paclone {
struct Sheet {
    static const int Width{128};
    static const int Height{256};
    static const int NumBytes{131072};
    static const uint32_t Pixels[32768];
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
        int X;
        int Y;
        int W;
        int H;
        int NumFrames;
        Anim::Code AnimType;
        uint8_t Char;
    } Sprite[NumSprites];
};
}

