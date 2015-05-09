#pragma once
//------------------------------------------------------------------------------
/**
    @defgroup NanoVG NanoVG
    @brief NanoVG Wrapper Module
    
    @class Oryol::NanoVG
    @ingroup NanoVG
    @brief NanoVG module facade
*/
#include "Core/Types.h"
#include "Core/Ptr.h"
#include "IO/Stream/Stream.h"
#include "Core/Containers/Map.h"

// FIXME: Windows.h workaround
#ifdef CreateFont
#undef CreateFont
#endif

struct NVGcontext;

namespace Oryol {
class NanoVG {
public:
    /// setup the NanoVG module
    static void Setup();
    /// discard the NanoVG module
    static void Discard();
    /// check if NanoVG module has been setup
    static bool IsValid();
    
    /// create nanovg context
    static NVGcontext* CreateContext(int flags);
    /// destroy nanovg context
    static void DeleteContext(NVGcontext* ctx);
    /// synchronously create nanovg image (image file must be preloaded into stream)
    static int CreateImage(NVGcontext* ctx, const Ptr<Stream>& fileData, int imageFlags);
    /// delete image
    static void DeleteImage(NVGcontext* ctx, int imgHandle);
    /// synchronously create nanovg font (font file must be preloaded into stream)
    static int CreateFont(NVGcontext* ctx, const char* name, const Ptr<Stream>& fileData);
    /// delete font
    static void DeleteFont(NVGcontext* ctx, int fontHandle);

    /// begin nanovg rendering
    static void BeginFrame(NVGcontext* ctx);
    /// end nanovg rendering
    static void EndFrame(NVGcontext* ctx);
    
private:
    struct _state {
        Map<int, Ptr<Stream>> fontStreams;
    };
    static _state *state;
};
    
} // namespace Oryol
