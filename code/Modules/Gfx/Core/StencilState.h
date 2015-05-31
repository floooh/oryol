#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::StencilState
    @ingroup Gfx
    @brief holds stencil-buffer render state for one face side
*/
#include "Core/Types.h"
#include "Gfx/Core/Enums.h"

namespace Oryol {
    
class StencilState {
public:

    union {
        struct {
            StencilOp::Code FailOp : 4;
            StencilOp::Code DepthFailOp : 4;
            StencilOp::Code PassOp : 4;
            CompareFunc::Code CmpFunc : 4;
        };
        uint16 Hash;
    };

    /// constructor
    StencilState() {
        this->Hash = 0;
        this->FailOp = StencilOp::Keep;
        this->DepthFailOp = StencilOp::Keep;
        this->PassOp = StencilOp::Keep;
        this->CmpFunc = CompareFunc::Always;
    }
    
    /// equality
    bool operator==(const StencilState& rhs) const {
        return this->Hash == rhs.Hash;
    };
    /// inequality
    bool operator!=(const StencilState& rhs) const {
        return this->Hash != rhs.Hash;
    };
};

} // namespace Oryol
