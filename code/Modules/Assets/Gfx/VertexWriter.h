#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::VertexWriter
    @ingroup Assets
    @brief efficiently write packed vertex components
*/
#include "Core/Types.h"
#include "Gfx/GfxTypes.h"

namespace Oryol {
    
class VertexWriter {
public:
    /// write 1D generic vertex component with run-time pack-format selection
    static uint8_t* Write(uint8_t* dst, VertexFormat::Code fmt, float x);
    /// write 2D generic vertex component with run-time pack-format selection
    static uint8_t* Write(uint8_t* dst, VertexFormat::Code fmt, float x, float y);
    /// write 3D generic vertex component with run-time pack-format selection
    static uint8_t* Write(uint8_t* dst, VertexFormat::Code fmt, float x, float y, float z);
    /// write 4D generic vertex component with run-time pack-format selection
    static uint8_t* Write(uint8_t* dst, VertexFormat::Code fmt, float x, float y, float z, float w);
};
    
} // namespace Oryol