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
    /// constructor
    RasterizerState();
    
    /// equality
    bool operator==(const RasterizerState& rhs) const;
    /// inequality
    bool operator!=(const RasterizerState& rhs) const;
    
    /// enable/disable face culling
    void SetCullFaceEnabled(bool b);
    /// get face culling flag
    bool GetCullFaceEnabled() const;
    /// set cull-face side
    void SetCullFace(Face::Code face);
    /// get cull face side
    Face::Code GetCullFace() const;
    /// enable/disable depth offset
    void SetDepthOffsetEnabled(bool b);
    /// get depth-offset enabled flag
    bool GetDepthOffsetEnabled() const;
    /// enable/disable scissor test
    void SetScissorTestEnabled(bool b);
    /// get scissor test enabled flag
    bool GetScissorTestEnabled() const;
    /// enable/disable dithering
    void SetDitherEnabled(bool b);
    /// get dither enabled flag
    bool GetDitherEnabled() const;
    
    /// get hash code
    uint8 GetHash() const;

    union {
        struct {
            bool cullFaceEnabled : 1;
            bool depthOffsetEnabled : 1;
            bool scissorTestEnabled : 1;
            bool ditherEnabled : 1;
            unsigned int cullFace : 2;
        } bits;
        uint8 hash;
    };
};

//------------------------------------------------------------------------------
inline
RasterizerState::RasterizerState() {
    this->hash = 0;
    this->bits.cullFaceEnabled = false;
    this->bits.depthOffsetEnabled = false;
    this->bits.scissorTestEnabled = false;
    this->bits.ditherEnabled = false;
    this->bits.cullFace = Face::Back;
}

//------------------------------------------------------------------------------
inline bool
RasterizerState::operator==(const RasterizerState& rhs) const {
    return this->hash == rhs.hash;
}

//------------------------------------------------------------------------------
inline bool
RasterizerState::operator!=(const RasterizerState& rhs) const {
    return this->hash != rhs.hash;
}

//------------------------------------------------------------------------------
inline void
RasterizerState::SetCullFaceEnabled(bool b) {
    this->bits.cullFaceEnabled = b;
}

//------------------------------------------------------------------------------
inline bool
RasterizerState::GetCullFaceEnabled() const {
    return this->bits.cullFaceEnabled;
}

//------------------------------------------------------------------------------
inline void
RasterizerState::SetCullFace(Face::Code f) {
    this->bits.cullFace = f;
}

//------------------------------------------------------------------------------
inline Face::Code
RasterizerState::GetCullFace() const {
    return (Face::Code) this->bits.cullFace;
}

//------------------------------------------------------------------------------
inline void
RasterizerState::SetDepthOffsetEnabled(bool b) {
    this->bits.depthOffsetEnabled = b;
}

//------------------------------------------------------------------------------
inline bool
RasterizerState::GetDepthOffsetEnabled() const {
    return this->bits.depthOffsetEnabled;
}

//------------------------------------------------------------------------------
inline void
RasterizerState::SetScissorTestEnabled(bool b) {
    this->bits.scissorTestEnabled = b;
}

//------------------------------------------------------------------------------
inline bool
RasterizerState::GetScissorTestEnabled() const {
    return this->bits.scissorTestEnabled;
}

//------------------------------------------------------------------------------
inline void
RasterizerState::SetDitherEnabled(bool b) {
    this->bits.ditherEnabled = b;
}

//------------------------------------------------------------------------------
inline bool
RasterizerState::GetDitherEnabled() const {
    return this->bits.ditherEnabled;
}

//------------------------------------------------------------------------------
inline uint8
RasterizerState::GetHash() const {
    return this->hash;
}

} // namespace Render
} // namespace Oryol
