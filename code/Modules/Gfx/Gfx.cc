//------------------------------------------------------------------------------
//  Gfx.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "Gfx.h"
#include "Core/Core.h"
#include "Gfx/Core/gfxPointers.h"

namespace Oryol {

using namespace _priv;

Gfx::_state* Gfx::state = nullptr;

//------------------------------------------------------------------------------
void
Gfx::Setup(const class GfxSetup& setup) {
    o_assert_dbg(!IsValid());
    state = Memory::New<_state>();
    state->gfxSetup = setup;

    gfxPointers pointers;
    pointers.displayMgr = &state->displayManager;
    pointers.renderer = &state->renderer;
    pointers.resContainer = &state->resourceContainer;
    pointers.meshPool = &state->resourceContainer.meshPool;
    pointers.shaderPool = &state->resourceContainer.shaderPool;
    pointers.texturePool = &state->resourceContainer.texturePool;
    pointers.pipelinePool = &state->resourceContainer.pipelinePool;
    
    state->displayManager.SetupDisplay(setup, pointers);
    state->renderer.setup(setup, pointers);
    state->resourceContainer.setup(setup, pointers);
    state->runLoopId = Core::PreRunLoop()->Add([] {
        state->displayManager.ProcessSystemEvents();
    });
    state->gfxFrameInfo = GfxFrameInfo();
}

//------------------------------------------------------------------------------
void
Gfx::Discard() {
    o_assert_dbg(IsValid());
    state->resourceContainer.Destroy(ResourceLabel::All);
    Core::PreRunLoop()->Remove(state->runLoopId);
    state->renderer.discard();
    state->resourceContainer.discard();
    state->displayManager.DiscardDisplay();
    Memory::Delete(state);
    state = nullptr;
}

//------------------------------------------------------------------------------
bool
Gfx::IsValid() {
    return nullptr != state;
}

//------------------------------------------------------------------------------
bool
Gfx::QuitRequested() {
    o_assert_dbg(IsValid());
    return state->displayManager.QuitRequested();
}

//------------------------------------------------------------------------------
Gfx::EventHandlerId
Gfx::Subscribe(EventHandler handler) {
    o_assert_dbg(IsValid());
    return state->displayManager.Subscribe(handler);
}

//------------------------------------------------------------------------------
void
Gfx::Unsubscribe(EventHandlerId id) {
    o_assert_dbg(IsValid());
    state->displayManager.Unsubscribe(id);
}

//------------------------------------------------------------------------------
const GfxSetup&
Gfx::GfxSetup() {
    o_assert_dbg(IsValid());
    return state->gfxSetup;
}

//------------------------------------------------------------------------------
const DisplayAttrs&
Gfx::DisplayAttrs() {
    o_assert_dbg(IsValid());
    return state->displayManager.GetDisplayAttrs();
}

//------------------------------------------------------------------------------
const DisplayAttrs&
Gfx::RenderTargetAttrs() {
    o_assert_dbg(IsValid());
    return state->renderer.renderTargetAttrs();
}

//------------------------------------------------------------------------------
const GfxFrameInfo&
Gfx::FrameInfo() {
    o_assert_dbg(IsValid());
    return state->gfxFrameInfo;
}

//------------------------------------------------------------------------------
void
Gfx::ApplyDefaultRenderTarget(const ClearState& clearState) {
    o_assert_dbg(IsValid());
    state->gfxFrameInfo.NumApplyRenderTarget++;
    state->renderer.applyRenderTarget(nullptr, clearState);
}

//------------------------------------------------------------------------------
void
Gfx::ApplyRenderTarget(const Id& id, const ClearState& clearState) {
    o_assert_dbg(IsValid());
    o_assert_dbg(id.IsValid());
    o_assert_dbg(id.Type == GfxResourceType::Texture);

    state->gfxFrameInfo.NumApplyRenderTarget++;
    texture* renderTarget = state->resourceContainer.lookupTexture(id);
    o_assert_dbg(nullptr != renderTarget);
    state->renderer.applyRenderTarget(renderTarget, clearState);
}

//------------------------------------------------------------------------------
void
Gfx::ApplyDrawState(const DrawState& drawState) {
    o_trace_scoped(Gfx_ApplyDrawState);
    o_assert_dbg(IsValid());
    o_assert_dbg(drawState.Pipeline.Type == GfxResourceType::Pipeline);
    state->gfxFrameInfo.NumApplyDrawState++;

    // apply pipeline and meshes
    pipeline* pip = state->resourceContainer.lookupPipeline(drawState.Pipeline);
    o_assert_dbg(pip);
    mesh* meshes[GfxConfig::MaxNumInputMeshes] = { };
    int numMeshes = 0;
    for (; numMeshes < GfxConfig::MaxNumInputMeshes; numMeshes++) {
        if (drawState.Mesh[numMeshes].IsValid()) {
            meshes[numMeshes] = state->resourceContainer.lookupMesh(drawState.Mesh[numMeshes]);
        }
        else {
            break;
        }
    }
    #if ORYOL_DEBUG
    validateMeshes(pip, meshes, numMeshes);
    #endif
    state->renderer.applyDrawState(pip, meshes, numMeshes);

    // apply vertex textures if any
    texture* vsTextures[GfxConfig::MaxNumVertexTextures] = { };
    int numVSTextures = 0;
    for (; numVSTextures < GfxConfig::MaxNumVertexTextures; numVSTextures++) {
        const Id& texId = drawState.VSTexture[numVSTextures];
        if (texId.IsValid()) {
            vsTextures[numVSTextures] = state->resourceContainer.lookupTexture(texId);
        }
        else {
            break;
        }
    }
    if (numVSTextures > 0) {
        #if ORYOL_DEBUG
        validateTextures(ShaderStage::VS, pip, vsTextures, numVSTextures);
        #endif
        state->renderer.applyTextures(ShaderStage::VS, vsTextures, numVSTextures);
    }

    // apply fragment textures if any
    texture* fsTextures[GfxConfig::MaxNumFragmentTextures] = { };
    int numFSTextures = 0;
    for (; numFSTextures < GfxConfig::MaxNumFragmentTextures; numFSTextures++) {
        const Id& texId = drawState.FSTexture[numFSTextures];
        if (texId.IsValid()) {
            fsTextures[numFSTextures] = state->resourceContainer.lookupTexture(texId);
        }
        else {
            break;
        }
    }
    if (numFSTextures > 0) {
        #if ORYOL_DEBUG
        validateTextures(ShaderStage::FS, pip, fsTextures, numFSTextures);
        #endif
        state->renderer.applyTextures(ShaderStage::FS, fsTextures, numFSTextures);
    }
}

//------------------------------------------------------------------------------
bool
Gfx::QueryFeature(GfxFeature::Code feat) {
    o_assert_dbg(IsValid());
    return state->renderer.queryFeature(feat);
}

//------------------------------------------------------------------------------
ResourceLabel
Gfx::PushResourceLabel() {
    o_assert_dbg(IsValid());
    return state->resourceContainer.PushLabel();
}

//------------------------------------------------------------------------------
void
Gfx::PushResourceLabel(ResourceLabel label) {
    o_assert_dbg(IsValid());
    state->resourceContainer.PushLabel(label);
}

//------------------------------------------------------------------------------
ResourceLabel
Gfx::PopResourceLabel() {
    o_assert_dbg(IsValid());
    return state->resourceContainer.PopLabel();
}

//------------------------------------------------------------------------------
Id
Gfx::LoadResource(const Ptr<ResourceLoader>& loader) {
    o_assert_dbg(IsValid());
    return state->resourceContainer.Load(loader);
}

//------------------------------------------------------------------------------
Id
Gfx::LookupResource(const Locator& locator) {
    o_assert_dbg(IsValid());
    return state->resourceContainer.Lookup(locator);
}

//------------------------------------------------------------------------------
int
Gfx::QueryFreeResourceSlots(GfxResourceType::Code resourceType) {
    o_assert_dbg(IsValid());
    return state->resourceContainer.QueryFreeSlots(resourceType);
}

//------------------------------------------------------------------------------
ResourceInfo
Gfx::QueryResourceInfo(const Id& id) {
    o_assert_dbg(IsValid());
    return state->resourceContainer.QueryResourceInfo(id);
}

//------------------------------------------------------------------------------
ResourcePoolInfo
Gfx::QueryResourcePoolInfo(GfxResourceType::Code resType) {
    o_assert_dbg(IsValid());
    return state->resourceContainer.QueryPoolInfo(resType);
}

//------------------------------------------------------------------------------
void
Gfx::DestroyResources(ResourceLabel label) {
    o_assert_dbg(IsValid());
    return state->resourceContainer.Destroy(label);
}

//------------------------------------------------------------------------------
_priv::gfxResourceContainer&
Gfx::resource() {
    o_assert_dbg(IsValid());
    return state->resourceContainer;
}

//------------------------------------------------------------------------------
void
Gfx::ApplyViewPort(int x, int y, int width, int height, bool originTopLeft) {
    o_assert_dbg(IsValid());
    state->gfxFrameInfo.NumApplyViewPort++;
    state->renderer.applyViewPort(x, y, width, height, originTopLeft);
}

//------------------------------------------------------------------------------
void
Gfx::ApplyScissorRect(int x, int y, int width, int height, bool originTopLeft) {
    o_assert_dbg(IsValid());
    state->gfxFrameInfo.NumApplyScissorRect++;
    state->renderer.applyScissorRect(x, y, width, height, originTopLeft);
}

//------------------------------------------------------------------------------
void
Gfx::CommitFrame() {
    o_trace_scoped(Gfx_CommitFrame);
    o_assert_dbg(IsValid());
    state->renderer.commitFrame();
    state->displayManager.Present();
    state->gfxFrameInfo = GfxFrameInfo();
}

//------------------------------------------------------------------------------
void
Gfx::ResetStateCache() {
    o_trace_scoped(Gfx_ResetStateCache);
    o_assert_dbg(IsValid());
    state->renderer.resetStateCache();
}

//------------------------------------------------------------------------------
void
Gfx::UpdateVertices(const Id& id, const void* data, int numBytes) {
    o_trace_scoped(Gfx_UpdateVertices);
    o_assert_dbg(IsValid());
    state->gfxFrameInfo.NumUpdateVertices++;
    mesh* msh = state->resourceContainer.lookupMesh(id);
    state->renderer.updateVertices(msh, data, numBytes);
}

//------------------------------------------------------------------------------
void
Gfx::UpdateIndices(const Id& id, const void* data, int numBytes) {
    o_trace_scoped(Gfx_UpdateIndices);
    o_assert_dbg(IsValid());
    state->gfxFrameInfo.NumUpdateIndices++;
    mesh* msh = state->resourceContainer.lookupMesh(id);
    state->renderer.updateIndices(msh, data, numBytes);
}

//------------------------------------------------------------------------------
void
Gfx::UpdateTexture(const Id& id, const void* data, const ImageDataAttrs& offsetsAndSizes) {
    o_trace_scoped(Gfx_UpdateTexture);
    o_assert_dbg(IsValid());
    state->gfxFrameInfo.NumUpdateTextures++;
    texture* tex = state->resourceContainer.lookupTexture(id);
    state->renderer.updateTexture(tex, data, offsetsAndSizes);
}

//------------------------------------------------------------------------------
void
Gfx::ReadPixels(void* buf, int bufNumBytes) {
    o_trace_scoped(Gfx_ReadPixels);
    o_assert_dbg(IsValid());
    state->renderer.readPixels(buf, bufNumBytes);
}

//------------------------------------------------------------------------------
void
Gfx::Draw(int primGroupIndex, int numInstances) {
    o_trace_scoped(Gfx_Draw);
    o_assert_dbg(IsValid());
    state->gfxFrameInfo.NumDraw++;
    if (1 == numInstances) {
        state->renderer.draw(primGroupIndex);
    }
    else {
        state->renderer.drawInstanced(primGroupIndex, numInstances);
    }
}

//------------------------------------------------------------------------------
void
Gfx::Draw(const PrimitiveGroup& primGroup, int numInstances) {
    o_trace_scoped(Gfx_Draw);
    o_assert_dbg(IsValid());
    state->gfxFrameInfo.NumDraw++;
    if (1 == numInstances) {
        state->renderer.draw(primGroup);
    }
    else {
        state->renderer.drawInstanced(primGroup, numInstances);
    }
}

//------------------------------------------------------------------------------
#if ORYOL_DEBUG
void
Gfx::validateMeshes(pipeline* pip, mesh** meshes, int num) {

    // checks that:
    //  - at least one input mesh must be attached, and it must be in slot 0
    //  - all attached input meshes must be valid
    //  - PrimitivesGroups may only be attached to the mesh in slot 0
    //  - if indexed rendering is used, the mesh in slot 0 must have an index buffer
    //  - the mesh in slot 0 cannot contain per-instance-data
    //  - no colliding vertex attributes across all input meshes
    //
    // this method should only be called in debug mode

    // FIXME FIXME FIXME: check for matching vertex layout!!!

    o_assert_dbg(meshes && (num > 0) && (num < GfxConfig::MaxNumInputMeshes));
    if (nullptr == meshes[0]) {
        o_error("invalid mesh block: at least one input mesh must be provided, in slot 0!\n");
    }
    if ((meshes[0]->indexBufferAttrs.Type != IndexType::None) &&
        (meshes[0]->indexBufferAttrs.NumIndices == 0)) {
        o_error("invalid mesh block: the input mesh at slot 0 uses indexed rendering, but has no indices!\n");
    }

    StaticArray<int, VertexAttr::NumVertexAttrs> vertexAttrCounts;
    vertexAttrCounts.Fill(0);
    for (int mshIndex = 0; mshIndex < GfxConfig::MaxNumInputMeshes; mshIndex++) {
        const meshBase* msh = meshes[mshIndex];
        if (msh) {
            if (ResourceState::Valid != msh->State) {
                o_error("invalid mesh block: input mesh at slot '%d' not valid!\n", mshIndex);
            }
            if ((mshIndex > 0) && (msh->indexBufferAttrs.Type != IndexType::None)) {
                o_error("invalid drawState: input mesh at slot '%d' has indices, only allowed for slot 0!", mshIndex);
            }
            if ((mshIndex > 0) && (msh->numPrimGroups > 0)) {
                o_error("invalid mesh block: input mesh at slot '%d' has primitive groups, only allowed for slot 0!", mshIndex);
            }
            const int numComps = msh->vertexBufferAttrs.Layout.NumComponents();
            for (int compIndex = 0; compIndex < numComps; compIndex++) {
                const auto& comp = msh->vertexBufferAttrs.Layout.ComponentAt(compIndex);
                vertexAttrCounts[comp.Attr]++;
                if (vertexAttrCounts[comp.Attr] > 1) {
                    o_error("invalid mesh block: same vertex attribute declared in multiple input meshes!");
                }
            }
        }
    }
}
#endif

//------------------------------------------------------------------------------
#if ORYOL_DEBUG
void
Gfx::validateTextures(ShaderStage::Code stage, pipeline* pip, texture** textures, int numTextures) {
    o_assert_dbg(pip);

    // check if provided texture types are compatible with the expections shader
    const shader* shd = pip->shd;
    o_assert_dbg(shd);
    int texBlockIndex = shd->Setup.TextureBlockIndexByStage(stage);
    o_assert_dbg(InvalidIndex != texBlockIndex);
    const TextureBlockLayout& layout = shd->Setup.TextureBlockLayout(texBlockIndex);
    for (int i = 0; i < numTextures; i++) {
        if (textures[i]) {
            const auto& texBlockComp = layout.ComponentAt(layout.ComponentIndexForBindSlot(i));
            if (texBlockComp.Type != textures[i]->textureAttrs.Type) {
                o_error("Texture type mismatch at slot '%s'\n", texBlockComp.Name.AsCStr());
            }
        }
    }
}
#endif

} // namespace Oryol
