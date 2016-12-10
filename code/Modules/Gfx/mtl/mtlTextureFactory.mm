//------------------------------------------------------------------------------
//  mtlTextureFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtlTextureFactory.h"
#include "mtl_impl.h"
#include "mtlTypes.h"
#include "Core/Assertion.h"
#include "Gfx/Resource/resource.h"
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
    // there should be mostly less then 64 different samplers
    this->samplerCache.Reserve(64);
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
    return this->createTexture(tex, nullptr, 0);
}

//------------------------------------------------------------------------------
ResourceState::Code
mtlTextureFactory::SetupResource(texture& tex, const void* data, int size) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(!tex.Setup.RenderTarget);
    return this->createTexture(tex, data, size);
}

//------------------------------------------------------------------------------
void
mtlTextureFactory::DestroyResource(texture& tex) {
    o_assert_dbg(this->isValid);
    for (auto& mtlTex : tex.mtlTextures) {
        if (nil != mtlTex) {
            this->pointers.renderer->releaseDeferred(mtlTex);
        }
    }
    if (nil != tex.mtlDepthTex) {
        this->pointers.renderer->releaseDeferred(tex.mtlDepthTex);
    }
    if (nil != tex.mtlMSAATex) {
        this->pointers.renderer->releaseDeferred(tex.mtlMSAATex);
    }
    if (nil != tex.mtlSamplerState) {
        this->releaseSamplerState(tex);
    }
    tex.Clear();
}

