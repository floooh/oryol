## RenderPasses

Render passes hold references to one or more render-target textures and
apply 'pass actions' on them.

### Render Pass Overview

All Gfx::Apply*() and Gfx::Draw() functions must be called inside
a Gfx::BeginPass() / Gfx::EndPass() pair. Gfx::BeginPass() sets the
active render target(s) and executes a _PassAction_ on it (for instance
clearing the color render target to a specific color).

The Gfx::EndPass() method must be called at the end of a
rendering pass (before starting another pass, or before
the final call to Gfx::CommitFrame()).

There are 4 variants of the Gfx::BeginPass() functions, differing
on the accepted parameters:

```cpp
class Gfx {
    ...
    /// begin rendering to default render pass
    static void BeginPass();
    /// begin rendering to default render pass with override clear values
    static void BeginPass(const PassAction& action);
    /// begin offscreen rendering
    static void BeginPass(const Id& passId);
    /// begin offscreen rendering with override clear colors
    static void BeginPass(const Id& passId, const PassAction& action);
    ...
}
```

The first variant **Gfx::BeginPass()** without any arguments starts
rendering to the default render target (which is made visible during 
Gfx::CommitFrame()) and applies the _default pass action_ defined
in the GfxSetup structure handed to the Gfx::Setup() function.

The default pass action is to clear the color buffer to black,
and (if exists) the depth-stencil buffer to depth=1.0, stencil=0.

The second variant **Gfx::BeginPass(const PassAction& passAction)** overrides
the default pass action in the GfxSetup structure. This is useful if
you don't know the clear color yet when the application starts, or
if the clear values change dynamically.

The third variant **Gfx::BeginPass(const Id& passId)** takes
an object id of a render pass object. This is used when rendering
to offscreen render targets instead of the default render target.

Since no PassAction object is provided, the default pass action
will be applied that is part of the PassSetup structure when
the render pass object was created (which default to clearing
the color buffer(s) to black and depth/stencil buffer to 
depth=1.0 and stencil=0).

And the last variant **Gfx::BeginPass(const Id& passId, const PassAction& passAction)**
starts rendering to an offscreen render target, and overrides
the default pass action.

### Pass Actions

A pass action defines what happens when a pass becomes active
during Gfx::BeginPass():

FIXME: PassAction interface needs cleanup!

### Render Pass Creation

Render passes always require one or more texture objects as input
which have been created as render target textures. The general 
procedure for creating a render pass object thus looks like this

* create one or more render target textures
* fill a **PassSetup** object with creation parameters (the
render target textures created before and optional PassActions)
* call **Gfx::CreateResource()** and get a resource Id back

The returned resource Id is passed to Gfx::BeginPass() calls, while
the render target texture Ids used for the pass creation can be plugged
as usual into a DrawState and bound for rendering via
_Gfx::ApplyDrawState()_.

The PassSetup class has 2 static creator method, the first initializes
the PassSetup object for a single color-buffer, and an optional 
depth-stencil-buffer, the second takes an initializer-list for 
multiple color-buffers, and a single, optional depth-stencil-buffer
(rendering to multiple color-buffers is called **MRT** rendering, MRT stands
for Multiple-Render-Target):

```cpp
/// construct from single render target textures, and optional depth-stencil texture
static PassSetup From(Id colorTexture, Id depthStencilTexture=Id::InvalidId());
/// construct from MRT render target textures, and option depth-stencil texture
static PassSetup From(std::initializer_list<Id> colorTextures, Id depthStencilTexture=Id::InvalidId());
```

### Create a render pass with color and depth/stencil buffer

This is the most typical scenario for offscreen rendering, 
rendering to a single color- and depth/stencil buffer. Only 
a single texture object is used in this case, the texture
object contains both the color-buffer texture as well as a
depth/stencil-buffer:

```cpp
auto rtSetup = TextureSetup::RenderTarget2D(128, 128, 
    PixelFormat::RGBA8, 
    PixelFormat::DEPTHSTENCIL);
Id rtTexture = Gfx::CreateResource(rtSetup);
auto rpSetup = PassSetup::From(rtTexture, rtTexture);
this->renderPass = Gfx::CreateResource(rpSetup);
```

The default PassAction in the PassSetup object was not modified,
this means the color buffer will be cleared to black, and the
depth/stencil buffer to 1.0 and 0.

The following example is identical, except that the default
PassAction is changed to clear the color buffer to red:

```cpp
auto rtSetup = TextureSetup::RenderTarget2D(128, 128, 
    PixelFormat::RGBA8, 
    PixelFormat::DEPTHSTENCIL);
Id rtTexture = Gfx::CreateResource(rtSetup);
auto rpSetup = PassSetup::From(rtTexture, rtTexture);
rpSetup.DefaultAction = PassAction::Clear(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
this->renderPass = Gfx::CreateResource(rpSetup);
```

### Create passes for rendering to cubemap faces

To render to the faces of a cube map texture, create a single
cubemap render-target texture, and 6 render passes:

```cpp
// create a cubemap render-target-texture
auto cubeMapSetup = TextureSetup::RenderTargetCube(1024, 1024,
    PixelFormat::RGBA8, 
    PixelFormat::DEPTH);
Id cubeMap = Gfx::CreateResource(cubeMapSetup);

// create 6 render passes, one for each cubemap-face
Id passes[NumFaces];
auto rpSetup = PassSetup::From(cubeMap, cubeMap);
for (int faceIndex = 0; faceIndex < NumFaces; faceIndex++) {
    rpSetup.ColorAttachments[0].Slice = faceIndex;
    passes[faceIndex] = Gfx::CreateResource(rpSetup);
}
```

### Create passes for rendering to array-texture layers

This looks very similar to the cubemap code, but with array-texture
layers instead of cubemap-faces:

```cpp
// create an array texture with 3 layers
static const int NumLayers = 3;
auto texSetup = TextureSetup::RenderTargetArray(256, 256, NumLayers,
    PixelFormat::RGBA8,
    PixelFormat::DEPTH);
Id arrayTexture = Gfx::CreateResource(texSetup);

// create 3 render passes, one for each texture slice
Id passes[NumLayer];
auto rpSetup = PassSetup::From(arrayTexture, arrayTexture);
for (int layerIndex = 0; layerIndex < NumLayers; layerIndex++) {
    rpSetup.ColorAttachments[0].Slice = faceIndex;
    passes[layerIndex] = Gfx::CreateResource(rpSetup);
}
```

### Create a render pass for Multiple-Render-Target rendering

This attached 3 render-target-textures as color buffers to a single
render pass. The first render-target-texture also provides the depth/stencil-buffer:

```cpp
// create 3 render target textures, the first with depth-stencil buffer:
auto rtSetup = TextureSetup::RenderTarget2D(128, 128, 
    PixelFormat::RGBA8,
    PixelFormat::DEPTHSTENCIL);
Id rt0 = Gfx::CreateResource(rtSetup);
rtSetup.DepthFormat = PixelFormat::None;
Id rt1 = Gfx::CreateResource(rtSetup);
Id rt2 = Gfx::CreateResource(rtSetup);

// create a render pass with 3 color attachments, and 1 depth/stencil attachment
auto passSetup = PassSetup::From({ rt0, rt1, rt2 }, rt0);
Id renderPass = Gfx::CreateResource(passSetup);
```
