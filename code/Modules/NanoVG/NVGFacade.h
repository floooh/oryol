#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::NVGFacade
    @ingroup NanoVG
    @brief nanovg initialization and resource management wrapper
*/
#include "Core/Types.h"
#include "Core/Singleton.h"
#include "Core/Ptr.h"
#include "IO/Stream/Stream.h"
#include "Core/Containers/Map.h"

// FIXME: Windows.h workaround
#ifdef CreateFont
#undef CreateFont
#endif

struct NVGcontext;

namespace Oryol {
    
class NVGFacade {
    OryolLocalSingletonDecl(NVGFacade);
public:
    /// constructor
    NVGFacade();
    /// destructor
    ~NVGFacade();
    
    /// create nanovg context
    NVGcontext* CreateContext(int flags);
    /// destroy nanovg context
    void DeleteContext(NVGcontext* ctx);
    /// synchronously create nanovg image (image file must be preloaded into stream)
    int CreateImage(NVGcontext* ctx, const Ptr<Stream>& fileData, int imageFlags);
    /// delete image
    void DeleteImage(NVGcontext* ctx, int imgHandle);
    /// synchronously create nanovg font (font file must be preloaded into stream)
    int CreateFont(NVGcontext* ctx, const char* name, const Ptr<Stream>& fileData);
    /// delete font
    void DeleteFont(NVGcontext* ctx, int fontHandle);

    /// begin nanovg rendering
    void BeginFrame(NVGcontext* ctx);
    /// end nanovg rendering
    void EndFrame(NVGcontext* ctx);
    
private:
    Map<int, Ptr<Stream>> fontStreams;
};
    
} // namespace Oryol
