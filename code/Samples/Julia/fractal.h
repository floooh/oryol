#pragma once
//------------------------------------------------------------------------------
/**
    A fractal state wrapper for the Julia sample.
*/
#include "Core/Containers/StaticArray.h"
#include "Resource/Id.h"
#include "Resource/ResourceLabel.h"
#include <glm/vec4.hpp>
#include "Julia/shaders.h"

namespace Julia {

class fractal {
public:
    /// constructor
    fractal();
    /// destructor
    ~fractal();

    /// setup the object with size of fractal render targets
    void setup(int w, int h, const glm::vec4& rect, Oryol::Id fsq);
    /// discard the object, call before shutting down Gfx module
    void discard();
    /// return true if object has been setup
    bool isValid() const;
    /// update the fractal render targets
    void update();

    /// the current rectangle in the real/imaginary plane (c0, i0, c1, i1)
    glm::vec4 rect;
    /// the current frame index
    int frameIndex;
    /// the current evaluation result render target (usable as texture)
    Oryol::Id colorTexture;

    Oryol::ResourceLabel label;
    Oryol::Id fractalDrawState;
    Oryol::StaticArray<Oryol::Id, 2> fractalTexture;
    Oryol::Id colorDrawState;
    Oryol::Shaders::Julia::VSParams vsParams;
    Oryol::Shaders::Julia::FSParams fsParams;
};

} // namespace Julia