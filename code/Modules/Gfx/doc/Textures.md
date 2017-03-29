## Textures

Texture objects provide image data for rendering, and 
can also act as render targets for offscreen rendering. 

2D- and cubemap textures are supported on all rendering backends, 2D-array
and 3D-textures are not supported on GLES2 and WebGL(1).

Samplers are not separate objects in the Oryol Gfx module
but instead are part of the texture state. This is also 
a restriction imposed by WebGL and GLES2.

### Related Data Types

Textures have a number of related data types, these are mostly
needed when creating texture objects:

#### TextureType

The TextureType enum class is used to describe which of the
4 supported texture types should be created:

```cpp
class TextureType {
public:
    enum Code {
        Texture2D = 0,
        TextureCube,
        Texture3D,
        TextureArray,

        NumTextureTypes,
        InvalidTextureType = 0xFFFFFFFF,
    };
};
```
A TextureArray is always an array of 2D textures (optionally with mipmaps), there is no support for cubemap-arrays.

#### PixelFormat

This is an enumeration class with associated utility methods
describing how the pixel data in a texture is structured. The
list of pixel formats is a subset of useful formats available
over a wide range of platforms.

Currently the following pixel formats are defined:

```cpp
class PixelFormat {
public:
    enum Code {
        RGBA8,          ///< 32-bit wide, 4 channels @ 8-bit
        RGB8,           ///< 24-bit wide, 3 channels @ 8-bit
        RGBA4,          ///< 16-bit wide, 4 channels @ 4-bit
        R5G6B5,         ///< 16-bit wide, 3 channels @ 5/6/5 bits
        R5G5B5A1,       ///< 16-bit wide, 4 channels @ 1-bit alpha, 5-bit rgb
        R10G10B10A2,    ///< 32-bit wide, 4 channels @ 10/10/10/2 bits
        RGBA32F,        ///< 128-bit wide, 4 channel @ 32-bit float
        RGBA16F,        ///< 64-bit wide, 4 channel @ 16-bit float
        R32F,           ///< 32-bit wide, 1 channel @ 32-bit float
        R16F,           ///< 16-bit wide, 1 channel @ 16-bit float

        L8,             ///< 8-bit wide, single channel
        DXT1,           ///< DXT1 compressed format
        DXT3,           ///< DXT3 compressed format
        DXT5,           ///< DXT5 compressed format
        DEPTH,          ///< depth-only format (at least 16 bit)
        DEPTHSTENCIL,   ///< depth-stencil format (usually D24S8)
        PVRTC2_RGB,     ///< PVRTC2 compressed format (RGB)
        PVRTC4_RGB,     ///< PVRTC4 compressed format (RGB)
        PVRTC2_RGBA,    ///< PVRTC2 compressed format (RGBA)
        PVRTC4_RGBA,    ///< PVRTC4 compressed format (RGBA)
        ETC2_RGB8,      ///< ETC2 compressed format (RGB8)
        ETC2_SRGB8,     ///< ETC2 compressed format (SRGB8)

        NumPixelFormats,            ///< number of pixel formats
        InvalidPixelFormat,         ///< invalid pixel format value
        None = InvalidPixelFormat,  ///< special "none" type
    };
}
```

The following static methods can be used to check a specific
pixel format against a requirement (such as if a pixel format
can be used for render target textures):

```cpp
class PixelFormat {
    ...
    /// return true for valid render target color formats
    static bool IsValidRenderTargetColorFormat(Code c);
    /// return true for valid render target depth formats
    static bool IsValidRenderTargetDepthFormat(Code c);
    /// return true for valid color texture formats
    static bool IsValidTextureColorFormat(Code c);
    /// return true for valid depth texture formats
    static bool IsValidTextureDepthFormat(Code c);
    /// test if the pixel format is a pure depth format (not a depth/stencil format)
    static bool IsDepthFormat(Code c);
    /// test if the pixel format is a depth/stencil format
    static bool IsDepthStencilFormat(Code c);
    /// return true if the pixel format is a compressed format
    static bool IsCompressedFormat(Code c);
    /// return true if this is a PVRTC-compressed format
    static bool IsPVRTC(Code c);
    /// return true if this is a DXT-compressed format
    static bool IsDXT(Code c);
    /// return true if this is an ETC-compressed format
    static bool IsETC2(Code c);
};
```

