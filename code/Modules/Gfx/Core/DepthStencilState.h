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
            uint16 DepthWriteEnabled:1;
            /// stencil-enabled flag
            uint16 StencilEnabled:1;
            /// stencil read-mask
            uint16 StencilReadMask : 8;
            /// stencil write-mask
            uint16 StencilWriteMask : 8;
            /// stencil-ref value
            uint16 StencilRef : 8;
        };
        uint32 Hash;
    };

    /// constructor
    DepthStencilState() {
        static_assert(sizeof(DepthStencilState) == 8, "sizeof(DepthStencilState) is not 8, bitfield packing problem?");
        this->Hash = 0;
        this->DepthCmpFunc = CompareFunc::Always;
        this->DepthWriteEnabled = false;
        this->StencilEnabled = false;
        this->StencilReadMask = 0xFF;
        this->StencilWriteMask = 0xFF;
        this->StencilRef = 0;
    };

    /// equality
    bool operator==(const DepthStencilState& rhs) const {
        return (this->Hash == rhs.Hash) &&
               (this->StencilFront == rhs.StencilFront) &&
               (this->StencilBack == rhs.StencilBack);
    };
    /// inequality
    bool operator!=(const DepthStencilState& rhs) const {
        return (this->Hash != rhs.Hash) ||
               (this->StencilFront != rhs.StencilFront) ||
               (this->StencilBack != rhs.StencilBack);
    }
};

} // namespace Oryol
