#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::RasterizerState
    @ingroup Gfx
    @brief rasterizer state flags
*/
#include "Core/Types.h"
#include "Gfx/Core/Enums.h"

namespace Oryol {
    
class RasterizerState {
public:
    union {
        #pragma pack(push,1)
        struct {
            uint8 CullFaceEnabled : 1;
            uint8 DepthOffsetEnabled : 1;
            uint8 ScissorTestEnabled : 1;
            uint8 DitherEnabled : 1;
            uint8 MultisampleEnabled : 1;
            Face::Code CullFace : 3;
        };
        #pragma pack(pop)
        uint8 Hash;
    };

    /// constructor
    RasterizerState() {
        static_assert(sizeof(RasterizerState) == 1, "sizeof(RasterizerState) is not 1, bitfield packing problem?");
        this->Hash = 0;
        this->CullFaceEnabled = false;
        this->DepthOffsetEnabled = false;
        this->ScissorTestEnabled = false;
        this->DitherEnabled = true;
        this->MultisampleEnabled = true;
        this->CullFace = Face::Back;
    }
    /// equality
    bool operator==(const RasterizerState& rhs) const {
        return this->Hash == rhs.Hash;
    };
    /// inequality
    bool operator!=(const RasterizerState& rhs) const {
        return this->Hash != rhs.Hash;
    };
};

} // namespace Oryol
