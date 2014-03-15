//------------------------------------------------------------------------------
//  VertexWriter.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "VertexWriter.h"
#include "Core/Macros.h"
#include "glm/glm.hpp"

namespace Oryol {
namespace Render {

//------------------------------------------------------------------------------
uint8*
VertexWriter::Write(uint8* dst, VertexFormat::Code fmt, float32 x) {
    o_assert_dbg(dst && (VertexFormat::Float == fmt));
    float32* p = (float32*) dst;
    *p++ = x;
    return (uint8*) p;
}

//------------------------------------------------------------------------------
uint8*
VertexWriter::Write(uint8* dst, VertexFormat::Code fmt, float32 x, float32 y) {
    o_assert_dbg(dst);
    if (VertexFormat::Float2 == fmt) {
        float32* p = (float32*) dst;
        *p++ = x; *p++ = y;
        return (uint8*) p;
    }
    else if (VertexFormat::Short2 == fmt) {
        glm::i16vec2 packed(glm::clamp(glm::vec2(x, y), -32768.0f, 32767.0f));
        int16* p = (int16*) dst;
        *p++ = packed.x;
        *p++ = packed.y;
        return (uint8*) p;
    }
    else if (VertexFormat::Short2N == fmt) {
        glm::i16vec2 packed(glm::round(glm::clamp(glm::vec2(x, y), -1.0f, 1.0f) * 32767.0f));
        int16* p = (int16*) dst;
        *p++ = packed.x;
        *p++ = packed.y;
        return (uint8*) p;
    }
    else {
        o_assert2_dbg(false, "Unsupported format!");
        return nullptr;
    }
}

//------------------------------------------------------------------------------
uint8*
VertexWriter::Write(uint8* dst, VertexFormat::Code fmt, float32 x, float32 y, float32 z) {
    o_assert_dbg(dst && (VertexFormat::Float3 == fmt));
    float32* p = (float32*) dst;
    *p++ = x; *p++ = y; *p++ = z;
    return (uint8*) p;
}

//------------------------------------------------------------------------------
uint8*
VertexWriter::Write(uint8* dst, VertexFormat::Code fmt, float32 x, float32 y, float32 z, float32 w) {
    o_assert_dbg(dst);
    if (VertexFormat::Float4 == fmt) {
        float32* p = (float32*) dst;
        *p++ = x; *p++ = y; *p++ = z; *p++ = w;
        return (uint8*) p;
    }
    else if (VertexFormat::Byte4 == fmt) {
        glm::i8vec4 packed(glm::clamp(glm::vec4(x, y, z, w), -128.0f, 127.0f));
        int8* p = (int8*) dst;
        *p++ = packed.x;
        *p++ = packed.y;
        *p++ = packed.z;
        *p++ = packed.w;
        return (uint8*) p;
    }
    else if (VertexFormat::Byte4N == fmt) {
        glm::i8vec4 packed(glm::round(glm::clamp(glm::vec4(x, y, z, w), -1.0f, 1.0f) * 127.0f));
        int8* p = (int8*) dst;
        *p++ = packed.x;
        *p++ = packed.y;
        *p++ = packed.z;
        *p++ = packed.w;
        return (uint8*) p;        
    }
    else if (VertexFormat::UByte4 == fmt) {
        glm::u8vec4 packed(glm::clamp(glm::vec4(x, y, z, w), 0.0f, 255.0f));
        *dst++ = packed.x;
        *dst++ = packed.y;
        *dst++ = packed.z;
        *dst++ = packed.w;
        return dst;
    }
    else if (VertexFormat::UByte4N == fmt) {
        glm::u8vec4 packed(glm::round(glm::clamp(glm::vec4(x, y, z, w), 0.0f, 1.0f) * 255.0f));
        *dst++ = packed.x;
        *dst++ = packed.y;
        *dst++ = packed.z;
        *dst++ = packed.w;
        return dst;
    }
    else if (VertexFormat::Short4 == fmt) {
        glm::i16vec4 packed(glm::clamp(glm::vec4(x, y, z, w), -32768.0f, 32767.0f));
        int16* p = (int16*) dst;
        *p++ = packed.x;
        *p++ = packed.y;
        *p++ = packed.z;
        *p++ = packed.w;
        return (uint8*) p;
    }
    else if (VertexFormat::Short4N == fmt) {
        glm::i16vec4 packed(glm::round(glm::clamp(glm::vec4(x, y, z, w), -1.0f, 1.0f) * 32767.0f));
        int16* p = (int16*) dst;
        *p++ = packed.x;
        *p++ = packed.y;
        *p++ = packed.z;
        *p++ = packed.w;
        return (uint8*) p;
    }
    else {
        o_assert2_dbg(false, "Unsupported format!");
        return nullptr;
    }
}

} // namespace Render
} // namespace Oryol