...and a few helper methods to compute sizes and distances:

```cpp
class PixelFormat {
    ...
    /// get byte size of pixel format
    static int ByteSize(Code c);
    /// get number of bits in a pixel format channel (only for non-compressed formats and non-depth formats!)
    static int8_t NumBits(Code pixelFormat, PixelChannel::Bits channel);
    /// compute row-pitch (distance in bytes from one row of data to next)
    static int RowPitch(PixelFormat::Code fmt, int width);
    /// compute image-pitch (distance in bytes from one image to next)
    static int ImagePitch(PixelFormat::Code fmt, int width, int height);
};
```

#### TextureWrapMode

Part of the texture sampler state, describes how uv coordinates outside
the 0.0..1.0 range are handled:

```cpp
class TextureWrapMode {
public:
    enum Code {
        ClampToEdge,
        Repeat,
        MirroredRepeat,
    };
};
```

#### TextureFilterMode

Also part of the texture sampler state, describes the 
filtering operations minifying or magnifying texture accesses:

```cpp
class TextureFilterMode {
public:
    enum Code {
        Nearest,
        Linear,
        NearestMipmapNearest,
        NearestMipmapLinear,
        LinearMipmapNearest,
        LinearMipmapLinear,
    };
};
```

#### Usage Hint

_Usage_ is used both for texture and mesh data to describe
the resource CPU-update strategy:

* **Usage::Immutable**: the texture is either created with image data and
cannot be changed later, or it is a render target texture which will only be
updated by the GPU
* **Usage::Stream**: the texture will not be created with image data and
will be updated by the CPU in each frame
* **Usage::Dynamic**: the texture will not be created with image data, but
will be updated by the CPU before first use, and after that only
infrequently (not each frame like Usage::Stream)

### Creating Textures

Texture objects are created like other Gfx resources:

* fill a **TextureSetup** with creation parameters
* optionally setup image data in memory
* call **Gfx::CreateResource()** and get a resource Id back

The returned texture resource Id is then either used as
input to create a render pass object, or plugged into 
one of the vertex- or fragment-texture bind slots 
of a DrawState structure.

#### Creating textures from data in memory

The following static creator method are used for partially 
initialize a TextureSetup object for creating a texture
from pixel data in memory:

```cpp
class TextureSetup {
    ...
    /// setup 2D texture from raw pixel data
    static TextureSetup FromPixelData2D(int w, int h, int numMipMaps, PixelFormat::Code fmt, const TextureSetup& blueprint=TextureSetup());
    /// setup cube texture from raw pixel data
    static TextureSetup FromPixelDataCube(int w, int h, int numMipMaps, PixelFormat::Code fmt, const TextureSetup& blueprint=TextureSetup());
    //// setup 3D texture from raw pixel data
    static TextureSetup FromPixelData3D(int w, int h, int d, int numMipMaps, PixelFormat::Code fmt, const TextureSetup& blueprint=TextureSetup());
    /// setup array texture from raw pixel data
    static TextureSetup FromPixelDataArray(int w, int h, int layers, int numMipMaps, PixelFormat::Code fmt, const TextureSetup& blueprint=TextureSetup());
}
```
> NOTE: the _blueprint_ parameter can be used to overwrite
> the default state of the returned TextureSetup pbject

When creating textures with pixel data, the usage hint is implicitely
set to Usage::Immutable.

Here is an example to setup a 2D 8-bit texture with a single 
mipmap, taken from the text renderer in the Dbg module:

```cpp
// fill a dynamic memory buffer with the pixel data
Buffer data;
uint8_t* dstPtr = data.Add(imgDataSize);
// ... actual image data setup omitted...

// setup texture, pixel format is 8bpp uncompressed
auto texSetup = TextureSetup::FromPixelData2D(imgWidth, imgHeight, 1, PixelFormat::L8);
texSetup.Sampler.MinFilter = TextureFilterMode::Nearest;
texSetup.Sampler.MagFilter = TextureFilterMode::Nearest;
texSetup.Sampler.WrapU = TextureWrapMode::ClampToEdge;
texSetup.Sampler.WrapV = TextureWrapMode::ClampToEdge;
texSetup.ImageData.Sizes[0][0] = imgDataSize;
Id tex = Gfx::CreateResource(texSetup, data);
```

