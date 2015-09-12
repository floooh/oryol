## Oryol Gfx Resource Tour

Disclaimer: Please note that I'm still quite the D3D12 noob. 
The D3D12 renderer is still work-in-progress, and work on
a Vulkan renderer hasn't started yet. I may still be using the D3D12 API
'the wrong way' here and there.

### Oryol and the new 3D APIs

The Oryol Gfx module tries to find the right balance between the following 
3 requirements:

* it should be a **simple** medium level API
* it must remain compatible with GLES2/WebGL
* but it should also map well to modern 3D APIs

I'm planning to create one Oryol renderer backend for each of current 3D APIs
(GL/GLES2/WebGL, D3D11, Metal, D3D12, Vulkan). The D3D12 and Vulkan renderers
will be more like an experiment, pragmatically I don't expect huge differences
in performance, but at least cleaner code and a learning experience :)

The following important features of the new 3D APIs are currently not used:

* command lists will not be exposed to the outside, making multi-threaded
rendering impossible
* no compute capabilities will be exposed
* less flexible shader resource usage, as dictated by GLES2/WebGL

The most useful feature in the future will be to expose a wrapper for command
lists. This would probably even make sense for WebGL, because render commands
could be assembled on one or more WebWorker threads which might have some
advantages even though the command lists need to be decoded and 'executed' on
the main thread.

The main advantage of using the new 3D APIs in Oryol comes from the use of
precompiled state. All graphics resources and their relationships need to be
defined up-front. This makes rendering less flexible (you cannot do
'incremental state changes' between drawcalls except for groups of shader
uniforms and textures), but the advantage is that no dynamic 'recompilation'
of state needs to happen between draw calls in the 3D API or GPU driver.

To map somewhat well to the new 3D APIs, the Oryol Gfx API had to be
changed slightly in the past months, and will continue to change until at least
the Vulkan rendering backend has been implemented. But I think the end-result
will still be a very simple medium-level 3D API which nicely bridges the
gap between 'old' and 'new'.


### Gfx Resource Types

Oryol has the following types of Gfx resources:

* **mesh**: provides geometry data
* **shader**: provides vertex- and fragment-shaders and shader-resource binding information
* **draw state**: basically all static state needed for issuing a draw call
* **texture**: a single texture, optionally usable as a render target
* **texture block**: a group of related textures that are set in one call
* **uniform block**: a group of related shader uniforms that are set in one call

Resources in Oryol are created by calling one of the Gfx::CreateResource() or
Gfx::LoadResource() functions with a setup-object that describes how the
resource should be created, similar to the DESC structs in D3D or the
descriptor objects in Metal.

The creation functions return an opaque 64 bit resource id which is basically
an index into one of the internal resource pools.

Resource creation can happen deferred under the hood for asynchronously loaded
resources. For instance a texture could take a little while until it is
ready for use because it is loading in the background. The Gfx user doesn't
need to be concerned about this, the returned resource id can be used right
away. If there are any resources used for rendering that have not finished
loading yet, the draw calls depending on it will be ignored (this is about the
only tracking that needs to take place under the hood).

A simple render-loop could look like this:

* for each render pass:
    * set render target
    * for each 'shape':
        * set draw-state object, this implicitely also sets shaders and input geometry
        * set per-material texture- and uniform-blocks
        * for each instance:
            * compute and update per-instance uniform values
            * call one of the draw-functions
* finally: mark the render frame as complete

Each of these steps is a single call into the Gfx API, so rendering code
can look quite clean:

```cpp
    Gfx::ApplyDefaultRenderTarget(clearState);
    for (const auto& shape: shapes) {
        Gfx::ApplyDrawState(shape.drawState);
        Gfx::ApplyUniformBlock(shape.materialUniforms);
        Gfx::ApplyTextureBlock(shape.materialTextures);
        for (const auto& inst : shapeInstances) {
            inst.uniforms.mvp = ...;
            Gfx::ApplyUniformBlock(inst.uniforms);
            Gfx::Draw(0);
        }
    }
    Gfx::CommitFrame();
```

Note that there is no Gfx::Clear() call to clear the current render target
(there used to be one up until the Metal renderer backend was implemented).
Now, the clear actions happen implicitely when a render target is applied. The
'ClearState' is a simple C struct with all the necessary information (if and
what to clear, and the color- and depth/stencil-clear-values).

