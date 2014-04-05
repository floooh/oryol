#pragma once
//------------------------------------------------------------------------------
/**
    @class Oryol::Render::TextureSetup
    @brief setup object for textures and render targets
    @todo setup from file, setup from stream, multiple rendertarget support, mipmap generation mode...?
*/
#include "Resource/Locator.h"
#include "Resource/Id.h"
#include "Render/Types/Usage.h"
#include "Render/Types/PixelFormat.h"
#include "Render/Types/TextureWrapMode.h"
#include "Render/Types/TextureFilterMode.h"

namespace Oryol {
namespace Render {
    
class TextureSetup {
public:
    /// setup as absolute-size render target
    static TextureSetup AsRenderTarget(const Resource::Locator& loc, int32 w, int32 h, PixelFormat::Code colorFmt, PixelFormat::Code depthFmt=PixelFormat::InvalidPixelFormat);
    /// setup as render target with size relative to current display size
    static TextureSetup AsRelSizeRenderTarget(const Resource::Locator& loc, float32 relWidth, float32 relHeight, PixelFormat::Code colorFmt, PixelFormat::Code depthFmt=PixelFormat::InvalidPixelFormat);
    /// create render target with shared depth buffer
    static TextureSetup AsSharedDepthRenderTarget(const Resource::Locator& loc, PixelFormat::Code colorFmt, const Resource::Id& depthRenderTarget);

    /// default constructor
    TextureSetup();
    /// return true if texture should be setup from a file
    bool ShouldSetupFromFile() const;
    /// return true if texture should be setup from data stream
    bool ShouldSetupFromData() const;
    /// return true if texture should be setup as render target
    bool ShouldSetupAsRenderTarget() const;
    /// return true if rel-size render target
    bool IsRelSizeRenderTarget() const;
    /// return true if render target has depth (shared or non-shared)
    bool HasDepth() const;
    /// return true if render target with shared depth buffer
    bool HasSharedDepth() const;
    
    /// get the resource locator
    const Resource::Locator& GetLocator() const;
    /// get the width in pixels (only if absolute-size render target)
    int32 GetWidth() const;
    /// get the height in pixels (only if absolute-size render target)
    int32 GetHeight() const;
    /// get display-relative width (only if screen render target)
    float32 GetRelWidth() const;
    /// get display-relative height (only if screen render target)
    float32 GetRelHeight() const;
    /// get the color pixel format (only if render target)
    PixelFormat::Code GetColorFormat() const;
    /// get the depth pixel format (only if render target, InvalidPixelFormat if render target should not have depth buffer)
    PixelFormat::Code GetDepthFormat() const;
    /// get resource id of render target which owns the depth buffer (only if render target with shared depth buffer)
    const Resource::Id& GetDepthRenderTarget() const;
    
    /// set texture wrap mode for u dimension (default is Repeat for textures, and ClampToEdge for render targets)
    void SetWrapU(TextureWrapMode::Code wrapU);
    /// get texture wrap mode for u dimension
    TextureWrapMode::Code GetWrapU() const;
    /// set texture wrap mode for v dimension
    void SetWrapV(TextureWrapMode::Code wrapV);
    /// get texture wrap mode for v dimension
    TextureWrapMode::Code GetWrapV() const;
    /// set texture wrap mode for w dimension
    void SetWrapW(TextureWrapMode::Code wrapW);
    /// get texture wrap mode for w dimension
    TextureWrapMode::Code GetWrapW() const;
    
    /// set magnification sample filter
    void SetMagFilter(TextureFilterMode::Code magFiler);
    /// get magnification sample filter
    TextureFilterMode::Code GetMagFilter() const;
    /// set minification sample filter
    void SetMinFilter(TextureFilterMode::Code minFilter);
    /// get minification sample filter
    TextureFilterMode::Code GetMinFilter() const;
    
private:
    bool shouldSetupFromFile : 1;
    bool shouldSetupFromData : 1;
    bool shouldSetupAsRenderTarget : 1;
    bool isRelSizeRenderTarget : 1;
    bool hasSharedDepth : 1;
    
    Resource::Locator locator;
    int32 width;
    int32 height;
    float32 relWidth;
    float32 relHeight;
    PixelFormat::Code colorFormat;
    PixelFormat::Code depthFormat;
    Resource::Id depthRenderTarget;
    TextureWrapMode::Code wrapU;
    TextureWrapMode::Code wrapV;
    TextureWrapMode::Code wrapW;
    TextureFilterMode::Code magFilter;
    TextureFilterMode::Code minFilter;
};
    
} // namespace Render
} // namespace Oryol