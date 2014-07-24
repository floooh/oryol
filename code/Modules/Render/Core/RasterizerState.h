#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::RasterizerState
    @brief rasterizer state flags
*/
#include "Core/Types.h"
#include "Render/Core/Enums.h"

namespace Oryol {
namespace Render {
    
class RasterizerState {
public:
    union {
        struct {
            bool CullFaceEnabled : 1;
            bool DepthOffsetEnabled : 1;
            bool ScissorTestEnabled : 1;
            bool DitherEnabled : 1;
            Face::Code CullFace : 2;
        };
        uint8 Hash;
    };

    /// constructor
    RasterizerState() {
        this->Hash = 0;
        this->CullFaceEnabled = false;
        this->DepthOffsetEnabled = false;
        this->ScissorTestEnabled = false;
        this->DitherEnabled = false;
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

} // namespace Render
} // namespace Oryol
