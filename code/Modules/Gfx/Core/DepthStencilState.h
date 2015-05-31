#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::DepthStencilState
    @ingroup Gfx
    @brief holds the complete depth and stencil render state
*/
#include "Gfx/Core/Enums.h"
#include "Gfx/Core/StencilState.h"

namespace Oryol {
    
class DepthStencilState {
public:
    /// front-side stencil state
    StencilState StencilFront;
    /// back-side stencil state
    StencilState StencilBack;
    /// common depth-stencil state
    union {
        struct {
            /// depth compare-function
            CompareFunc::Code DepthCmpFunc:5;
            /// depth write enabled flag
            bool DepthWriteEnabled:1;
            /// stencil-enabled flag
            bool StencilEnabled:1;
            /// stencil read-mask
            unsigned int StencilReadMask : 8;
            /// stencil write-mask
            unsigned int StencilWriteMask : 8;
            /// stencil-ref value
            unsigned int StencilRef : 8;
        };
        uint32 StateHash;
    };

    /// constructor
    DepthStencilState() {
        this->StateHash = 0;
        this->DepthCmpFunc = CompareFunc::Always;
        this->DepthWriteEnabled = false;
        this->StencilEnabled = false;
        this->StencilReadMask = 0xFF;
        this->StencilWriteMask = 0xFF;
        this->StencilRef = 0;
    };

    /// equality
    bool operator==(const DepthStencilState& rhs) const {
        return (this->StateHash == rhs.StateHash) &&
               (this->StencilFront == rhs.StencilFront) &&
               (this->StencilBack == rhs.StencilBack);
    };
    /// inequality
    bool operator!=(const DepthStencilState& rhs) const {
        return (this->StateHash != rhs.StateHash) ||
               (this->StencilFront != rhs.StencilFront) ||
               (this->StencilBack != rhs.StencilBack);
    }
};

} // namespace Oryol
