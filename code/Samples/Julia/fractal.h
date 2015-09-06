#pragma once
//------------------------------------------------------------------------------
/**
    A fractal state wrapper for the Julia sample.
*/
#include "Core/Containers/StaticArray.h"
#include "Resource/Id.h"
#include "Resource/ResourceLabel.h"
#include "glm/vec4.hpp"
#include "Julia/shaders.h"

namespace Julia {

class fractal {
public:
    /// constructor
    fractal();
    /// destructor
    ~fractal();

    /// setup the object with size of fractal render targets
    void setup(int w, int h, const glm::vec4& rect, const glm::vec2& pos, Oryol::Id fsq);
    /// discard the object, call before shutting down Gfx module
    void discard();
    /// return true if object has been setup
    bool isValid() const;
    /// update the fractal render targets
    void update();

    /// number of frames until state reset
    int cycleCount = 10 * 60;
    /// the current rectangle in the real/imaginary plane (c0, i0, c1, i1)
    glm::vec4 rect;
    /// the 'position' of the Julia set within the Mandelbrot set
    glm::vec2 pos;
    /// the current frame index
    int frameIndex;
    /// the current evaluation result render target (usable as texture)
    Oryol::Id colorTexture;

    Oryol::ResourceLabel label;
    Oryol::Id fractalDrawState;
    Oryol::StaticArray<Oryol::Id, 2> fractalTexture;
    Oryol::StaticArray<Oryol::Id, 2> fractalTextureBundle;
    Oryol::Id colorDrawState;
    Oryol::StaticArray<Oryol::Id, 2> colorTextureBundle;
    Oryol::Shaders::Julia::VSParams fractalVSParams;
    Oryol::Shaders::Julia::FSParams fractalFSParams;
};

} // namespace Julia
