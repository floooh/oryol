#pragma once
//------------------------------------------------------------------------------
/**
    @brief fast-hash implementation (https://code.google.com/p/fast-hash/)

    NOTE: contains modifications to better fit into Oryol (inlining, 
    namespace and types), any bugs are probably caused by this :)
*/

/* The MIT License

Copyright (C) 2012 Zilong Tan (eric.zltan@gmail.com)

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use, copy,
modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include "Core/Types.h"

namespace Oryol {
namespace _priv {

// Compression function for Merkle-Damgard construction.
// This function is generated using the framework provided.
//------------------------------------------------------------------------------
inline uint64
fasthash_mix(uint64 h) {
    h ^= h >> 23;
    h *= 0x2127599bf4325c37ULL;
    h ^= h >> 47;
    return h;
}

//------------------------------------------------------------------------------
inline uint64 
fasthash64(const void *buf, size_t len, uint64 seed)
{
    const uint64    m = 0x880355f21e6d1965ULL;
    const uint64 *pos = (const uint64 *)buf;
    const uint64 *end = pos + (len / 8);
    const unsigned char *pos2;
    uint64 h = seed ^ (len * m);
    uint64 v;

    while (pos != end) {
        v = *pos++;
        h ^= fasthash_mix(v);
        h *= m;
    }

    pos2 = (const unsigned char*)pos;
    v = 0;

    switch (len & 7) {
    case 7: v ^= (uint64)pos2[6] << 48;
    case 6: v ^= (uint64)pos2[5] << 40;
    case 5: v ^= (uint64)pos2[4] << 32;
    case 4: v ^= (uint64)pos2[3] << 24;
    case 3: v ^= (uint64)pos2[2] << 16;
    case 2: v ^= (uint64)pos2[1] << 8;
    case 1: v ^= (uint64)pos2[0];
        h ^= fasthash_mix(v);
        h *= m;
    }

    return fasthash_mix(h);
}

//------------------------------------------------------------------------------
inline uint32 
fasthash32(const void *buf, size_t len, uint32 seed)
{
    // the following trick converts the 64-bit hashcode to Fermat
    // residue, which shall retain information from both the higher
    // and lower parts of hashcode.
    uint64 h = fasthash64(buf, len, seed);
    return uint32(h - (h >> 32));
}

} // namespace _priv
} // namespace Oryol