#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::ImageDataAttrs
    @brief describe offsets and sizes of image surfaces
*/
#include "Core/Types.h"
#include "Core/Containers/StaticArray.h"
#include "Gfx/Core/GfxConfig.h"

namespace Oryol {

class ImageDataAttrs {
public:
    /// constructor
    ImageDataAttrs() :
    NumFaces(0),
    NumMipMaps(0) {
        for (auto& offsets : this->Offsets) {
            offsets.Fill(0);
        }
        for (auto& sizes : this->Sizes) {
            sizes.Fill(0);
        }
    };

    /// number of faces
    int32 NumFaces;
    /// number of mipmaps
    int32 NumMipMaps;
    /// pixel data mipmap image offsets
    StaticArray<StaticArray<int32, GfxConfig::MaxNumTextureMipMaps>, GfxConfig::MaxNumTextureFaces> Offsets;
    /// pixel data mipmap image sizes
    StaticArray<StaticArray<int32, GfxConfig::MaxNumTextureMipMaps>, GfxConfig::MaxNumTextureFaces> Sizes;
};

} // namespace Oryol
