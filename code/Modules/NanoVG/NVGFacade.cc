//------------------------------------------------------------------------------
//  NVGFacade.cc
//------------------------------------------------------------------------------
#include "Pre.h"
#include "NVGFacade.h"
#if ORYOL_OPENGLES2
#define NANOVG_GLES2_IMPLEMENTATION
#elif ORYOL_OSX
#define NANOVG_GL3_IMPLEMENTATION
#else
#define NANOVG_GL2_IMPLEMENTATION
#endif
#include "NanoVG.h"
#include "Render/RenderFacade.h"

namespace Oryol {
namespace NanoVG {

using namespace Render;
    
OryolLocalSingletonImpl(NVGFacade);
    
//------------------------------------------------------------------------------
NVGFacade::NVGFacade() {
    this->SingletonEnsureUnique();

    // make sure that a RenderFacade object exists
    if (!RenderFacade::HasInstance()) {
        o_error("RenderFacade object must be created before NVGFacade!\n");
    }
}

//------------------------------------------------------------------------------
NVGFacade::~NVGFacade() {
    // empty
}

//------------------------------------------------------------------------------
NVGcontext*
NVGFacade::CreateContext(int flags) {
    NVGcontext* ctx = nullptr;
    #if ORYOL_OPENGLES2
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
NVGFacade::DeleteContext(NVGcontext* ctx) {
    o_assert_dbg(ctx);
    #if ORYOL_OPENGLES2
    nvgDeleteGLES2(ctx);
    #elif ORYOL_OSX
    nvgDeleteGL3(ctx);
    #else
    nvgDeleteGL2(ctx);
    #endif
}

//------------------------------------------------------------------------------
void
NVGFacade::BeginFrame(NVGcontext* ctx) {
    o_assert_dbg(ctx);
    
    const DisplayAttrs& dispAttrs = RenderFacade::Instance()->GetDisplayAttrs();
    const int32 w = dispAttrs.FramebufferWidth;
    const int32 h = dispAttrs.FramebufferHeight;
    o_assert_dbg(h > 0);
    const float32 aspect = float32(w) / float32(h);
    
    nvgBeginFrame(ctx, w, h, aspect);
}

//------------------------------------------------------------------------------
void
NVGFacade::EndFrame(NVGcontext* ctx) {
    o_assert_dbg(ctx);
    nvgEndFrame(ctx);
    RenderFacade::Instance()->ResetStateCache();
}

//------------------------------------------------------------------------------
int
NVGFacade::CreateImage(NVGcontext* ctx, const Ptr<Stream>& fileData, int flags) {
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
NVGFacade::DeleteImage(NVGcontext* ctx, int imgHandle) {
    o_assert_dbg(ctx);
    if (0 != imgHandle) {
        nvgDeleteImage(ctx, imgHandle);
    }
}

//------------------------------------------------------------------------------
int
NVGFacade::CreateFont(NVGcontext* ctx, const char* name, const Ptr<Stream>& fileData) {
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
        this->fontStreams.AddUnique(fontHandle, fileData);
    }
    return fontHandle;
}

} // namespace NanoVG
} // namespace Oryol