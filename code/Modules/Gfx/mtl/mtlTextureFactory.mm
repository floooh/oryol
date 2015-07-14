//------------------------------------------------------------------------------
//  mtlTextureFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtlTextureFactory.h"
#include "mtl_impl.h"
#include "mtlTypes.h"
#include "Core/Assertion.h"
#include "Gfx/Resource/texture.h"
#include "Gfx/Core/renderer.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
mtlTextureFactory::mtlTextureFactory() :
renderer(nullptr),
displayManager(nullptr),
texPool(nullptr),
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
mtlTextureFactory::~mtlTextureFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
mtlTextureFactory::Setup(class renderer* rendr, displayMgr* displayMgr, texturePool* texPool_) {
    o_assert_dbg(!this->isValid);
    o_assert_dbg(nullptr != rendr);
    o_assert_dbg(nullptr != displayMgr);
    o_assert_dbg(nullptr != texPool_);

    this->isValid = true;
    this->renderer = rendr;
    this->displayManager = displayMgr;
    this->texPool = texPool_;
}

//------------------------------------------------------------------------------
void
mtlTextureFactory::Discard() {
    o_assert_dbg(this->isValid);

    this->isValid = false;
    this->renderer = nullptr;
    this->displayManager = nullptr;
    this->texPool = nullptr;
}

//------------------------------------------------------------------------------
bool
mtlTextureFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
ResourceState::Code
mtlTextureFactory::SetupResource(texture& tex) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(!tex.Setup.ShouldSetupFromPixelData());
    o_assert_dbg(!tex.Setup.ShouldSetupFromFile());

    this->renderer->invalidateTextureState();
    if (tex.Setup.ShouldSetupAsRenderTarget()) {
        return this->createRenderTarget(tex);
    }
    else {
        // here would go more ways to create textures without image data
        return ResourceState::InvalidState;
    }
}

//------------------------------------------------------------------------------
ResourceState::Code
mtlTextureFactory::SetupResource(texture& tex, const void* data, int32 size) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(!tex.Setup.ShouldSetupAsRenderTarget());
    o_assert_dbg(!tex.Setup.ShouldSetupFromFile());

    this->renderer->invalidateTextureState();
    if (tex.Setup.ShouldSetupFromPixelData()) {
        return this->createFromPixelData(tex, data, size);
    }
    else {
        // here would go more ways to create textures with image data
        return ResourceState::InvalidState;
    }
}

//------------------------------------------------------------------------------
void
mtlTextureFactory::DestroyResource(texture& tex) {
    o_assert_dbg(this->isValid);

    this->renderer->invalidateTextureState();
    if (nil != tex.mtlTex) {
        ORYOL_OBJC_RELEASE(tex.mtlTex);
        tex.mtlTex = nil;
    }
    if (nil != tex.mtlSamplerState) {
        ORYOL_OBJC_RELEASE(tex.mtlSamplerState);
        tex.mtlSamplerState = nil;
    }
    tex.Clear();
}

//------------------------------------------------------------------------------
ResourceState::Code
mtlTextureFactory::createRenderTarget(texture& tex) {
    o_error("mtlTextureFactory: FIXME!\n");
    return ResourceState::InvalidState;
}

//------------------------------------------------------------------------------
ResourceState::Code
mtlTextureFactory::createFromPixelData(texture& tex, const void* data, int32 size) {
    o_assert_dbg(this->renderer && this->renderer->mtlDevice);
    o_assert_dbg(nil == tex.mtlTex);
    o_assert_dbg(nil == tex.mtlSamplerState);

    const TextureSetup& setup = tex.Setup;
    o_assert_dbg(setup.NumMipMaps > 0);

    if (setup.Type == TextureType::Texture3D) {
        o_warn("mtlTextureFactory: 3d textures not yet implemented!\n");
        return ResourceState::Failed;
    }

    // create metal texture object
    MTLTextureDescriptor* texDesc = [[MTLTextureDescriptor alloc] init];
    texDesc.textureType = mtlTypes::asTextureType(setup.Type);
    texDesc.pixelFormat = mtlTypes::asTextureFormat(setup.ColorFormat);
    if (MTLPixelFormatInvalid == texDesc.pixelFormat) {
        o_warn("mtlTextureFactory: texture pixel format not supported in Metal!\n");
        return ResourceState::Failed;
    }
    texDesc.width  = setup.Width;
    texDesc.height = setup.Height;
    texDesc.depth  = 1;
    texDesc.mipmapLevelCount = setup.NumMipMaps;
    texDesc.arrayLength = 1;
    texDesc.sampleCount = 1;
    texDesc.resourceOptions = MTLResourceStorageModeAuto;
    tex.mtlTex = [this->renderer->mtlDevice newTextureWithDescriptor:texDesc];
    o_assert(nil != tex.mtlTex);

    // copy data bytes into texture
    const uint8* srcPtr = (const uint8*) data;
    const int numFaces = setup.Type == TextureType::TextureCube ? 6 : 1;
    for (int faceIndex = 0; faceIndex < numFaces; faceIndex++) {
        for (int mipIndex = 0; mipIndex < setup.NumMipMaps; mipIndex++) {
            int mipWidth = std::max(setup.Width >> mipIndex, 1);
            int mipHeight = std::max(setup.Height >> mipIndex, 1);
            MTLRegion region = MTLRegionMake2D(0, 0, mipWidth, mipHeight);
            [tex.mtlTex replaceRegion:region
                mipmapLevel:mipIndex
                slice:faceIndex
                withBytes:srcPtr+setup.ImageOffsets[faceIndex][mipIndex]
                bytesPerRow:PixelFormat::RowPitch(setup.ColorFormat, mipWidth)
                bytesPerImage:0];
        }
    }

    // create sampler object
    this->createSamplerState(tex);
    o_assert(nil != tex.mtlSamplerState);

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
mtlTextureFactory::createSamplerState(texture& tex) {
    // FIXME!
}

} // namespace _priv
} // namespace Oryol