#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::StencilState
    @brief holds stencil-buffer render state for one face side
*/
#include "Core/Types.h"
#include "Render/Core/Enums.h"

namespace Oryol {
namespace Render {
    
class StencilState {
public:

    union {
        struct {
            bool StencilEnabled : 1;
            StencilOp::Code StencilFailOp : 4;
            StencilOp::Code DepthFailOp : 4;
            StencilOp::Code DepthStencilPassOp : 4;
            CompareFunc::Code StencilCmpFunc : 4;
            unsigned int StencilReadMask : 8;
            unsigned int StencilWriteMask : 8;
            unsigned int StencilRef : 8;
        };
        uint64 Hash;
    };

    /// constructor
    StencilState() {
        this->Hash = 0;
        this->StencilEnabled = false;
        this->StencilFailOp = StencilOp::Keep;
        this->DepthFailOp = StencilOp::Keep;
        this->DepthStencilPassOp = StencilOp::Keep;
        this->StencilCmpFunc = CompareFunc::Always;
        this->StencilReadMask = 0xFF;
        this->StencilWriteMask = 0xFF;
        this->StencilRef = 0;
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

} // namespace Render
} // namespace Oryol
