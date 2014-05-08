#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::VertexWriter
    @brief efficiently write packed vertex components
*/
#include "Core/Types.h"
#include "Render/Core/Enums.h"

namespace Oryol {
namespace Render {
    
class VertexWriter {
public:
    /// write 1D generic vertex component with run-time pack-format selection
    static uint8* Write(uint8* dst, VertexFormat::Code fmt, float32 x);
    /// write 2D generic vertex component with run-time pack-format selection
    static uint8* Write(uint8* dst, VertexFormat::Code fmt, float32 x, float32 y);
    /// write 3D generic vertex component with run-time pack-format selection
    static uint8* Write(uint8* dst, VertexFormat::Code fmt, float32 x, float32 y, float32 z);
    /// write 4D generic vertex component with run-time pack-format selection
    static uint8* Write(uint8* dst, VertexFormat::Code fmt, float32 x, float32 y, float32 z, float32 w);
};
    
} // namespace Render
} // namespace Oryol