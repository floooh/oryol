## Gfx Module

The Oryol Gfx module provides a thin portability wrapper around a 
platform's native 3D APIs, with WebGL/OpenGLES2 on the low-end, and
D3D12 and (hopefully soon) Vulkan on the 'high-end'.


### Platform/API matrix

What 3D APIs are supported on which platforms:

Platform   |GL3.3|GLES2|Metal|D3D11|D3D12|Vulkan
-----------|-----|-----|-----|-----|-----|------
OSX 10.11+ |YES  |---  |YES  |---  |---  |---
iOS 9.x+   |---  |YES  |YES  |---  |---  |---
Window7+   |YES  |---  |---  |YES  |---  |MAYBE
Window10+  |YES  |---  |---  |YES  |YES  |MAYBE
Linux      |YES  |---  |---  |---  |---  |MAYBE
Android    |---  |YES  |---  |---  |---  |MAYBE
HTML5      |---  |YES  |---  |---  |---  |---
PNaCl      |---  |YES  |---  |---  |---  |---
RaspberryPi|---  |YES  |---  |---  |---  |---


### The Gfx Functions

(TODO: this should go further down?)

All rendering is controlled through the Gfx facade class declared
in the header **"Gfx/Gfx.h"**. The Gfx facade class offers a number
of static methods which fall in the following feature groups:

#### Setup and Shutdown

* **Gfx::Setup()**: initializes the Gfx module
* **Gfx::Discard()**: shutdown the Gfx module and discard any remaining resources

#### Resource Management

* **CreateResource()**: create a resource from data in memory
* **LoadResource()**: asynchronously load a resource
* **PushResourceLabel()**: push a new resource label on the label stack
* **PopResourceLabel()**: pop top-most resource label from label stack
* **DestroyResources()**: destroy all resources with matching resource label
* **LookupResource()**: lookup a resource by name

#### Applying State

* **Gfx::ApplyDefaultRenderTarget()**: make the default-render-target
  (==backbuffer) current and optionally peform a clear-operation
* **Gfx::ApplyRenderTarget()**: make an offscreen-render-target current and
  optionally perform a clear-operation
* **Gfx::ApplyViewPort()**: set current view port (performs all rendering
  within the defined rectangular area)
* **Gfx::ApplyScissorRect()**: defines the current scissor rect (clips all
  rendering to the defined rectangular area) also requires the
  **RasterizerState::ScissorTestEnabled** state to be set
* **Gfx::ApplyDrawState()**: sets the entire state required for the following
  draw calls
* **Gfx::ApplyUniformBlock()**: sets shader-parameters for the next draw call(s)

#### Drawing Functions

* **Gfx::Draw()**: performs a draw-call to render a group of primitives using
  the shader defined by the currently active DrawState
* **Gfx::DrawInstanced()**: performs a draw-call using D3D9-style
  instanced-rendering

#### Updating Dynamic Resources

* **UpdateVertices()**: update the vertex data in a dynamic Mesh resource
* **UpdateIndices()**: update the index data in a dynamic Mesh resource
* **UpdateTexture()**: update pixel data in a dynamic Texture resource

#### Getting Information

NOTE: the resource-info methods will most likely change in the future

* **Gfx::QueryFeature()**: test if an optional rendering feature is supported
* **Gfx::QueryResourceInfo()**: get the current loading-state of a resource
* **Gfx::QueryFreeResourceSlots()**: get the number of free resource slots in a
  resource pool 
* **Gfx::QueryResourcePoolInfo()**: get information about a resource pool
* **Gfx::GfxSetup()**: get the GfxSetup object which was used to initialize the
  Gfx module
* **Gfx::DisplayAttrs()**: get information about the default frame-buffer (most
  importantly the backbuffer's rendering resolution)
* **Gfx::RenderTargetAttrs()**: get information about the currently active render
  target
* **Gfx::ReadPixels()**: read-back the color pixels of the current render-target,
  this causes a pipeline-stall and is currently only supported in the GL
  rendering-backend

### Frame Rendering

The following steps are necessary to render something with the Gfx module:

1. [Initialize the Gfx Module](#initialize-the-gfx-module)
2. [Create Resources](#resources)
3. For each frame:
    1. [Apply Render Target and optionally Clear](#render-targets)
    2. [Optional: Update Dynamic Resources](#dynamic-resources)
    4. [Apply a DrawState](#drawstates)
    5. [Draw](#draw-functions)
    6. [Commit](#committing-the-frame)
4. [Shutdown the Gfx Module](#shutting-down)

### Initializing the Gfx Module

### Resources

#### Resource Types

1. [Meshes](#meshes)
2. [Textures](#textures)
3. [Shaders](#shaders)
4. [DrawStates](#drawstates)

#### Resource Management
TODO

#### Meshes
TODO

#### Textures
TODO

#### Render Targets
TODO

#### Dynamic Resources
TODO

#### Shaders
TODO

#### DrawStates
TODO

### Draw Functions
TODO

### Committing a Frame
TODO

### Shutting Down
TODO

