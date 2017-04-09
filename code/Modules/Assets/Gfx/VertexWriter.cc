//------------------------------------------------------------------------------
//  VertexWriter.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "VertexWriter.h"
#include "Core/Assertion.h"
#include "glm/glm.hpp"
#include "glm/gtc/packing.hpp"

namespace Oryol {

//------------------------------------------------------------------------------
uint8_t*
VertexWriter::Write(uint8_t* dst, VertexFormat::Code fmt, float x) {
    o_assert_dbg(dst && (VertexFormat::Float == fmt));
    float* p = (float*) dst;
    *p++ = x;
    return (uint8_t*) p;
}

//------------------------------------------------------------------------------
uint8_t*
VertexWriter::Write(uint8_t* dst, VertexFormat::Code fmt, float x, float y) {
    o_assert_dbg(dst);
    if (VertexFormat::Float2 == fmt) {
        float* p = (float*) dst;
        *p++ = x; *p++ = y;
        return (uint8_t*) p;
    }
    else if (VertexFormat::Short2 == fmt) {
        glm::i16vec2 packed(glm::clamp(glm::vec2(x, y), -32768.0f, 32767.0f));
        int16_t* p = (int16_t*) dst;
        *p++ = packed.x;
        *p++ = packed.y;
        return (uint8_t*) p;
    }
    else if (VertexFormat::Short2N == fmt) {
        glm::i16vec2 packed(glm::round(glm::clamp(glm::vec2(x, y), -1.0f, 1.0f) * 32767.0f));
        int16_t* p = (int16_t*) dst;
        *p++ = packed.x;
        *p++ = packed.y;
        return (uint8_t*) p;
    }
    else {
        o_assert2_dbg(false, "Unsupported format!");
        return nullptr;
    }
}

//------------------------------------------------------------------------------
uint8_t*
VertexWriter::Write(uint8_t* dst, VertexFormat::Code fmt, float x, float y, float z, float w) {
    o_assert_dbg(dst);
    if (VertexFormat::Float4 == fmt) {
        float* p = (float*) dst;
        *p++ = x; *p++ = y; *p++ = z; *p++ = w;
        return (uint8_t*) p;
    }
    else if (VertexFormat::Byte4 == fmt) {
        glm::i8vec4 packed(glm::clamp(glm::vec4(x, y, z, w), -128.0f, 127.0f));
        int8_t* p = (int8_t*) dst;
        *p++ = packed.x;
        *p++ = packed.y;
        *p++ = packed.z;
        *p++ = packed.w;
        return (uint8_t*) p;
    }
    else if (VertexFormat::Byte4N == fmt) {
        glm::i8vec4 packed(glm::round(glm::clamp(glm::vec4(x, y, z, w), -1.0f, 1.0f) * 127.0f));
        int8_t* p = (int8_t*) dst;
        *p++ = packed.x;
        *p++ = packed.y;
        *p++ = packed.z;
        *p++ = packed.w;
        return (uint8_t*) p;
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
        int16_t* p = (int16_t*) dst;
        *p++ = packed.x;
        *p++ = packed.y;
        *p++ = packed.z;
        *p++ = packed.w;
        return (uint8_t*) p;
    }
    else if (VertexFormat::Short4N == fmt) {
        glm::i16vec4 packed(glm::round(glm::clamp(glm::vec4(x, y, z, w), -1.0f, 1.0f) * 32767.0f));
        int16_t* p = (int16_t*) dst;
        *p++ = packed.x;
        *p++ = packed.y;
        *p++ = packed.z;
        *p++ = packed.w;
        return (uint8_t*) p;
    }
    else if (VertexFormat::UInt10_2N == fmt) {
        glm::uint32 packed = glm::packUnorm3x10_1x2(glm::vec4(x, y, z, w));
        uint32_t* p = (uint32_t*) dst;
        *p++ = packed;
        return (uint8_t*) p;
    }
    else {
        o_assert2_dbg(false, "Unsupported format!");
        return nullptr;
    }
}

//------------------------------------------------------------------------------
uint8_t*
VertexWriter::Write(uint8_t* dst, VertexFormat::Code fmt, float x, float y, float z) {
    if (VertexFormat::Float3 == fmt) {
        float* p = (float*) dst;
        *p++ = x; *p++ = y; *p++ = z;
        return (uint8_t*) p;
    }
    else {
        // hmm, try to extend to 4 components
        return Write(dst, fmt, x, y, z, 0.0f);
    }
}
    
} // namespace Oryol

