#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::FullscreenQuadBuilder
    @ingroup Assets
    @brief helper class to build a fullscreen quad mesh.

    FIXME: probably makes sense to rewrite this to a single triangle

    Positions (vertex attr name: "position")
    0:  -1.0,+1.0,0.0
    1:  +1.0,+1.0,0.0
    2:  -1.0,-1.0,0.0
    3:  +1.0,-1.0,0.0

    UV Coords (vertex attr name: "texcoord0", FlipV == false:
    0:  0.0, 1.0
    1:  1.0, 1.0
    2:  0.0, 0.0
    3:  1.0, 0.0

    UV Coords, FlipV == true:
    0:  0.0, 0.0
    1:  1.0, 0.0
    2:  0.0, 1.0
    3:  1.0, 1.0
*/
#include "Gfx/GfxTypes.h"

namespace Oryol {

class FullscreenQuadBuilder {
public:
    struct Result {
        BufferDesc VertexBufferDesc;
        PipelineDesc PipelineDesc;  // blueprint pipeline desc
        int NumElements = 4;        // rendered as triangle strip
    };
    bool flipV = false;

    // texture v coord is (1.0 - y) 
    FullscreenQuadBuilder& FlipV(bool b) { flipV = b; return *this; }
    bool FlipV() const { return flipV; }

    // populate and return Result object
    Result Build();
};

} // namespace Oryol
