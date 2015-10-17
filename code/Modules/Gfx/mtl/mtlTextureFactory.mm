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
#include "Gfx/Core/displayMgr.h"
#include "Gfx/Resource/resourcePools.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
mtlTextureFactory::mtlTextureFactory() :
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
mtlTextureFactory::~mtlTextureFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
mtlTextureFactory::Setup(const gfxPointers& ptrs) {
    o_assert_dbg(!this->isValid);
    this->isValid = true;
    this->pointers = ptrs;
}

//------------------------------------------------------------------------------
void
mtlTextureFactory::Discard() {
    o_assert_dbg(this->isValid);
    this->pointers = gfxPointers();
    this->isValid = false;
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

    if (nil != tex.mtlTex) {
        ORYOL_OBJC_RELEASE(tex.mtlTex);
        tex.mtlTex = nil;
    }
    if (nil != tex.mtlSamplerState) {
        ORYOL_OBJC_RELEASE(tex.mtlSamplerState);
        tex.mtlSamplerState = nil;
    }
    if (nil != tex.mtlDepthTex) {
        ORYOL_OBJC_RELEASE(tex.mtlDepthTex);
        tex.mtlDepthTex = nil;
    }
    tex.Clear();
}

//------------------------------------------------------------------------------
ResourceState::Code
mtlTextureFactory::createRenderTarget(texture& tex) {
    o_assert_dbg(nil == tex.mtlTex);
    o_assert_dbg(nil == tex.mtlSamplerState);
    o_assert_dbg(nil == tex.mtlDepthTex);

    const TextureSetup& setup = tex.Setup;
    o_assert_dbg(setup.NumMipMaps == 1);
    o_assert_dbg(setup.Type == TextureType::Texture2D);
    o_assert_dbg(PixelFormat::IsValidRenderTargetColorFormat(setup.ColorFormat));

    // get size of new render target
    int32 width, height;
    texture* sharedDepthProvider = nullptr;
    if (setup.IsRelSizeRenderTarget()) {
        const DisplayAttrs& dispAttrs = this->pointers.displayMgr->GetDisplayAttrs();
        width = int32(dispAttrs.FramebufferWidth * setup.RelWidth);
        height = int32(dispAttrs.FramebufferHeight * setup.RelHeight);
    }
    else if (setup.HasSharedDepth()) {
        // a shared depth-buffer render target, obtain width and height
        // from the original render target
        texture* sharedDepthProvider = this->pointers.texturePool->Lookup(setup.DepthRenderTarget);
        o_assert_dbg(nullptr != sharedDepthProvider);
        width = sharedDepthProvider->textureAttrs.Width;
        height = sharedDepthProvider->textureAttrs.Height;
    }
    else {
        width = setup.Width;
        height = setup.Height;
    }
    o_assert_dbg((width > 0) && (height > 0));

    // create the color texture
    MTLTextureDescriptor* texDesc = [[MTLTextureDescriptor alloc] init];
    texDesc.textureType = MTLTextureType2D;
    texDesc.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead;
    texDesc.pixelFormat = mtlTypes::asRenderTargetColorFormat(setup.ColorFormat);
    if (MTLPixelFormatInvalid == texDesc.pixelFormat) {
        o_warn("mtlTextureFactory: not a renderable pixel format!\n");
        return ResourceState::Failed;
    }
    texDesc.width = width;
    texDesc.height = height;
    texDesc.depth = 1;
    texDesc.mipmapLevelCount = 1;
    texDesc.arrayLength = 1;
    texDesc.sampleCount = 1;
    texDesc.resourceOptions = MTLResourceStorageModePrivate;
    texDesc.cpuCacheMode = MTLCPUCacheModeDefaultCache;
    texDesc.storageMode = MTLStorageModePrivate;
    tex.mtlTex = [this->pointers.renderer->mtlDevice newTextureWithDescriptor:texDesc];
    o_assert(nil != tex.mtlTex);

    // create option depth texture
    if (setup.HasDepth()) {
        if (setup.HasSharedDepth()) {
            // shared depth buffer
            o_assert_dbg(sharedDepthProvider && sharedDepthProvider->mtlDepthTex);
            tex.mtlDepthTex = sharedDepthProvider->mtlDepthTex;
        }
        else {
            // create depth buffer texture
            o_assert_dbg(PixelFormat::IsValidRenderTargetDepthFormat(setup.DepthFormat));
            o_assert_dbg(PixelFormat::None != setup.DepthFormat);
            texDesc.pixelFormat = mtlTypes::asRenderTargetDepthFormat(setup.DepthFormat);
            tex.mtlDepthTex = [this->pointers.renderer->mtlDevice newTextureWithDescriptor:texDesc];
            o_assert(nil != tex.mtlDepthTex);
        }
    }

    // create sampler state for color texture
    this->createSamplerState(tex);
    o_assert_dbg(nil != tex.mtlSamplerState);

    // setup texture attributes
    TextureAttrs attrs;
    attrs.Locator = setup.Locator;
    attrs.Type = TextureType::Texture2D;
    attrs.ColorFormat = setup.ColorFormat;
    attrs.DepthFormat = setup.DepthFormat;
    attrs.TextureUsage = Usage::Immutable;
    attrs.Width = setup.Width;
    attrs.Height = setup.Height;
    attrs.NumMipMaps = 1;
    attrs.IsRenderTarget = true;
    attrs.HasDepthBuffer = setup.HasDepth();
    attrs.HasSharedDepthBuffer = setup.HasSharedDepth();
    tex.textureAttrs = attrs;

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
ResourceState::Code
mtlTextureFactory::createFromPixelData(texture& tex, const void* data, int32 size) {
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
    texDesc.usage = MTLTextureUsageShaderRead;
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
    tex.mtlTex = [this->pointers.renderer->mtlDevice newTextureWithDescriptor:texDesc];
    o_assert(nil != tex.mtlTex);

    // copy data bytes into texture
    const uint8* srcPtr = (const uint8*) data;
    const int numFaces = setup.Type == TextureType::TextureCube ? 6 : 1;
    for (int faceIndex = 0; faceIndex < numFaces; faceIndex++) {
        for (int mipIndex = 0; mipIndex < setup.NumMipMaps; mipIndex++) {
            int mipWidth = std::max(setup.Width >> mipIndex, 1);
            int mipHeight = std::max(setup.Height >> mipIndex, 1);
            // special case PVRTC formats: bytesPerRow must be 0
            int bytesPerRow = 0;
            if (!PixelFormat::IsPVRTC(setup.ColorFormat)) {
                bytesPerRow = PixelFormat::RowPitch(setup.ColorFormat, mipWidth);
            }
            MTLRegion region = MTLRegionMake2D(0, 0, mipWidth, mipHeight);
            [tex.mtlTex replaceRegion:region
                mipmapLevel:mipIndex
                slice:faceIndex
                withBytes:srcPtr+setup.ImageOffsets[faceIndex][mipIndex]
                bytesPerRow:bytesPerRow
                bytesPerImage:0];
        }
    }

    // create sampler object
    this->createSamplerState(tex);
    o_assert(nil != tex.mtlSamplerState);

    // setup texture attributes
    TextureAttrs attrs;
    attrs.Locator = setup.Locator;
    attrs.Type = setup.Type;
    attrs.ColorFormat = setup.ColorFormat;
    attrs.TextureUsage = Usage::Immutable;
    attrs.Width = setup.Width;
    attrs.Height = setup.Height;
    attrs.NumMipMaps = setup.NumMipMaps;
    tex.textureAttrs = attrs;

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
mtlTextureFactory::createSamplerState(texture& tex) {
    o_assert_dbg(nil == tex.mtlSamplerState);

    MTLSamplerDescriptor* desc = [[MTLSamplerDescriptor alloc] init];
    desc.sAddressMode = mtlTypes::asSamplerAddressMode(tex.Setup.Sampler.WrapU);
    desc.tAddressMode = mtlTypes::asSamplerAddressMode(tex.Setup.Sampler.WrapV);
    if (TextureType::Texture3D == tex.Setup.Type) {
        desc.rAddressMode = mtlTypes::asSamplerAddressMode(tex.Setup.Sampler.WrapW);
    }
    desc.minFilter = mtlTypes::asSamplerMinMagFilter(tex.Setup.Sampler.MinFilter);
    desc.magFilter = mtlTypes::asSamplerMinMagFilter(tex.Setup.Sampler.MagFilter);
    desc.mipFilter = mtlTypes::asSamplerMipFilter(tex.Setup.Sampler.MinFilter);
    desc.lodMinClamp = 0.0f;
    desc.lodMaxClamp = FLT_MAX;
    desc.maxAnisotropy = 1;
    desc.normalizedCoordinates = YES;
    tex.mtlSamplerState = [this->pointers.renderer->mtlDevice newSamplerStateWithDescriptor:desc];
    o_assert(nil != tex.mtlSamplerState);
}

} // namespace _priv
} // namespace Oryol
