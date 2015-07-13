#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::BlendState
    @ingroup Gfx
    @brief describe alpha blending state
*/
#include "Core/Types.h"
#include "Gfx/Core/Enums.h"

namespace Oryol {
    
class BlendState {
public:
    union {
        #pragma pack(push,1)
        struct {
            uint64 BlendEnabled:1;
            BlendFactor::Code SrcFactorRGB:5;
            BlendFactor::Code DstFactorRGB:5;
            BlendOperation::Code OpRGB:3;
            BlendFactor::Code SrcFactorAlpha:5;
            BlendFactor::Code DstFactorAlpha:5;
            BlendOperation::Code OpAlpha:3;
            PixelChannel::Mask ColorWriteMask:4;
            PixelFormat::Code ColorFormat : 5;
            PixelFormat::Code DepthFormat : 5;
        };
        #pragma pack(pop)
        /// hash code from merged state
        uint64 Hash;
    };

    /// constructor
    BlendState() {
        static_assert(sizeof(BlendState) == 8, "sizeof(BlendState) is not 8, bitfield packing problem?");
        this->Hash = 0;
        this->BlendEnabled = false;
        this->SrcFactorRGB = BlendFactor::One;
        this->DstFactorRGB = BlendFactor::Zero;
        this->OpRGB = BlendOperation::Add;
        this->SrcFactorAlpha = BlendFactor::One;
        this->DstFactorAlpha = BlendFactor::Zero;
        this->OpAlpha = BlendOperation::Add;
        this->ColorWriteMask = PixelChannel::RGBA;
        this->ColorFormat = PixelFormat::RGBA8;
        this->DepthFormat = PixelFormat::D24S8;        
    };
    
    /// equality
    bool operator==(const BlendState& rhs) const {
        return this->Hash == rhs.Hash;
    };
    /// inequality
    bool operator!=(const BlendState& rhs) const {
        return this->Hash != rhs.Hash;
    };
};

} // namespace Oryol