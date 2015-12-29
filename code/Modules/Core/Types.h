#pragma once
//------------------------------------------------------------------------------
/**
    @file Core/Types.h
    @brief defines basic data types for Oryol
*/
#include <cstdint>

namespace Oryol {

typedef std::int8_t int8;
typedef std::uint8_t uint8;
typedef std::int16_t int16;
typedef std::uint16_t uint16;
typedef std::int32_t int32;
typedef std::uint32_t uint32;
typedef std::int64_t int64;
typedef std::uint64_t uint64;
typedef float float32;
typedef double float64;
typedef std::intptr_t intptr;
typedef std::uintptr_t uintptr;
typedef unsigned char uchar;

static const int32 InvalidIndex = -1;
static const int32 EndOfString = -1;
static const int32 EndOfFile = -1;

} // namespace Oryol

