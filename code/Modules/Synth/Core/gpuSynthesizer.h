#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::_priv::gpuSynthesizer
    @ingroup _priv
    @brief the GPU-driven synthesizer core
    
    This 'renders' sample buffers on the GPU.
*/
#include "Synth/Core/opBundle.h"
#include "Synth/Core/SynthSetup.h"
#include "Resource/Id.h"

namespace Oryol {
namespace _priv {
    
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
    Id renderTarget;
    Id drawState;
};
    
} // namespace _priv
} // namespace Oryol