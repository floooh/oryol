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
            bool StencilEnabled : 1;
            StencilOp::Code FailOp : 4;
            StencilOp::Code DepthFailOp : 4;
            StencilOp::Code PassOp : 4;
            CompareFunc::Code CmpFunc : 4;
            unsigned int ReadMask : 8;
            unsigned int WriteMask : 8;
            unsigned int Ref : 8;
        };
        uint64 Hash;
    };

    /// constructor
    StencilState() {
        this->Hash = 0;
        this->StencilEnabled = false;
        this->FailOp = StencilOp::Keep;
        this->DepthFailOp = StencilOp::Keep;
        this->PassOp = StencilOp::Keep;
        this->CmpFunc = CompareFunc::Always;
        this->ReadMask = 0xFF;
        this->WriteMask = 0xFF;
        this->Ref = 0;
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