The Gfx::ApplyDrawState() call sets the precompiled pipeline state along
with the shader and one or more meshes which provide the input geometry.

The only thing left before a draw call is to set uniform- and texture-blocks,
which can change between draw calls.

Finally the call to Gfx::CommitFrame() marks the current frame as finished.

Except for the Gfx::ApplyUniformBlock() call, all other Gfx::ApplyXXX() calls 
take resource ids as parameter which point to 'pre-compiled' resource objects.

The Gfx::ApplyUniformBlock() call is slightly different, since it only
takes a const-ref to a simple C struct in memory which contains uniform
values.


### Meshes

Mesh objects describe geometry needed for rendering. They are not set directly
in the Gfx API but are only used as input parameters when creating draw-states.

The following information is provided when creating a mesh:

* a vertex-layout, describing the components of a vertex
  (vertex attribute slots and data types)
* [optional] a primitive type for the whole mesh (triangle list 
  or strip, line list or strip, point list)
* [optional] an array of primitive groups describing 'sub-mesh-ranges'
* the type of index data (none, 16-bit or 32-bit)
* usage hints for vertex and index data (immutable vs dynamic)
* [optional] pointers to vertex and index data to initialize the mesh with
* [optional] a 'vertex-step-rate' and 'vertex-step-function' which is used for
  hardware-instanced rendering

The primitive-type was originally part of a primitive-group, so that the 
same mesh could contain triangle-lists, line-lists, etc... This was
changed for D3D12 which needs to know in the pipeline-state-object whether
triangles, lines or points are rendered.

#### What happens on mesh creation:

**GL/GLES2/WebGL:**

* up to 2 calls to glGenBuffers()
* up to 2 calls to glBufferData() if initialization data was provided

**D3D11:**

* up to 2 calls to ID3D11Device::CreateBuffer()

**Metal:**

* one call each [MTLDevice newBufferWithBytes:length:options] or 
[MTLDevice newBufferWithLength:options] for vertex- and optional index-buffer,
depending on whether initialization-data was provided or not

**D3D12:**

* for the vertex buffer and optional index buffer:
    * a call to ID3D12Device::CreateCommittedResource() on the default heap
    * if data is provided:
        * a call to ID3D12Device::CreateCommittedResource() on the upload heap
        * map upload buffer, copy data, and unmap
        * call ID3D12GraphicsCommandList::CopyBufferRegion() to initiate upload from
          upload buffer into default buffer
        * put the upload buffer into a deferred release queue to be released 
          N frames later when no longer in use by GPU
    * a call to ID3D12GraphicsCommandList::ResourceBarrier() for state transition of the default
      heap buffer from copy-dest to use as vertex/constant/index buffer

This D3D12 resource initialization is (AFAIK) the most basic way to initialize
a buffer resource and details may change in the future (for instance it
probably makes sense to pre-allocate a large default heap and sub-allocate from
there), but it nicely shows the toll the programmer has to pay for getting more
detailed control over resource creation.


### Shaders

A shader object contains a pair of vertex- and fragment-shaders as well
as resource binding information for uniform- and texture-blocks.

Just like meshes, shader objects are not used directly for rendering, but
only serve as input for the creation of draw-state- and texture-block-objects.

To create a shader object, the following information needs to be provided:

* shader source-code (on GL) or byte-code (all other 3D APIs) for one vertex-shader 
  and one fragment-shader
* detailed resource-binding-information for the vertex- and fragment-shader stage:
    * up to 4 uniform-block-layouts per shader stage
    * up to 4 texture-block-layouts per shader stage

Each of the 2 shader stages (vertex- and fragment-shader) has 4 binding slots
for uniform-blocks and 4 binding-slots for texture-blocks.

These binding-slots are basically the D3D12 root signature. The number of 
binding slots is small enough to fit into the root signature (hopefully...),
and big enough to allow the most important update-frequencies (e.g. per frame,
per material, per draw-call).

