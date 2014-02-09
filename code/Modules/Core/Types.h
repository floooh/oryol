#pragma once
//------------------------------------------------------------------------------
/**
    @file Core/Types.h
    @brief defines basic data types for Oryol
*/
#include <cstdint>

namespace Oryol {

/// signed 8-bit integer
typedef std::int8_t int8;
/// unsigned 8-bit integer
typedef std::uint8_t uint8;
/// signed 16-bit integer
typedef std::int16_t int16;
/// unsigned 16-bit integer
typedef std::uint16_t uint16;
/// signed 32-bit integer
typedef std::int32_t int32;
/// unsigned 32-bit integer
typedef std::uint32_t uint32;
/// signed 64-bit integer
typedef std::int64_t int64;
/// unsigned 64-bit integer
typedef std::uint64_t uint64;
/// 32-bit floating point number
typedef float float32;
/// 64-bit floating point number
typedef double float64;
/// a signed integer with as many bits as a pointer
typedef std::intptr_t intptr;
/// an unsigned integer with as many bits as a pointer
typedef std::uintptr_t uintptr;
/// unsigned 8-bit character
typedef unsigned char uchar;

/// invalid index constant
static const int32 InvalidIndex = -1;

/// end-of-string indicator
static const int32 EndOfString = -1;

/// end-of-file indicator
static const int32 EndOfStream = -1;

} // namespace Oryol

