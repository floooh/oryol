//------------------------------------------------------------------------------
//  NVGFacade.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "NanoVG.h"
#if (ORYOL_OPENGLES2 || ORYOL_OPENGLES3)
#define NANOVG_GLES2_IMPLEMENTATION
#elif ORYOL_OSX
#define NANOVG_GL3_IMPLEMENTATION
#else
#define NANOVG_GL2_IMPLEMENTATION
#endif
#include "NanoVGWrapper.h"
#include "Gfx/Gfx.h"

namespace Oryol {

NanoVG::_state* NanoVG::state = nullptr;
    
//------------------------------------------------------------------------------
void
NanoVG::Setup() {
    o_assert(!IsValid());
    state = new _state();
    if (!Gfx::IsValid()) {
        o_error("Render module must be setuo before NanoVG!\n");
    }
}

//------------------------------------------------------------------------------
void
NanoVG::Discard() {
    o_assert(IsValid());
    delete state;
    state = nullptr;
}

//------------------------------------------------------------------------------
bool
NanoVG::IsValid() {
    return nullptr != state;
}

//------------------------------------------------------------------------------
NVGcontext*
NanoVG::CreateContext(int flags) {
    o_assert_dbg(IsValid());

    NVGcontext* ctx = nullptr;
    #if (ORYOL_OPENGLES2 || ORYOL_OPENGLES3)
    ctx = nvgCreateGLES2(flags);
    #elif ORYOL_OSX
    ctx = nvgCreateGL3(flags);
    #else
    ctx = nvgCreateGL2(flags);
    #endif
    return ctx;
}

//------------------------------------------------------------------------------
void
NanoVG::DeleteContext(NVGcontext* ctx) {
    o_assert_dbg(IsValid());
    o_assert_dbg(ctx);
    
    #if (ORYOL_OPENGLES2 || ORYOL_OPENGLES3)
    nvgDeleteGLES2(ctx);
    #elif ORYOL_OSX
    nvgDeleteGL3(ctx);
    #else
    nvgDeleteGL2(ctx);
    #endif
}

//------------------------------------------------------------------------------
void
NanoVG::BeginFrame(NVGcontext* ctx) {
    o_assert_dbg(IsValid());
    o_assert_dbg(ctx);
    
    const int32 w = Gfx::DisplayAttrs().FramebufferWidth;
    const int32 h = Gfx::DisplayAttrs().FramebufferHeight;
    o_assert_dbg(h > 0);
    const float32 aspect = float32(w) / float32(h);
    
    nvgBeginFrame(ctx, w, h, aspect);
}

//------------------------------------------------------------------------------
void
NanoVG::EndFrame(NVGcontext* ctx) {
    o_assert_dbg(IsValid());
    o_assert_dbg(ctx);

    nvgEndFrame(ctx);
    Gfx::ResetStateCache();
}

//------------------------------------------------------------------------------
int
NanoVG::CreateImage(NVGcontext* ctx, const Ptr<Stream>& fileData, int flags) {
    o_assert_dbg(IsValid());
    o_assert_dbg(ctx);
    o_assert_dbg(fileData.isValid() && (fileData->Size() > 0));
    
    int imgHandle = 0;
    if (fileData->Open(OpenMode::ReadOnly)) {
        const uint8* maxValidPtr = nullptr;
        const uint8* ptr = fileData->MapRead(&maxValidPtr);
        imgHandle = nvgCreateImageMem(ctx, flags, (uint8*) ptr, fileData->Size());
        fileData->UnmapRead();
        fileData->Close();
    }
    return imgHandle;
}

//------------------------------------------------------------------------------
void
NanoVG::DeleteImage(NVGcontext* ctx, int imgHandle) {
    o_assert_dbg(IsValid());
    o_assert_dbg(ctx);

    if (0 != imgHandle) {
        nvgDeleteImage(ctx, imgHandle);
    }
}

//------------------------------------------------------------------------------
int
NanoVG::CreateFont(NVGcontext* ctx, const char* name, const Ptr<Stream>& fileData) {
    o_assert_dbg(IsValid());
    o_assert_dbg(ctx);
    o_assert_dbg(name);
    o_assert_dbg(fileData.isValid() && (fileData->Size() > 0));
    
    int fontHandle = 0;
    if (fileData->Open(OpenMode::ReadOnly)) {
        const uint8* maxValidPtr = nullptr;
        const uint8* ptr = fileData->MapRead(&maxValidPtr);
        fontHandle = nvgCreateFontMem(ctx, name, (uint8*) ptr, fileData->Size(), 0);
        fileData->UnmapRead();
        fileData->Close();
        
        // we need to keep the font memory chunk around until the font is
        // deleted, but it looks like nanovg has no way to delete a font(?)
        state->fontStreams.AddUnique(fontHandle, fileData);
    }
    return fontHandle;
}

} // namespace Oryol