A uniform-block-layout describes the names and types of 
up to 16 uniform values in a single uniform block (note: there is currently
no support for uniform arrays, I haven't made up my mind about these yet).

A texture-block-layout describes the names, types and texture-bind-slots of 
up to 16 textures.

Even though it is possible to setup shader objects manually in C++, the common
way is through the shader code generator that's integrated into the Oryol
build process:

* shader source is written in an 'annotated GLSL-style' syntax where annotation
tags are used for parts of the shader code that differ between
target shader languages (for instance uniform and texture declarations, or
the linkage between vertex- and fragment-shaders)
* when the shader code generator runs during the build process it will:
    * generate target shader code for different GLSL versions, HLSL5 and Metal
    * run the GLSL code through the Khronos glslangValidator to check for errors
    * compile the generated HLSL5 and Metal source code into byte code
    * generate C++ source code:
        * one C struct per uniform-block which maps the uniform-block members
          for writing from the C++ side
        * one uniform-block-layout per uniform-block 
        * one texture-block-layout per texture-block
        * embedded C strings or C arrays with the shader source-code or byte-code
        * a ShaderSetup object with pointers to the shader source- or byte-code,
          and the uniform- and texture-block-layouts

#### What happens on shader creation:

**GL/GLES2/WebGL**:

* the vertex- and fragment-shader source is compiled and linked:
    * glShaderSource() for vertex- and fragment-shader
    * glCompileShader() for vertex- and fragment-shader
    * ..check GL_COMPILE_STATUS
    * glCreateProgram()
    * glAttachShader() for compiled vertex- and fragment-shader
    * up to 16x glBindAttribLocation() for every vertex attribute
    * glLinkProgram()
    * glDeleteShader() for vertex- and fragment-shader
    * check GL_LINK_STATUS
* for each uniform in each uniform-block, and each texture in each texture-block,
  glGetUniformLocation() is called to obtain the uniform bind locations, and 
  then stored in the shader object for later use during rendering

**D3D11**:

* D3D vertex- and pixel-shader objects are created from the embedded byte-code
  by calling ID3D11Device::CreateVertexShader() and ID3D11Device::CreatePixelShader()
* for each uniform-block, a constant buffer is created via ID3D11Device::CreateBuffer()

**Metal**:

* one shader-library is created per Oryol shader object via 
  [MTLDevice newLibraryWithData:error:] which contains the compiled shader byte
  code from the shader-code-generator
* the vertex- and fragment-shader entry-points are looked up via 
  [MTLLibrary newFunctionWithName:]

In Metal, there is one big global buffer for shader-uniform data which is 
updated on-the-fly during rendering (one memcpy per assigned uniform-block).

**D3D12**:

* no D3D12 calls are happening, only pointers to the shader-byte-code
  are stored in the Oryol shader object

Similar to Metal, there is only one global constant buffer for the 
shader-uniform updates.

### DrawState objects

Draw-state objects is where the interesting stuff happens, these contain
all the render-states, define which meshes provide the input geometry data
and what shaders to use for the following draw-calls.

To create a draw-state object, the following information needs to be provided:

* 1 previously created shader object
* 1..4 previously created mesh objects
* static alpha-blending render-state grouped in a BlendState struct
* static depth-stencil render-state grouped in a DepthStencilState struct
* static rasterizer render-state grouped in a RasterizerState struct

The 4 mesh slots can be used to setup interesting geometry input scenarios like
hardware-instanced rendering where one mesh provides the render-geometry and
one or more other meshes provide per-instance data in their vertex buffers, or
one mesh may provide static vertex data and one or more other meshes
provide dynamically updated vertex components streams.

There are a few notable changes to accomodate restrictions in some of the
new 3D APIs:

* blend-color and stencil-ref values are compiled into the draw state and 
  cannot be changed independently
* the rasterizer-state needs to know the MSAA-sample-count of the render-targets
  it will be used with
* the blend-state needs to know the color-buffer and depth-buffer pixel-formats
  of the render-targets it will be used with

These restrictions are a major reason why the new 3D APIs may be hard to
support in existing engines, and they may lead to 'combinatorial explosions',
but on the other hand, these are examples where state might have to be recompiled
during rendering which are now avoided (NOTE: it might be that the blend-color
can be moved out into a separate call again)
  
How draw-state objects are created and used in the different 3D APIs varies 
drastically:

#### What happens during draw-state creation and rendering:

**GL/GLES2/WebGL**:

at draw-state creation:

* no GL calls are made
* only a combined array of the vertex attribute elements of the input meshes 
  is created to simplify the glVertexAttribPointer calls later during rendering

in Gfx::ApplyDrawState():

* the BlendState, DepthStencilState and RasterizerState is applied through
  many granular GL calls, but filtered through a state cache:
    * each of the state structs is actually a union of bitfields, with the 
      minimal number of bits to represent each render-state, and one 'hash'
      value which is the union of all state bits
    * if the coarse hash values for BlendState, DepthStencilState and 
      RasterizerState haven't changed, no GL calls need to be made
    * otherwise, each contained GL state is checked and updated, worst case is:
        * 9 GL calls to update depth-stencil-state
        * 5 GL calls to update blend-state
        * 6 GL calls to update rasterizer-state
* one call to glUseProgram if the shader has changed
* bind the input geometry, everything here also goes through the state cache,
  so this can vary between draw calls:
    * up to one glBindBuffer(GL_ARRAY_BUFFER,...) call per input mesh
    * up to 16 calls to glVertexAttribPointer()
    * up to 16 calls to glDisableVertexAttribArray() or glEnableVertexAttribArray()
      (usually these calls cache very well)

**D3D11**:

at draw-state creation:

* ID3D11Device::CreateInputLayout(), but only if the same vertex layout hadn't 
  been encountered before
* the coarse state objects are created:
    * ID3D11Device::CreateRasterizerState()
    * ID3D11Device::CreateDepthStencilState()
    * ID3D11Device::CreateBlendState()
* note that D3D11 has an internal cache of those state objects and returns
  the same state object for previously encountered state-combinations, so 
  we do not need to do our own caching here

in Gfx::ApplyDrawState():

* all calls are on the ID3D11DeviceContext and filtered through a state cache
* apply state objects:
    * RSSetState() for the rasterizer-state
    * OMSetDepthStencilState() for the depth-stencil-state
    * OMSetBlendState() for the blend-state
* apply mesh state:
    * one call to IASetVertexBuffers() (even if several input meshes)
    * IASetPrimitiveTopology()
    * IASetIndexBuffer()
    * IASetInputLayout()
* apply shaders and constant buffers
    * VSSetShader()
    * PSSetShader()
    * 1..4 calls to VSSetConstantBuffers
    * 1..4 calls to PSSetConstantBuffers


**Metal**:

at draw-state creation:

* one call to [MTLDevice newDepthStencilStateWithDescriptor:]
* one call to [MTLDevice newRenderPipelineStateWithDescriptor: error:]

in Gfx::ApplyDrawState:

* there's currently no separate state-caching here, needs to be implemented!
* [MTLRenderCommandEncoder setBlendColorRed:green:blue:alpha]
* [MTLRenderCommandEncoder setCullMode:]
* [MTLRenderCommandEncoder setRenderPipelineState:]
* [MTLRenderCommandEncoder setDepthStencilState:]
* for each of the 4 mesh slots:
    * [MTLRenderCommandEncoder setVertexBuffer:offset:atIndex], for empty 
      mesh bind slots, this is called with a nil vertex-buffer

**D3D12**:

at draw-state creation:

* one call to ID3D12Device::CreateGraphicsPipelineState()

What's currently missing here is a pipeline-state cache to re-use PSOs with
the same configuration.

in Gfx::ApplyDrawState():

* all calls are currently *not* going through a state-cache
* ID3D12GraphicsCommandList::SetPipelineState()
* ID3D12GraphicsCommandList::IASetVertexBuffers() (once, even if several input meshes)
* ID3D12GraphicsCommandList::IASetIndexBuffer()
* ID3D12GraphicsCommandList::IASetPrimitiveTopology()
* ID3D12GraphicsCommandList::OMSetBlendFactor()

### Uniform Blocks

As mentioned previously, uniform-blocks are a bit special because they don't
have a 'baked' resource object behind them, instead they are exposed to the
application code as simple C-structs generated by the shader code generator and
then applied with Gfx::ApplyUniformBlock() between calls to Gfx::ApplyDrawState()
and one of the draw-call functions.

#### What happens during Gfx::ApplyUniformBlock()

**GL/GLES2/WebGL**:

* for each of the up to 16 uniform block members, one of the following GL
  functions is called:
    * glUniform[1,2,3,4]f()
    * glUniformMatrix[2,3,4]fv()
    * glUniform1i()

The uniform locations had been looked up before during shader creation, so
there's only a simple mapping taking place between a 'bind slot index' and 
actual GL uniform location index.

**D3D11**:

* a single call to ID3D11DeviceContext::UpdateSubResource()

**Metal**:

* a memcpy() into the next free position of the global uniform buffer, and...
* one call to either [MTLRenderCommandEncoder setVertexBuffer:offset:atIndex]
  or [MTLRenderCommandEncoder setFragmentBuffer:offset:atIndex] with the
  position of the uniform block data inside the global uniform buffer

**D3D12**:

Much like the Metal implementation:

* a memcpy() into the next free position of the global uniform buffer, and...
* a call to ID3D12GraphicsCommandList::SetGraphicsRootConstantBufferView() to
  directly set the global uniform buffer location into the root signature

### Textures

Texture objects come in 2 flavours: as input data for rendering, or as 
render targets, optionally with a depth buffer attached. Depth buffers
can be shared between several render target texture objects (this is useful
if there are multiple rendering passes that need to use the same depth buffer).

In Oryol, sampler state is part of the texture object, just like in GLES2/WebGL,
in the future this might be split up into separate texture and sampler objects.

To create texture objects, the following information is provided:

* the texture type (2D or Cube, no 3D or Array textures are currently supported)
* width and height
* number of mipmap surfaces
* the color pixel format
* texture address wrap modes (ClampToEdge, Repeat, MirroredRepeat)
* min and mag texture filter modes in GL style (Nearest, Linear, NearestMipmapLinear, ...)
* a flag whether the texture is a render target
* an optional depth-buffer pixel-format
* an optional shared-depth-buffer resource id
* optional initialization data as one pointer, and offsets and sizes of 
  mip-surface data

Currently, no 'multiple-render-targets' are supported.

The only Gfx function that directly accepts a texture resource id is 
Gfx::ApplyRenderTarget():

#### What happens at creation and in Gfx::ApplyRenderTarget():

**GL/GLES2/WebGL**:

at texture creation:

* if a non-render-target texture:
    * glGenTextures(), glActiveTexture() and glBindTexture()
    * 4 calls to glTexParameteri() to set the sampler filter and uv-wrap modes
    * if data is provided, for each mipmap surface:
        * either a call to glCompressedTexImage2D()...
        * or a call to glTexImage2D()
* if a render-target texture:
    * glGenFramebuffers(), glBindFramebuffer()
    * glGenTextures(), glActiveTexture(), glBindTexture()
    * 4x glTexParameteri() to set sampler filter and uv-wrap modes
    * glTexImage2D() to set the pixel format, width and height
    * glFramebufferTexture2D() with GL_COLOR_ATTACHMENT0
    * if a depth-buffer must be created:
        * glGenRenderbuffers(), glBindRenderbuffer()
        * glRenderbufferStorage() to define width, height and depth-pixel-format
        * glFramebufferRenderbuffer() to attach the depth buffer to the frame buffer
        * optionally another glFramebufferRenderbuffer() to attach the stencil buffer
          to the frame buffer
    * a glCheckFramebufferStatus() to check for framebuffer completeness

in Gfx::ApplyRenderTarget():

* glBindFramebuffer() is called
* a call to glViewport() to reset the view-port to cover the whole screen
* a call to glDisable(GL_SCISSOR_TEST)
* depending on the provided ClearState any or none of the following is called:
    * a call to glColorMask(), glDepthMask() and glStencilMask() to enable all 
      color and depth/stencil channels
    * calls to glClearColor(), glClearDepth(f)(), glClearStencil() to set 
      the clear values
    * a final call to glClear() to actually perform the clear

**D3D11**:

at texture creation:

* for both render-target and 'normal' textures:
    * ID3D11Device::CreateTexture2D()
    * ID3D11Device::CreateShaderResourceView()
    * ID3D11Device::CreateSamplerState()
* additionally for render-target-textures:
    * ID3D11Device::CreateRenderTargetView()
    * if a depth-buffer must be created:
        * ID3D11Device::CreateTexture2D()
        * ID3D11Device::CreateDepthStencilView()

Note that D3D11 returns the same sampler-state object for the same
creation-parameters, so we don't need to implement this on our own!

in Gfx::ApplyRenderTarget():

* a call to ID3D11DeviceContext::OMSetRenderTargets()
* a call to ID3D11DeviceContext::RSSetViewports() to reset the viewport to the 
  whole screen
* depending on the provided ClearState any or no calls to:
    * ID3D11DeviceContext::ClearRenderTargetView()
    * ID3D11DeviceContext::ClearDepthStencilView()

**Metal**:

at texture creation:

* for non-render-target textures:
    * initialize an MTLTextureDescriptor
    * create an MTLTexture through [MTLDevice newTextureWithDescriptor:]
    * if provided, copy pixel data into texture with one call per
      face (for cube-textures) and mip-level to 
      [MTLTexture replaceRegion: ...]
    * initialize an MTLSamplerDescriptor
    * create an MTLSamplerState through [MTLDevice newSamplerStateWithDescriptor:]
* additionally for render-target-textures if a depth-buffer is needed:
    * another call to [MTLDevice newTextureWithDescriptor:]

in Gfx::ApplyRenderTarget():

* the previous render pass (if any) is finished calling 
  [MTLRenderCommandEncoder endEncoding]
* an MTLRenderPassDescriptor is initialized
* a new command encoder is created via 
  [MTLCommandBuffer renderCommandEncoderWithDescriptor:]

**D3D12**:

this is work in progress!

at texture creation: not implemented yet!

in Gfx::ApplyRenderTarget():

* ID3D12GraphicsCommandList::ResourceBarrier() is called to transition the
  texture into 'render target state'
* ID3D12GraphicsCommandList::OMSetRenderTargets() is called
* viewport and scissor rects are reset
* depending on the provided ClearState:
    * ID3D12GraphicsCommandList::ClearRenderTargetView()
    * ID3D12GraphicsCommandList::ClearDepthStencilView()


### Texture Blocks

Texture blocks group a number of related textures into an opaque object
in order to apply multiple textures with a single call.

One texture block may contain up to 16 textures, and can be applied to one
of 4 bind slots either on the vertex-shader or fragment-shader stage.

To create a texture-block object, the following input data must be provided:

* a previously created shader object with texture-block-layouts
* the shader stage and bind slot this texture block will be bound to
* up to 16 previously created texture objects set in the 'texture-bind-slots'

At creation time, the texture-layouts in the provided shader object
will be used to check whether the provided textures are type-compatible
(for instance, if a 2D texture is assigned to a texture-slot which expects
a cube-texture, the texture-block creation will fail).

#### What happens on creation and during Gfx::ApplyTextureBlock():

**GL/GLES2/WebGL**:

at texture-block creation:

* no GL calls are made, only the GL object ids and GL bind slots are 
  gathered and the texture types are tested for compatibility with the 
  shader object

in Gfx::ApplyTextureBlock():

* for each texture in the texture-block, one call to glActiveTexture()
  and glBindTexture() is made

**D3D11**:

at texture-block creation:

* no D3D11 calls are made, only the D3D shader-resource-view and sampler-state
  object pointers are gathered and texture-type-compatibility is checked

in Gfx::ApplyTextureBlock():

* textures and samplers are only set on the device-context if they have changed
* if the texture-block is bound to the vertex-shader stage:
    * for each texture in the texture block:
        * ID3D11DeviceContext::VSSetShaderResources()
        * ID3D11DeviceContext::VSSetSamplers()
* if the texture-block is bound to the fragment-shader stage:
    * for each texture in the texture block:
        * ID3D11DeviceContext::PSSetShaderResources()
        * ID3D11DeviceContext::PSSetSamplers()

Note: these multiple calls should probably be reduced to a single call for
shader resources and sampler states each.

**Metal**:

at texture-block creation:

* no Metal calls are made, only the MTLTexture and MTLSamplerState pointers
  are gathered, and the texture types are checked for compatibility with the
  texture-block-layouts in the provided shader

in Gfx::ApplyTextureBlock():

* if the texture-block is bound to the vertex-shader stage:
    * for each texture in the texture-block:
        * [MTLRenderCommandEncoder setVertexTexture:atIndex:]
        * [MTLRenderCommandEncoder setVertexSamplerState:atIndex]
* if the texture-block is bound to the fragment-shader stage:
    * for each texture in the texture-block:
        * [MTLRenderCommandEncoder setFragmentTexture:atIndex:]
        * [MTLRenderCommandEncoder setFragmentSamplerState:atIndex:]

**D3D12**

Texture blocks have not yet been implemented for D3D12!

