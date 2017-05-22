//------------------------------------------------------------------------------
//  mtlFactory.mm
//------------------------------------------------------------------------------
#include "Pre.h"
#include "mtl_impl.h"
#include "mtlFactory.h"
#include "mtlTypes.h"
#include "Gfx/private/renderer.h"
#include "Gfx/private/resource.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
void
mtlFactory::setup(const gfxPointers& ptrs) {
    o_assert(!this->isValid);
    gfxFactoryBase::setup(ptrs);
    this->samplerCache.Reserve(64);
    this->releaseQueue.Reserve(256);
}

//------------------------------------------------------------------------------
void
mtlFactory::discard() {
    o_assert(this->isValid);
    this->releaseQueue.Clear();
    this->samplerCache.Clear();
    gfxFactoryBase::discard();
}

//------------------------------------------------------------------------------
ResourceState::Code
mtlFactory::initMesh(mesh& msh, const void* data, int size) {
    o_assert_dbg(this->isValid);
    if (msh.Setup.ShouldSetupFullScreenQuad()) {
        return this->initFullscreenQuad(msh);
    }
    else {
        return this->initStdMesh(msh, data, size);
    }
}

//------------------------------------------------------------------------------
void
mtlFactory::destroyMesh(mesh& msh) {
    o_assert_dbg(this->isValid);
    for (auto& buf : msh.buffers) {
        for (auto& mtlBuf : buf.mtlBuffers) {
            if (nil != mtlBuf) {
                this->releaseDeferred(mtlBuf);
            }
        }
    }
    msh.Clear();
}

//------------------------------------------------------------------------------
id<MTLBuffer>
mtlFactory::createBuffer(const void* data, uint32 bufSize, Usage::Code usage) {
    o_assert_dbg(bufSize > 0);
    MTLResourceOptions options = mtlTypes::asBufferResourceOptions(usage);
    id<MTLBuffer> buf;
    if (data) {
        buf = [this->pointers.renderer->mtlDevice newBufferWithBytes:data length:bufSize options:options];
    }
    else {
        buf = [this->pointers.renderer->mtlDevice newBufferWithLength:bufSize options:options];
    }
    return buf;
}

