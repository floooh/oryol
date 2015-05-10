#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Wave
    @ingroup Assets
    @brief helper class to generate sample wave data
    
    The wave generator generates a sine, square, triangle and
    saw-tooth sample from a sample position usually provided 
    by a Freq object. The sample pos parameter is expected
    to be normalized to 1 Hz (so, pos is 0.0 at start, and 1.0 at
    a full second).
*/
#include "Core/Types.h"
#include <cmath>
#include <cstdlib>

namespace Oryol {

class Wave {
public:
    /// get next sine wave value at normalized sample position
    static float32 Sine(float32 p);
    /// get next square wave value at normalized sample position
    static float32 Square(float32 p);
    /// get next triangle wave value at normalized sample position
    static float32 Triangle(float32 p);
    /// get sawtooth wave value at normalized sample position
    static float32 SawTooth(float32 p);
    /// get inverse-sawtooth wave value at normalized sample position
    static float32 InvSawTooth(float32 p);
    /// return random noise
    static float32 RandNoise();
};

//------------------------------------------------------------------------------
inline float32
Wave::Sine(float32 p) {
    return std::sin(p * M_PI * 2.0f);
}

//------------------------------------------------------------------------------
inline float32
Wave::Square(float32 p) {
    return (p < 0.5f) ? -1.0f : 1.0f;
}

//------------------------------------------------------------------------------
inline float32
Wave::Triangle(float32 p) {
    if (p < 0.5f) {
        return (p * 4.0f) - 1.0f;
    }
    else {
        return 1.0f - ((p - 0.5f) * 4.0f);
    }
}

//------------------------------------------------------------------------------
inline float32
Wave::SawTooth(float32 p) {
    return 1.0f - (p * 2.0f);
}

//------------------------------------------------------------------------------
inline float32
Wave::InvSawTooth(float32 p) {
    return (p * 2.0f) - 1.0f;
}

//------------------------------------------------------------------------------
inline float32
Wave::RandNoise() {
    return (float32(std::rand() & 0xffff) / 32768.0f) - 1.0f;
}

} // namespace Oryol