You can also create a texture from a 'raw' pointer to the data and the
byte size of course:

```cpp
uint32_t pixels[128 * 128];
// ...omitted: fill the pixel buffer with data
auto texSetup = TextureSetup::FromPixelData2D(128, 128, 1, PixelFormat::RGBA8);
Id tex = Gfx::CreateResource(texSetup, pixels, sizeof(pixels));
```

If you want to setup a texture with multiple mipmaps, you need
to pass the number of mipmaps to TextureSetup::FromPixelData2D(), 
and initialize the ImageData struct with the offsets and sizes
of the mipmap surfaces:

```cpp
auto texSetup = TextureSetup::FromPixelData2D(imgWidth, imgHeight, numMips, PixelFormat::L8);
for (int mipIndex = 0; mipIndex < numMips; mipIndex++) {
    texSetup.ImageData.Offsets[0][mipIndex] = ...byte offset of mipmap data
    texSetup.ImageData.Sizes[0][mipIndex] =  ...byte size of mipmap data
}
```

#### Creating empty textures for dynamic update

The following static creator methods are used to setup empty,
dynamic textures:

```cpp
class TextureSetup {
    ...
    /// setup empty 2D texture
    static TextureSetup Empty2D(int w, int h, int numMipMaps, PixelFormat::Code fmt, Usage::Code usage, const TextureSetup& blueprint=TextureSetup());
    /// setup empty cube texture
    static TextureSetup EmptyCube(int w, int h, int numMipMaps, PixelFormat::Code fmt, Usage::Code usage, const TextureSetup& blueprint=TextureSetup());
    /// setup empty 3D texture
    static TextureSetup Empty3D(int w, int h, int d, int numMipMaps, PixelFormat::Code fmt, Usage::Code usage, const TextureSetup& blueprint=TextureSetup());
    /// setup empty array texture
    static TextureSetup EmptyArray(int w, int h, int layers, int numMipMaps, PixelFormat::Code fmt, Usage::Code usage, const TextureSetup& blueprint=TextureSetup());
}
```

