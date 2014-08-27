#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Synth::gpuSynthesizer
    @brief the GPU-driven synthesizer core
    
    This 'renders' sample buffers on the GPU.
*/
#include "Synth/Core/opBundle.h"
#include "Synth/Core/SynthSetup.h"
#include "Resource/Id.h"

namespace Oryol {
namespace Synth {
    
class gpuSynthesizer {
public:
    /// constructor
    gpuSynthesizer();
    /// destructor
    ~gpuSynthesizer();
    
    /// setup the object
    void Setup(const SynthSetup& setupAttrs);
    /// discard the object
    void Discard();
    /// return true if object has been setup
    bool IsValid() const;
    
    /// synthesize!
    void Synthesize(const opBundle& bundle) const;
    
private:
    bool isValid;
    Resource::Id renderTarget;
    Resource::Id drawState;
};
    
} // namespace Synth
} // namespace Oryol