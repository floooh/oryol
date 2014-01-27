#pragma once
//------------------------------------------------------------------------------
/**
    @file Core/Types.h

    Basic datatypes for Oryol.
*/
namespace Oryol
{

// basic typedefs
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

// invalid index constant
static const int32 InvalidIndex = -1;

// end-of-string indicator
static const int32 EndOfString = -1;

} // namespace Oryol

