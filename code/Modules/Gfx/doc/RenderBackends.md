## Gfx Rendering Backends

Currently, Oryol apps can use the following rendering backends,
depending on platform:

* **HTML5**: WebGL or WebGL2
* **Windows**: GL 3.3 or Direct3D11
* **macOS**: GL3.3 or Metal
* **iOS**: GLES3 or Metal
* **Android**: GLES3 or GLES2
* **Linux**: GL 3.3
* **RaspberryPi**: GLES2

The common default rendering backend is GL/GLES. On mobile
and web platforms, GLES3/WebGL2 will fall back to GLES2 or 
WebGL if the newer API is not available.

### Selecting a Rendering Backend

Rendering backends are selected at compile time through cmake options and
cannot be switched at runtime. Each target platform has a default rendering
backend that will be selected automatically, usually this is OpenGL.

The best way to select a non-default rendering backend is to
select a different **fips build config**:

To see the list of supported build configs for D3D11 and Metal:

```
> ./fips list configs | grep d3d11
...
> ./fips list configs | grep metal
...
```

To select an iOS build config using the Metal rendering backend:

```
> ./fips set config metal-ios-xcode-debug
> ./fips gen
> ./fips open
```

...or to select D3D11 on Windows:

```
> ./fips set config d3d11-win64-vstudio-debug
> ./fips gen
> ./fips open
```

### Optional Rendering Features

Some rendering features are not supported on all platforms, and their
availability on a specific platform or rendering backend must be checked at
runtime before such an optional feature is used.

To check whether an optional feature is available:

```cpp
if (Gfx::QueryFeature(GfxFeature::MultipleRenderTarget)) {
    // MRT rendering is supported
}
```

The current list of optional features is defined in the GfxFeature enum class:

```cpp
class GfxFeature {
public:
    enum Code {
        TextureCompressionDXT = 0,  ///< GPU supports DXT compressed textures
        TextureCompressionPVRTC,    ///< GPU supports PVRTC compressed textures
        TextureCompressionATC,      ///< GPU supports ATC compressed textures
        TextureCompressionETC2,     ///< GPU supports ETC2 compressed textures (OpenGLES3)
        TextureFloat,               ///< support for float textures
        TextureHalfFloat,           ///< support for half-float textures
        Instancing,                 ///< supports hardware-instanced rendering
        OriginBottomLeft,           ///< image space origin is bottom-left (GL-style)
        OriginTopLeft,              ///< image space origin is top-left (D3D-style)
        MSAARenderTargets,          ///< MSAA support in offscreen-render-targets
        PackedVertexFormat_10_2,    ///< support for 10.10.10.2 bit packed vertex formats
        MultipleRenderTarget,       ///< support for MRT offscreen rendering
        Texture3D,                  ///< support for 3D textures
        TextureArray,               ///< support for array textures
        NativeTexture,              ///< can work with externally created texture objects

        NumFeatures,
        InvalidFeature
    };
};

```