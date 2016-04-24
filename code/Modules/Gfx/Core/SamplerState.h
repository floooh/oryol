#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::SamplerState
    @ingroup Gfx
    @brief wrap texture sampler state
*/
#include "Core/Types.h"
#include "Gfx/Core/Enums.h"

namespace Oryol {

class SamplerState {
public:
    union {
        #pragma pack(push, 1)
        struct {
            /// texture-wrap mode for u-axis
            TextureWrapMode::Code WrapU : 2;
            /// texture-wrap mode for v-axis
            TextureWrapMode::Code WrapV : 2;
            /// texture-wrap mode for w-axis
            TextureWrapMode::Code WrapW : 2;
            /// magnification filter
            TextureFilterMode::Code MagFilter : 3;
            /// minification filter
            TextureFilterMode::Code MinFilter : 3;
        };
        #pragma pack(pop)
        uint16_t Hash;
    };

    /// constructor
    SamplerState() {
        static_assert(sizeof(SamplerState) == 2, "sizeof(SamplerState) is not 2, bitfield packing problem?");
        this->Hash = 0;
        this->WrapU = TextureWrapMode::Repeat;
        this->WrapV = TextureWrapMode::Repeat;
        this->WrapW = TextureWrapMode::Repeat;
        this->MagFilter = TextureFilterMode::Nearest;
        this->MinFilter = TextureFilterMode::Nearest;
    };
    /// equality
    bool operator==(const SamplerState& rhs) const {
        return this->Hash == rhs.Hash;
    };
    /// inequality
    bool operator!=(const SamplerState& rhs) const {
        return this->Hash != rhs.Hash;
    };
};

} // namespace Oryol

