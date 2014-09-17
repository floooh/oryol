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
        struct {
            bool CullFaceEnabled : 1;
            bool DepthOffsetEnabled : 1;
            bool ScissorTestEnabled : 1;
            bool DitherEnabled : 1;
            bool MultisampleEnabled : 1;
            Face::Code CullFace : 3;
        };
        uint8 Hash;
    };

    /// constructor
    RasterizerState() {
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