//------------------------------------------------------------------------------
ResourceState::Code
mtlFactory::initStdMesh(mesh& msh, const void* data, int size) {
    o_assert_dbg(nil == msh.buffers[mesh::vb].mtlBuffers[0]);
    o_assert_dbg(nil == msh.buffers[mesh::ib].mtlBuffers[0]);
    o_assert_dbg(1 == msh.buffers[mesh::vb].numSlots);
    o_assert_dbg(1 == msh.buffers[mesh::ib].numSlots);

    VertexBufferAttrs vbAttrs;
    vbAttrs.NumVertices = msh.Setup.NumVertices;
    vbAttrs.BufferUsage = msh.Setup.VertexUsage;
    vbAttrs.Layout = msh.Setup.Layout;
    msh.vertexBufferAttrs = vbAttrs;

    IndexBufferAttrs ibAttrs;
    ibAttrs.NumIndices = msh.Setup.NumIndices;
    ibAttrs.Type = msh.Setup.IndicesType;
    ibAttrs.BufferUsage = msh.Setup.IndexUsage;
    msh.indexBufferAttrs = ibAttrs;

    msh.numPrimGroups = msh.Setup.NumPrimitiveGroups();
    o_assert_dbg(msh.numPrimGroups < GfxConfig::MaxNumPrimGroups);
    for (int i = 0; i < msh.numPrimGroups; i++) {
        msh.primGroups[i] = msh.Setup.PrimitiveGroup(i);
    }

    const uint8* ptr = (const uint8*) data;

    // create vertex buffer
    if (msh.Setup.NumVertices > 0) {
        const auto& vbAttrs = msh.vertexBufferAttrs;
        const int vbSize = vbAttrs.NumVertices * msh.Setup.Layout.ByteSize();
        msh.buffers[mesh::vb].numSlots = Usage::Immutable == vbAttrs.BufferUsage ? 1 : 2;
        const uint8* vertices = nullptr;
        if (data) {
            o_assert((msh.Setup.VertexDataOffset >= 0) && (size > 0));
            vertices = ptr + msh.Setup.VertexDataOffset;
            o_assert_dbg((ptr + size) >= (vertices + vbSize));
        }
        for (uint8 slotIndex = 0; slotIndex < msh.buffers[mesh::vb].numSlots; slotIndex++) {
            msh.buffers[mesh::vb].mtlBuffers[slotIndex] = this->createBuffer(vertices, vbSize, vbAttrs.BufferUsage);
            o_assert_dbg(nil != msh.buffers[mesh::vb].mtlBuffers[slotIndex]);
        }
    }

    // create optional index buffer
    if (msh.indexBufferAttrs.Type != IndexType::None) {
        const auto& ibAttrs = msh.indexBufferAttrs;
        const int ibSize = ibAttrs.NumIndices * IndexType::ByteSize(ibAttrs.Type);
        msh.buffers[mesh::ib].numSlots = Usage::Immutable == ibAttrs.BufferUsage ? 1 : 2;
        const uint8* indices = nullptr;
        if (data) {
            o_assert((msh.Setup.IndexDataOffset >= 0) && (size > 0));
            indices = ptr + msh.Setup.IndexDataOffset;
            o_assert_dbg((ptr + size) >= (indices + ibSize));
        }
        for (uint8 slotIndex = 0; slotIndex < msh.buffers[mesh::ib].numSlots; slotIndex++) {
            msh.buffers[mesh::ib].mtlBuffers[slotIndex] = this->createBuffer(indices, ibSize, ibAttrs.BufferUsage);
            o_assert_dbg(nil != msh.buffers[mesh::ib].mtlBuffers[slotIndex]);
        }
    }
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
ResourceState::Code
mtlFactory::initFullscreenQuad(mesh& msh) {

    VertexBufferAttrs vbAttrs;
    vbAttrs.NumVertices = 4;
    vbAttrs.BufferUsage = Usage::Immutable;
    vbAttrs.Layout.Add(VertexAttr::Position, VertexFormat::Float3);
    vbAttrs.Layout.Add(VertexAttr::TexCoord0, VertexFormat::Float2);
    msh.vertexBufferAttrs = vbAttrs;

    IndexBufferAttrs ibAttrs;
    ibAttrs.NumIndices = 6;
    ibAttrs.Type = IndexType::Index16;
    ibAttrs.BufferUsage = Usage::Immutable;
    msh.indexBufferAttrs = ibAttrs;

    msh.numPrimGroups = 1;
    msh.primGroups[0] = PrimitiveGroup(0, 6);

    const float topV = msh.Setup.FullScreenQuadFlipV ? 0.0f : 1.0f;
    const float botV = msh.Setup.FullScreenQuadFlipV ? 1.0f : 0.0f;
    float vertices[] = {
        -1.0f, +1.0f, 0.0f, 0.0f, topV,     // top-left corner
        +1.0f, +1.0f, 0.0f, 1.0f, topV,     // top-right corner
        +1.0f, -1.0f, 0.0f, 1.0f, botV,     // bottom-right corner
        -1.0f, -1.0f, 0.0f, 0.0f, botV,     // bottom-left corner
    };

    uint16_t indices[] = {
        0, 2, 1,            // topleft -> bottomright -> topright
        0, 3, 2,            // topleft -> bottomleft -> bottomright
    };

    msh.buffers[mesh::vb].mtlBuffers[0] = this->createBuffer(vertices, sizeof(vertices), Usage::Immutable);
    msh.buffers[mesh::ib].mtlBuffers[0] = this->createBuffer(indices, sizeof(indices), Usage::Immutable);

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
ResourceState::Code
mtlFactory::initTexture(texture& tex, const void* data, int size) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(nil == tex.mtlTextures[0]);
    o_assert_dbg(nil == tex.mtlSamplerState);

    const TextureSetup& setup = tex.Setup;
    #if ORYOL_DEBUG
    o_assert(setup.NumMipMaps > 0);
    if (setup.IsRenderTarget) {
        o_assert(PixelFormat::IsValidRenderTargetColorFormat(setup.ColorFormat));
    }
    #endif

    // create one or two texture objects
    tex.numSlots = Usage::Immutable == setup.TextureUsage ? 1 : 2;

    // create metal texture object
    MTLTextureDescriptor* texDesc = [[MTLTextureDescriptor alloc] init];
    texDesc.textureType = mtlTypes::asTextureType(setup.Type);
    if (setup.IsRenderTarget) {
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
    if (setup.IsRenderTarget) {
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

    if (setup.IsRenderTarget) {
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
            o_assert_dbg(setup.IsRenderTarget);
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
    attrs.IsRenderTarget = setup.IsRenderTarget;
    attrs.HasDepthBuffer = setup.HasDepth();
    tex.textureAttrs = attrs;

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
mtlFactory::destroyTexture(texture& tex) {
    o_assert_dbg(this->isValid);
    for (auto& mtlTex : tex.mtlTextures) {
        if (nil != mtlTex) {
            this->releaseDeferred(mtlTex);
        }
    }
    if (nil != tex.mtlDepthTex) {
        this->releaseDeferred(tex.mtlDepthTex);
    }
    if (nil != tex.mtlMSAATex) {
        this->releaseDeferred(tex.mtlMSAATex);
    }
    if (nil != tex.mtlSamplerState) {
        this->releaseSamplerState(tex);
    }
    tex.Clear();
}

//------------------------------------------------------------------------------
void
mtlFactory::createSamplerState(texture& tex) {
    o_assert_dbg(nil == tex.mtlSamplerState);

    // check if an identical state already exists
    const int cacheIndex = this->samplerCache.FindIndex(tex.Setup.Sampler.Hash);
    if (InvalidIndex != cacheIndex) {
        // re-use existing sampler-state object
        SamplerCacheItem& item = this->samplerCache.ValueAtIndex(cacheIndex);
        item.useCount++;
        tex.mtlSamplerState = item.mtlSamplerState;
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

        // add new cache entry
        SamplerCacheItem item;
        item.mtlSamplerState = tex.mtlSamplerState;
        item.useCount = 1;
        this->samplerCache.Add(tex.Setup.Sampler.Hash, item);
    }
}

//------------------------------------------------------------------------------
void
mtlFactory::releaseSamplerState(texture& tex) {
    o_assert_dbg(nil != tex.mtlSamplerState);

    // find cache entry (linear search is ok, since total number of
    // sampler state will be low
    for (int index = this->samplerCache.Size()-1; index >= 0; index--) {
        SamplerCacheItem& item = this->samplerCache.ValueAtIndex(index);
        if (item.mtlSamplerState == tex.mtlSamplerState) {
            o_assert_dbg(item.useCount > 0);
            if (--item.useCount == 0) {
                this->releaseDeferred(item.mtlSamplerState);
                this->samplerCache.EraseIndex(index);
            }
            break;
        }
    }
}

//------------------------------------------------------------------------------
ResourceState::Code
mtlFactory::initShader(shader& shd) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(nil == shd.mtlVertexShaderLibrary);
    o_assert_dbg(nil == shd.mtlFragmentShaderLibrary);

    const ShaderLang::Code slang = ShaderLang::Metal;
    const ShaderSetup& setup = shd.Setup;
    const void* vsLibraryByteCode = nullptr;
    uint32_t vsLibraryByteCodeSize = 0;
    setup.VertexShaderByteCode(slang, vsLibraryByteCode, vsLibraryByteCodeSize);
    o_assert_dbg(vsLibraryByteCode && (vsLibraryByteCodeSize > 0));
    const void* fsLibraryByteCode = nullptr;
    uint32_t fsLibraryByteCodeSize = 0;
    setup.FragmentShaderByteCode(slang, fsLibraryByteCode, fsLibraryByteCodeSize);
    o_assert_dbg(fsLibraryByteCode && (fsLibraryByteCodeSize > 0));

    // first create the shader library (one library per bundle)
    NSError* err = 0;
    dispatch_data_t vsLibData = dispatch_data_create(vsLibraryByteCode, vsLibraryByteCodeSize, NULL, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
    shd.mtlVertexShaderLibrary = [this->pointers.renderer->mtlDevice newLibraryWithData:vsLibData error:&err];
    o_assert(nil == err);
    dispatch_data_t fsLibData = dispatch_data_create(fsLibraryByteCode, fsLibraryByteCodeSize, NULL, DISPATCH_DATA_DESTRUCTOR_DEFAULT);
    shd.mtlFragmentShaderLibrary = [this->pointers.renderer->mtlDevice newLibraryWithData:fsLibData error:&err];
    o_assert(nil == err);

    // create vertex and fragment shader function objects
    const StringAtom& vsName = setup.VertexShaderFunc(slang);
    shd.mtlVertexShader = [shd.mtlVertexShaderLibrary newFunctionWithName:[NSString stringWithUTF8String:vsName.AsCStr()]];
    const StringAtom& fsName = setup.FragmentShaderFunc(slang);
    shd.mtlFragmentShader = [shd.mtlFragmentShaderLibrary newFunctionWithName:[NSString stringWithUTF8String:fsName.AsCStr()]];

    // get the vertex shader attribute locations
    shd.vsAttrIndices.Fill(InvalidIndex);
    for (MTLVertexAttribute* mtlAttr in shd.mtlVertexShader.vertexAttributes) {
        if (mtlAttr.active) {
            VertexAttr::Code attr = VertexAttr::FromString(mtlAttr.name.UTF8String);
            if (VertexAttr::InvalidVertexAttr != attr) {
                shd.vsAttrIndices[attr] = int(mtlAttr.attributeIndex);
            }
            else {
                o_error("Invalid vertex attribute name '%s'!\n", mtlAttr.name.UTF8String);
            }
        }
    }
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
mtlFactory::destroyShader(shader& shd) {
    o_assert_dbg(this->isValid);
    if (nil != shd.mtlVertexShader) {
        this->releaseDeferred(shd.mtlVertexShader);
    }
    if (nil != shd.mtlFragmentShader) {
        this->releaseDeferred(shd.mtlFragmentShader);
    }
    if (nil != shd.mtlVertexShaderLibrary) {
        this->releaseDeferred(shd.mtlVertexShaderLibrary);
    }
    if (nil != shd.mtlFragmentShaderLibrary) {
        this->releaseDeferred(shd.mtlFragmentShaderLibrary);
    }
    shd.Clear();
}

//------------------------------------------------------------------------------
ResourceState::Code
mtlFactory::initPipeline(pipeline& pip) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(nil == pip.mtlRenderPipelineState);
    o_assert_dbg(nil == pip.mtlDepthStencilState);

    gfxFactoryBase::initPipeline(pip);
    o_assert_dbg(pip.shd);

    // create vertex-descriptor object
    MTLVertexDescriptor* vtxDesc = [MTLVertexDescriptor vertexDescriptor];
    for (int meshSlotIndex = 0; meshSlotIndex < GfxConfig::MaxNumInputMeshes; meshSlotIndex++) {
        // NOTE: vertex buffers are located after constant buffers
        const int vbSlotIndex = meshSlotIndex + GfxConfig::MaxNumUniformBlocksPerStage;
        const VertexLayout& meshLayout = pip.Setup.Layouts[meshSlotIndex];
        for (int meshCompIndex = 0; meshCompIndex < meshLayout.NumComponents(); meshCompIndex++) {
            const auto& comp = meshLayout.ComponentAt(meshCompIndex);
            // find the vertex attribute index in the shader
            int mtlAttrIndex = pip.shd->vsAttrIndices[comp.Attr];
            if (InvalidIndex != mtlAttrIndex) {
                vtxDesc.attributes[mtlAttrIndex].format = mtlTypes::asVertexFormat(comp.Format);
                vtxDesc.attributes[mtlAttrIndex].bufferIndex = vbSlotIndex;
                vtxDesc.attributes[mtlAttrIndex].offset = meshLayout.ComponentByteOffset(meshCompIndex);
            }
        }
        vtxDesc.layouts[vbSlotIndex].stride = meshLayout.ByteSize();
        vtxDesc.layouts[vbSlotIndex].stepFunction = mtlTypes::asVertexStepFunc(meshLayout.StepFunction);
        vtxDesc.layouts[vbSlotIndex].stepRate = meshLayout.StepRate;
    }

    // create renderpipeline-state
    const BlendState& blendState = pip.Setup.BlendState;
    MTLRenderPipelineDescriptor* rpDesc = [[MTLRenderPipelineDescriptor alloc] init];
    o_assert(blendState.MRTCount <= GfxConfig::MaxNumColorAttachments);
    for (int i = 0; i < blendState.MRTCount; i++) {
        rpDesc.colorAttachments[i].pixelFormat = mtlTypes::asRenderTargetColorFormat(blendState.ColorFormat);
        rpDesc.colorAttachments[i].writeMask = mtlTypes::asColorWriteMask(blendState.ColorWriteMask);
        rpDesc.colorAttachments[i].blendingEnabled = blendState.BlendEnabled;
        rpDesc.colorAttachments[i].alphaBlendOperation = mtlTypes::asBlendOp(blendState.OpAlpha);
        rpDesc.colorAttachments[i].rgbBlendOperation = mtlTypes::asBlendOp(blendState.OpRGB);
        rpDesc.colorAttachments[i].destinationAlphaBlendFactor = mtlTypes::asBlendFactor(blendState.DstFactorAlpha);
        rpDesc.colorAttachments[i].destinationRGBBlendFactor = mtlTypes::asBlendFactor(blendState.DstFactorRGB);
        rpDesc.colorAttachments[i].sourceAlphaBlendFactor = mtlTypes::asBlendFactor(blendState.SrcFactorAlpha);
        rpDesc.colorAttachments[i].sourceRGBBlendFactor = mtlTypes::asBlendFactor(blendState.SrcFactorRGB);
    }
    rpDesc.depthAttachmentPixelFormat = mtlTypes::asRenderTargetDepthFormat(blendState.DepthFormat);
    rpDesc.stencilAttachmentPixelFormat = mtlTypes::asRenderTargetStencilFormat(blendState.DepthFormat);
    rpDesc.fragmentFunction = pip.shd->mtlFragmentShader;
    rpDesc.vertexFunction = pip.shd->mtlVertexShader;
    rpDesc.vertexDescriptor = vtxDesc;
    rpDesc.rasterizationEnabled = YES;
    rpDesc.alphaToCoverageEnabled = pip.Setup.RasterizerState.AlphaToCoverageEnabled;
    rpDesc.alphaToOneEnabled = NO;
    rpDesc.sampleCount = pip.Setup.RasterizerState.SampleCount;
    NSError* err = NULL;
    pip.mtlRenderPipelineState = [this->pointers.renderer->mtlDevice newRenderPipelineStateWithDescriptor:rpDesc error:&err];
    if (!pip.mtlRenderPipelineState) {
        o_error("mtlPipelineFactory: failed to create MTLRenderPipelineState with:\n  %s\n", err.localizedDescription.UTF8String);
    }

    // create depth-stencil-state
    const DepthStencilState& dss = pip.Setup.DepthStencilState;
    MTLDepthStencilDescriptor* dsDesc = [[MTLDepthStencilDescriptor alloc] init];
    dsDesc.depthCompareFunction = mtlTypes::asCompareFunc(dss.DepthCmpFunc);
    dsDesc.depthWriteEnabled = dss.DepthWriteEnabled;
    if (dss.StencilEnabled) {
        dsDesc.backFaceStencil = [[MTLStencilDescriptor alloc] init];
        dsDesc.backFaceStencil.stencilFailureOperation = mtlTypes::asStencilOp(dss.StencilBack.FailOp);
        dsDesc.backFaceStencil.depthFailureOperation = mtlTypes::asStencilOp(dss.StencilBack.DepthFailOp);
        dsDesc.backFaceStencil.depthStencilPassOperation = mtlTypes::asStencilOp(dss.StencilBack.PassOp);
        dsDesc.backFaceStencil.stencilCompareFunction = mtlTypes::asCompareFunc(dss.StencilBack.CmpFunc);
        dsDesc.backFaceStencil.readMask = dss.StencilReadMask;
        dsDesc.backFaceStencil.writeMask = dss.StencilWriteMask;

        dsDesc.frontFaceStencil = [[MTLStencilDescriptor alloc] init];
        dsDesc.frontFaceStencil.stencilFailureOperation = mtlTypes::asStencilOp(dss.StencilFront.FailOp);
        dsDesc.frontFaceStencil.depthFailureOperation = mtlTypes::asStencilOp(dss.StencilFront.DepthFailOp);
        dsDesc.frontFaceStencil.depthStencilPassOperation = mtlTypes::asStencilOp(dss.StencilFront.PassOp);
        dsDesc.frontFaceStencil.stencilCompareFunction = mtlTypes::asCompareFunc(dss.StencilFront.CmpFunc);
        dsDesc.frontFaceStencil.readMask = dss.StencilReadMask;
        dsDesc.frontFaceStencil.writeMask = dss.StencilWriteMask;
    }
    pip.mtlDepthStencilState = [this->pointers.renderer->mtlDevice newDepthStencilStateWithDescriptor:dsDesc];
    o_assert(nil != pip.mtlDepthStencilState);

    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
mtlFactory::destroyPipeline(pipeline& pip) {
    o_assert_dbg(this->isValid);
    if (nil != pip.mtlRenderPipelineState) {
        this->releaseDeferred(pip.mtlRenderPipelineState);
    }
    if (nil != pip.mtlDepthStencilState) {
        this->releaseDeferred(pip.mtlDepthStencilState);
    }
    gfxFactoryBase::destroyPipeline(pip);
}

//------------------------------------------------------------------------------
mtlFactory::freeItem::freeItem() :
frameIndex(0),
obj(nil) {
    // empty
}

//------------------------------------------------------------------------------
mtlFactory::freeItem::freeItem(int frameIndex_, ORYOL_OBJC_ID obj_) :
frameIndex(frameIndex_),
obj(obj_) {
    // empty
}

//------------------------------------------------------------------------------
mtlFactory::freeItem::~freeItem() {
    if (nil != this->obj) {
        ORYOL_OBJC_RELEASE(this->obj);
        this->obj = nil;
    }
}

//------------------------------------------------------------------------------
void
mtlFactory::garbageCollect() {
    o_assert_dbg(this->isValid);
    const int frameIndex = this->pointers.renderer->frameIndex;
    const int safeNumFrames = GfxConfig::MaxInflightFrames + 1;
    if (frameIndex > safeNumFrames) {
        const int minReleaseFrame = frameIndex - GfxConfig::MaxInflightFrames;
        while (!this->releaseQueue.Empty() && (this->releaseQueue.Front().frameIndex < minReleaseFrame)) {
            this->releaseQueue.Dequeue();
        }
    }
}

//------------------------------------------------------------------------------
void
mtlFactory::releaseDeferred(ORYOL_OBJC_ID obj) {
    o_assert_dbg(this->isValid);
    const int frameIndex = this->pointers.renderer->frameIndex;
    this->releaseQueue.Enqueue(frameIndex, obj);
}

} // namespace _priv
} // namespace Oryol