Note that you have to provide a Usage hint (which shouldn't be Usage::Immutable, since
immutable textures without content don't make much sense).

Here is a code sample from the [YAKC emulator](https://github.com/floooh/yakc) to
setup the dynamic texture for the emulator framebuffer which is updated each
frame by the CPU:

```cpp
auto texSetup = TextureSetup::Empty2D(width, height, 1, PixelFormat::RGBA8, Usage::Stream);
texSetup.Sampler.MinFilter = TextureFilterMode::Linear;
texSetup.Sampler.MagFilter = TextureFilterMode::Linear;
texSetup.Sampler.WrapU = TextureWrapMode::ClampToEdge;
texSetup.Sampler.WrapV = TextureWrapMode::ClampToEdge;
this->texture = Gfx::CreateResource(texSetup);
```

#### Creating render target textures

Render target textures serve a dual role. They are used as input for render pass
objects, where they are rendered to by the GPU, and after the pass is finished 
they serve as normal textures sampled by fragment shaders.

A render target texture may own an additional internal depth/stencil buffer, and
it may have multisample-format for anti-aliased offscreen-rendering. In this case
some 3D-APIs require an 'MSAA-resolve' operation at the end of a render pass
into a separate 'resolve texture'. These complicated (and 3D-API specific) details 
are handled inside the Gfx module, the user-code mostly doesn't need to care
(but it helps to know what is happening under the hood of course).

Here is a code sample to create a 2D render target with color- and depth
buffer, taken from the [SimpleRenderTarget
sample](../../../Samples/SimpleRenderTarget/SimpleRenderTarget.cc), this
also checks if MSAA offscreen rendering is support, and if yes, creates a
multisampled render target texture:

```cpp
auto rtSetup = TextureSetup::RenderTarget2D(128, 128, PixelFormat::RGBA8, PixelFormat::DEPTH);
rtSetup.Sampler.WrapU = TextureWrapMode::Repeat;
rtSetup.Sampler.WrapV = TextureWrapMode::Repeat;
rtSetup.Sampler.MagFilter = TextureFilterMode::Linear;
rtSetup.Sampler.MinFilter = TextureFilterMode::Linear;
if (Gfx::QueryFeature(GfxFeature::MSAARenderTargets)) {
    rtSetup.SampleCount = 4;
}
Id rtTexture = Gfx::CreateResource(rtSetup);
```
 
Setting up a cubemap render target texture is just as simple (this is taken
from the [RenderToCubeMap sample](../../../Samples/RenderToCubeMap/RenderToCubeMap.cc):

```cpp
auto cubeMapSetup = TextureSetup::RenderTargetCube(1024, 1024, PixelFormat::RGBA8, PixelFormat::DEPTH);
cubeMapSetup.Sampler.MinFilter = TextureFilterMode::Linear;
cubeMapSetup.Sampler.MagFilter = TextureFilterMode::Linear;
this->cubeMap = Gfx::CreateResource(cubeMapSetup);
```

Creating a render target texture is only one half of the required steps for
offscreen rendering, the other half is to create a render pass object from
one or more render target textures. This is explained in detail
in the [Render Passes documentation](RenderPasses.md)

#### Loading textures from disk or the web

Oryol comes with a standard texture loader implementation in the Assets module
for loading 2D- and Cubemap textures from DDS, PVR and KTX container files. This
loader is also a good example for implementing your own texture loaders:

[See here](../../Assets/Gfx/TextureLoader.cc).

In the simplest case, loading a texture is just a one-liner, albeit a nested one ;)

```cpp
#include "Assets/Gfx/TextureLoader.h"

Id tex = Gfx::LoadResource(TextureLoader::Create(TextureSetup::FromFile("tex:bla.dds")));
```

_TextureSetup::FromFile("tex:bla.dds")_ creates a TextureSetup object with the
information to load the texture at path "tex:bla.dds" 
(check the [IO module documentation](../../IO/README.md) for more details on 
file paths.

_TextureLoader::Create()_ then creates a new texture loader object which
takes the TextureSetup object as input.

And finally _Gfx::LoadResource()_ takes the TextureLoader object as input, it will
immediate allocate and return a new texture resource Id, and will delegate the
rest of the asynchronous loading process to the resource loader object.

The returned Id can be used immediately for rendering, but as long as the texture
has not finished loading, all rendering operations involving this texture
objects will be silently dropped.

You can also check if a resource has finished loading yourself by inspecting
_Gfx::QueryResourceInfo().State_. This will return **ResourceState::Valid** when the
resource object is valid and ready for rendering, **ResourceState::Pending**
while the resource data is still loading, or **ResourceState::Failed**
when loading has failed (for instance because the file doesn't exist):

```cpp
if (Gfx::QueryResourceInfo(texId).State == ResourceState::Valid) {
    // resource is in a valid state and can be used for rendering
}
```

In reality, texture loading for multiple platforms often needs to look 
a bit more involved, since platform don't agree on a common 
texture compression format. In addition you can pass a TextureSetup
'blueprint' object with creation parameters (such as texture sampling
parameters) to the TextureLoader object. Here is the complete texture
loading code from the [Cubemap sample](../../../Samples/DDSCubeMap/DDSCubeMap.cc):

```cpp
TextureSetup texBluePrint;
texBluePrint.Sampler.MinFilter = TextureFilterMode::LinearMipmapLinear;
texBluePrint.Sampler.MagFilter = TextureFilterMode::Linear;
texBluePrint.Sampler.WrapU = TextureWrapMode::ClampToEdge;
texBluePrint.Sampler.WrapV = TextureWrapMode::ClampToEdge;
StringAtom texPath;
if (Gfx::QueryFeature(GfxFeature::TextureCompressionPVRTC)) {
    texPath = "tex:romechurch_bpp2.pvr";
}
else {
    texPath = "tex:romechurch_dxt1.dds";
}
Id texId = Gfx::LoadResource(
    TextureLoader::Create(TextureSetup::FromFile(texPath, texBluePrint))
);
```
