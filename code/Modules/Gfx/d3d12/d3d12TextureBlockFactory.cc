//------------------------------------------------------------------------------
//  d3d12TextureBlockFactory.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Gfx/Core/renderer.h"
#include "Gfx/Resource/textureBlock.h"
#include "Gfx/Resource/resourcePools.h"
#include "Core/Assertion.h"
#include "d3d12_impl.h"
#include "d3d12TextureBlockFactory.h"
#include "d3d12Types.h"

namespace Oryol {
namespace _priv {

//------------------------------------------------------------------------------
d3d12TextureBlockFactory::d3d12TextureBlockFactory() :
isValid(false) {
    // empty
}

//------------------------------------------------------------------------------
d3d12TextureBlockFactory::~d3d12TextureBlockFactory() {
    o_assert_dbg(!this->isValid);
}

//------------------------------------------------------------------------------
void
d3d12TextureBlockFactory::Setup(const gfxPointers& ptrs) {
    o_assert_dbg(!this->isValid);
    this->isValid = true;
    this->pointers = ptrs;
}

//------------------------------------------------------------------------------
void
d3d12TextureBlockFactory::Discard() {
    o_assert_dbg(this->isValid);
    this->isValid = false;
    this->pointers = gfxPointers();
}

//------------------------------------------------------------------------------
bool
d3d12TextureBlockFactory::IsValid() const {
    return this->isValid;
}

//------------------------------------------------------------------------------
ResourceState::Code
d3d12TextureBlockFactory::SetupResource(textureBlock& tb) {

    // FIXME FIXME FIXME
    //
    // The root signature needs one descriptor-table-entry
    // per update-frequency (per-pass, per-material, we don't allow
    // per instance texture changes).
    // Each of the 2 texture-block-bind-slot needs a 
    // base-register-index (e.g. texture registers 0..7 are for
    // texture-block-bind-slot 0, and texture registers 8..15
    // are for texture-block-bind-slot 1. The base-register-index
    // must be defined on the descriptor-range at root-signature-setup.
    //
    // (and the same for samplers)
    //
    // the shader code generator must allocate the texture and
    // sampler registers only continuously within 1 texture block,
    // and if several texture blocks exist, start at their respective
    // texture-base-register!

    /*
    o_assert_dbg(this->isValid);
    o_assert_dbg(this->pointers.texturePool);
    o_assert_dbg(this->pointers.shaderPool);

    ID3D12Device* d3d12Device = this->pointers.renderer->d3d12Device;
    o_assert_dbg(d3d12Device);
    d3d12DescAllocator& descAllocator = this->pointers.renderer->descAllocator;

    const shader* shd = this->pointers.shaderPool->Lookup(tb.Setup.Shader);
    o_assert_dbg(shd);
    int32 texBlockIndex = shd->Setup.TextureBlockIndexByStageAndSlot(tb.Setup.BindStage, tb.Setup.BindSlot);
    o_assert_dbg(InvalidIndex != texBlockIndex);
    const TextureBlockLayout& layout = shd->Setup.TextureBlockLayout(texBlockIndex);
    
    tb.d3d12SRVDescriptor = descAllocator.Allocate(d3d12DescAllocator::TextureBlock);
    tb.bindStage = tb.Setup.BindStage;

    for (int bindSlot = 0; bindSlot < GfxConfig::MaxNumTexturesPerStage; bindSlot++) {

        const Id& texId = tb.Setup.Slot[bindSlot];
        if (texId.IsValid()) {
            o_assert(texId.Type == GfxResourceType::Texture);

            // lookup source texture
            const texture* tex = this->pointers.texturePool->Get(texId);
            o_assert_dbg(tex && (ResourceState::Valid == tex->State));
            o_assert_dbg(tex->d3d12TextureRes);
            const TextureSetup& texSetup = tex->Setup;

            // make sure type of source texture (2D, Cube...) is matching the
            // expected binding slot texture type
            const auto& texBlockComp = layout.ComponentAt(layout.ComponentIndexForBindSlot(bindSlot));
            if (texBlockComp.Type != tex->textureAttrs.Type) {
                o_error("Texture type mismatch at slot '%s'\n", texBlockComp.Name.AsCStr());
            }

            // initialize the shader-resource-view
            D3D12_SHADER_RESOURCE_VIEW_DESC desc;
            Memory::Clear(&desc, sizeof(desc));
            desc.Format = d3d12Types::asTextureFormat(texSetup.ColorFormat);
            desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            if (texSetup.Type == TextureType::Texture2D) {
                desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
                desc.Texture2D.MostDetailedMip = 0;
                desc.Texture2D.MipLevels = texSetup.NumMipMaps;
                desc.Texture2D.PlaneSlice = 0;
                desc.Texture2D.ResourceMinLODClamp = 0.0f; 
            }
            else {
                desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
                desc.TextureCube.MostDetailedMip = 0;
                desc.TextureCube.MipLevels = texSetup.NumMipMaps;
                desc.TextureCube.ResourceMinLODClamp = 0.0f;
            }

            D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
            descAllocator.CPUHandle(tb.d3d12SRVDescriptor, bindSlot, cpuHandle);
            d3d12Device->CreateShaderResourceView(tex->d3d12TextureRes, &desc, cpuHandle);

            // FIXME: create sampler
        }
        else {
            // FIXME: how to 
        }
    }
    */
    return ResourceState::Valid;
}

//------------------------------------------------------------------------------
void
d3d12TextureBlockFactory::DestroyResource(textureBlock& tb) {
    o_assert_dbg(this->isValid);
    o_assert_dbg(this->pointers.renderer);

    d3d12DescAllocator& descAllocator = this->pointers.renderer->descAllocator;
    const uint64 frameIndex = this->pointers.renderer->frameIndex;

    // deferred-release the shader-resource-view range
    if (tb.d3d12SRVDescriptor.IsValid()) {
        descAllocator.ReleaseDeferred(frameIndex, tb.d3d12SRVDescriptor);
    }
    // FIXME: release samplers

    tb.Clear();
}

} // namespace _priv
} // namespace Oryol
