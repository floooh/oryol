#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Sound
    @brief audio module for generated sound effects and short samples
*/
#include "Core/Types.h"

namespace Oryol {

class Sound {
public:
    /// setup the Sound module
    static void Setup(const SoundSetup& setup);
    /// discard the Sound module
    static void Discard();
    /// check if Sound module is valid
    static bool IsValid();

    /// generate new resource label and push on label stack
    static ResourceLabel PushResourceLabel();
    /// push explicit resource label on label stack
    static void PushResourceLabel(ResourceLabel label);
    /// pop resource label from label stack
    static ResourceLabel PopResourceLabel();
    /// create a sound effect resource
    static Id CreateResource(const SoundEffectSetup& setup);
    /// create a sound effect resource with data in stream
    static Id CreateResource(const SoundEffectSetup& setup, const Ptr<Stream>& stream);
    /// create a sound effect resource with raw data
    static Id CreateResource(const SoundEffectSetup& setup, const void* data, int32 size);
    /// lookup a resource id by Locator
    static Id LookupResource(const Locator& locator);
    /// destroy one or several sound resources by matching label
    static void DestroyResources(ResourceLabel label);

    /// play a sound effect
    static void Play(Id snd, int32 loopCount=1, int32 freqShift=0);
};

} // namespace Oryol