//------------------------------------------------------------------------------
ResourceState::Code
mtlTextureFactory::createTexture(texture& tex, const void* data, int size) {
    o_assert_dbg(nil == tex.mtlTextures[0]);
    o_assert_dbg(nil == tex.mtlSamplerState);

    const TextureSetup& setup = tex.Setup;
    #if ORYOL_DEBUG
    o_assert(setup.NumMipMaps > 0);
    if (setup.RenderTarget) {
        o_assert(PixelFormat::IsValidRenderTargetColorFormat(setup.ColorFormat));
    }
    #endif

    // create one or two texture objects
    tex.numSlots = Usage::Immutable == setup.TextureUsage ? 1 : 2;

    // create metal texture object
    MTLTextureDescriptor* texDesc = [[MTLTextureDescriptor alloc] init];
    texDesc.textureType = mtlTypes::asTextureType(setup.Type);
    if (setup.RenderTarget) {
        texDesc.pixelFormat = mtlTypes::asRenderTargetColorFormat(setup.ColorFormat);
    }
    else {
        texDesc.pixelFormat = mtlTypes::asTextureFormat(setup.ColorFormat);
    }
    if (MTLPixelFormatInvalid == texDesc.pixelFormat) {
        o_warn("mtlTextureFactory: texture pixel format not supported in Metal!\n");
        return ResourceState::Failed;
    }
    texDesc.width  = setup.Width;
    texDesc.height = setup.Height;
    if (setup.Type == TextureType::Texture3D) {
        texDesc.depth  = setup.Depth;
    }
    else {
        texDesc.depth = 1;
    }
    texDesc.mipmapLevelCount = setup.NumMipMaps;
    if (setup.Type == TextureType::TextureArray) {
        texDesc.arrayLength = setup.Depth;
    }
    else {
        texDesc.arrayLength = 1;
    }
    texDesc.usage = MTLTextureUsageShaderRead;
    if (setup.RenderTarget) {
        texDesc.resourceOptions = MTLResourceStorageModePrivate;
        texDesc.cpuCacheMode = MTLCPUCacheModeDefaultCache;
        texDesc.storageMode = MTLStorageModePrivate;
        texDesc.usage |= MTLTextureUsageRenderTarget;
    }

    const int numFaces = setup.Type == TextureType::TextureCube ? 6 : 1;
    const int numSlices = setup.Type == TextureType::TextureArray ? setup.Depth : 1;
    for (int slotIndex = 0; slotIndex < tex.numSlots; slotIndex++) {
        tex.mtlTextures[slotIndex] = [this->pointers.renderer->mtlDevice newTextureWithDescriptor:texDesc];
        o_assert(nil != tex.mtlTextures[slotIndex]);

        // copy optional data bytes into texture
        if (data) {
            o_assert_dbg(size > 0);
            const uint8* srcPtr = (const uint8*) data;
            for (int faceIndex = 0; faceIndex < numFaces; faceIndex++) {
                for (int mipIndex = 0; mipIndex < setup.ImageData.NumMipMaps; mipIndex++) {
                    o_assert_dbg(mipIndex <= setup.NumMipMaps);
                    int mipWidth = std::max(setup.Width >> mipIndex, 1);
                    int mipHeight = std::max(setup.Height >> mipIndex, 1);
                    // special case PVRTC formats: bytesPerRow must be 0
                    int bytesPerRow = 0;
                    int bytesPerImage = 0;
                    if (!PixelFormat::IsPVRTC(setup.ColorFormat)) {
                        bytesPerRow = PixelFormat::RowPitch(setup.ColorFormat, mipWidth);
                    }
                    MTLRegion region;
                    if (setup.Type == TextureType::Texture3D) {
                        int mipDepth = std::max(setup.Depth >> mipIndex, 1);
                        region = MTLRegionMake3D(0, 0, 0, mipWidth, mipHeight, mipDepth);
                        bytesPerImage = bytesPerRow * mipHeight;
                        if (bytesPerImage < 4096) {
                            o_warn("FIXME: bytesPerImage < 4096, this must be fixed in code by copying the data\n"
                                   "into a temp buffer with the right image alignment!\n");
                        }
                    }
                    else {
                        region = MTLRegionMake2D(0, 0, mipWidth, mipHeight);
                    }
                    for (int sliceIndex = 0; sliceIndex < numSlices; sliceIndex++) {
                        const int mtlSliceIndex = setup.Type == TextureType::TextureCube ? faceIndex : sliceIndex;
                        const int sliceDataOffset = sliceIndex * PixelFormat::ImagePitch(setup.ColorFormat, mipWidth, mipHeight);
                        [tex.mtlTextures[slotIndex] replaceRegion:region
                            mipmapLevel:mipIndex
                            slice:mtlSliceIndex
                            withBytes:srcPtr + setup.ImageData.Offsets[faceIndex][mipIndex] + sliceDataOffset
                            bytesPerRow:bytesPerRow
                            bytesPerImage:bytesPerImage];
                    }
                }
            }
        }
    }

    if (setup.RenderTarget) {
        // prepare texture descriptor for optional MSAA and depth texture
        texDesc.textureType = MTLTextureType2D;
        texDesc.depth = 1;
        texDesc.mipmapLevelCount = 1;
        texDesc.arrayLength = 1;

        // create optional MSAA texture where offscreen rendering will go to,
        // the 'default' Metal texture will serve as resolve-texture
        if (setup.SampleCount > 1) {
            texDesc.textureType = MTLTextureType2DMultisample;
            texDesc.sampleCount = setup.SampleCount;
            tex.mtlMSAATex = [this->pointers.renderer->mtlDevice newTextureWithDescriptor:texDesc];
            o_assert(nil != tex.mtlMSAATex);
        }

        // create optional depth buffer texture (may be MSAA)
        if (setup.HasDepth()) {
            o_assert_dbg(setup.RenderTarget);
            o_assert_dbg(PixelFormat::IsValidRenderTargetDepthFormat(setup.DepthFormat));
            o_assert_dbg(PixelFormat::None != setup.DepthFormat);
            texDesc.pixelFormat = mtlTypes::asRenderTargetDepthFormat(setup.DepthFormat);
            tex.mtlDepthTex = [this->pointers.renderer->mtlDevice newTextureWithDescriptor:texDesc];
            o_assert(nil != tex.mtlDepthTex);
        }
    }

    // create sampler object
    this->createSamplerState(tex);
    o_assert(nil != tex.mtlSamplerState);

    // setup texture attributes
    TextureAttrs attrs;
    attrs.Locator       = setup.Locator;
    attrs.Type          = setup.Type;
    attrs.ColorFormat   = setup.ColorFormat;
    attrs.DepthFormat   = setup.DepthFormat;
    attrs.SampleCount   = setup.SampleCount;
    attrs.TextureUsage  = setup.TextureUsage;
    attrs.Width         = setup.Width;
    attrs.Height        = setup.Height;
    attrs.Depth         = setup.Depth;
    attrs.NumMipMaps    = setup.NumMipMaps;
    attrs.IsRenderTarget = setup.RenderTarget;
    attrs.HasDepthBuffer = setup.HasDepth();
    tex.textureAttrs = attrs;

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
mtlTextureFactory::createSamplerState(texture& tex) {
    o_assert_dbg(nil == tex.mtlSamplerState);

    // check if an identical state already exists
    const int cacheIndex = this->samplerCache.FindIndex(tex.Setup.Sampler.Hash);
    if (InvalidIndex != cacheIndex) {
        // re-use existing sampler-state object
        SamplerCacheItem& item = this->samplerCache.ValueAtIndex(cacheIndex);
        item.useCount++;
        tex.mtlSamplerState = item.mtlSamplerState;
        o_dbg("mtlTextureFactory: re-use MTLSamplerState at %p\n", tex.mtlSamplerState);
    }
    else {
        // create new sampler-state object
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
        o_dbg("mtlTextureFactory: create new MTLSamplerState at %p\n", tex.mtlSamplerState);

        // add new cache entry
        SamplerCacheItem item;
        item.mtlSamplerState = tex.mtlSamplerState;
        item.useCount = 1;
        this->samplerCache.Add(tex.Setup.Sampler.Hash, item);
    }
}

//------------------------------------------------------------------------------
void
mtlTextureFactory::releaseSamplerState(texture& tex) {
    o_assert_dbg(nil != tex.mtlSamplerState);

    // find cache entry (linear search is ok, since total number of
    // sampler state will be low
    for (int index = this->samplerCache.Size()-1; index >= 0; index--) {
        SamplerCacheItem& item = this->samplerCache.ValueAtIndex(index);
        if (item.mtlSamplerState == tex.mtlSamplerState) {
            o_assert_dbg(item.useCount > 0);
            if (--item.useCount == 0) {
                o_dbg("mtlTextureFactory: destroy MTLSamplerState at %p\n", tex.mtlSamplerState);
                this->pointers.renderer->releaseDeferred(item.mtlSamplerState);
                this->samplerCache.EraseIndex(index);
            }
            break;
        }
    }
}

} // namespace _priv
} // namespace Oryol
