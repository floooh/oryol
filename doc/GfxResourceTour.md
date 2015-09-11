## Oryol Gfx Resource Tour

The Oryol Gfx module tries to find the right balance between the following 
3 requirements:

* it should be a *simple* medium level API
* it must remain compatibel with OpenGLES2/WebGL
* it should map well to modern 3D APIs

I'm planning to create a rendering backend for all current 3D APIs, for D3D12
and Vulkan this is more an experiment, and doesn't make a lot of sense
from a pragmatic point of view because important improvements in those 
APIs are currently not used:

* command lists are not exposed in the Oryol Gfx API, all rendering
commands are issued from the main thread
* no compute capabilities
* shader resource usage is less flexible, limited by GLES2/WebGL

But Oryol makes use of the new 'everything is baked' philosophy of the
modern 3D APIs, in Oryol, all graphics resources and their relationships
are defined upfront. For this the Gfx module API had to be slightly 
changed in the past months, and will continue to change until at least the
Vulkan rendering backend has been implemented, but I think the result is still
a very simple API which bridges the gap between 'old' and 'new'.

#### Gfx Resource Types

Oryol has the following Gfx resource types:

* *mesh*: provides geometry data
* *shader*: a linked shader program plus shader resource binding information
* *draw state*: all static render pipeline state, plus shader and or several meshes
* *texture*: a single texture, which can also be used as a render target
* *texture block*: a group of related textures that are set together on a shader stage
* *uniform block*: a group of related shader uniforms that are set together on a shader stage

Resources in Oryol are created by calling a Create function with a setup object
that describes how the resource should be created. This is similar to the D3D DESC 
structs, or Metal's descriptor objects, and often maps directly to those.

The Creation function returns an opaque 64 bit resource id (this is made of
a 16-bit resource type, a 16-bit resource pool index, and a 32-bit unique 
counter to prevent 'dangling pointers' to freed resources.

Resource creation can be deferred under the hood for asynchronously loaded
resources. For instance a texture could take a little while until it is
ready for use because it is loading in the background. The Gfx user doesn't
need to be concerned about this, the returned resource id can be used right
away. If there are any resources used for rendering that have not finished
loading yet, the following draw calls will be ignoring (this is about the
only tracking that needs to take place under the hood).

In a simple render loop, the following happens:

* for each render pass:
    * set render target, this starts a new rendering pass
    * for each 'material batch':
        * set draw state object, this implicitely also sets the current shader and mesh(es)
        * set per-material texture- and uniform-blocks
        * for each instance:
            * compute per-instance uniforms
            * set per-instance uniform block
            * call one of the draw-functions
* finally: submit the frame

Each of these steps is a single call into the Gfx API, for instance:

```cpp
    Gfx::ApplyDefaultRenderTarget(clearState);
    for (const auto& mat : materials) {
        Gfx::ApplyDrawState(mat.drawState);
        Gfx::ApplyUniformBlock(mat.uniforms);
        Gfx::ApplyTextureBlock(mat.textures);
        for (const auto& inst : instances) {
            inst.uniforms.mvp = ...;
            Gfx::ApplyUniformBlock(inst.uniforms);
            Gfx::Draw(0);
        }
    }
    Gfx::CommitFrame();
```

There is no Gfx::Clear() call to clear the current render target (there used
to be up until the Metal renderer backend was implemented). Now the clear
actions happen implicitely when a render target is applied. The 'ClearState'
is a simple struct with all the necessary information (if and what to clear,
and the clear RGBA and depth/stencil values).

The Gfx::ApplyDrawState() call sets the precompiled pipeline state along
with the shader and one or more meshes which provide the input geometry.

The only thing left is then calls to Gfx::ApplyUniformBlock() and
Gfx::ApplyTextureBlock() to set and change uniforms and textures between
draw calls.

Finally the call to Gfx::CommitFrame() marks the current frame as finished.

Except for the Gfx::ApplyUniformBlock() call, all other Gfx::ApplyXXX() calls 
take resource ids as parameter which point to pre-baked resource objects.

The Gfx::ApplyUniformBlock() call is slightly different, since it only
takes a const-ref to a simple C struct in memory which directly contains
variable uniform values and constant shader stage binding information 
(what shader stage to bind to (VS or FS), and the bind slot index).

#### Meshes

Mesh objects describe a piece of geometry for rendering:

* a vertex buffer
* a vertex-layout which describes the components of a vertex
* the primitive type (triangles, lines, points, etc... optional)
* an array of 'primitive groups' (optional)
* an index buffer (optional)
* whether the index buffer contains 16- or 32-bit indices
* usage hints for the vertex and index buffers (immutable vs dynamic)

Mesh objects are not directly set in the render pipeline, instead they
are part of draw state objects. The only reason why they exist as separate
resources is to allow reuse and combination.

Vertex components have pre-defined semantics (position, normal, texcoord0, ...)
and data types (float, float2, ..., ubyte4n, ...). Currently it is not possible
to create your own vertex component semantics.

Primitive groups describe of primitives that are rendered together (aka sub-meshes) 
within the mesh data, via a baseElement and numElements value. For indexed rendering, 
baseElement and numElements refer describe a range in the index buffer, 
for non-indexed rendering in the vertex buffer.

The entire mesh must use a single primitive type (e.g. triangle list, 
triangle strip, etc...). Formerly, the primitive type was part of the 
the primitive group so that one mesh could include different primitive types.
The reason why the primitive type this has been out of the primitive groups
is that the D3D12 pipeline state object needs to know the basic type
(whether triangles, lines or points are rendered).

A typical mesh for non-indexed rendering will have a vertex buffer,
a vertex layout, and at least one primitive group. For indexed rendering,
it will also have an index buffer, and the index type set which says
whether the indices in the index buffer are 16 or 32 bits.

The usage hint for the vertex and index data tell Oryol whether the data
is immutable (in this case the mesh must be created with data, it is not
possible to set the data later), or dynamic, which means that new data
will be uploaded frequently (usually once per frame).

It is also possible to create meshes which only provide a vertex buffer
and a vertex layout, but have no primitive groups or a primitive type set.
These meshes serve as additional vertex streams for instanced rendering,
or when it makes sense to separate static from dynamic vertex data. More
on that later in the DrawState section.

#### Shaders

A shader object contains the actual vertex- and fragment-shaders as well
as resource binding information for uniform- and texture-blocks.

It is not possible to re-combine vertex-shaders with different fragment-shaders
during rendering, everything must be defined upfront.

The resource binding model is quite efficient for both 'old-school' resource binding
(where each uniform and each texture is set individually) as well as 
modern resource binding where resources set as pre-defined groups.

Each of the 2 shader stages (vertex- and fragment-shaders) has 4 bind-slots
for uniform blocks and 4 bind-slots for texture blocks.

Each uniform block can have up to 16 uniforms values, and each 
texture block can have up to 16 textures.

These limits are what they are to somewhat fit together with the
WebGL spec, and to keep the D3D12 'root signature' small. It is very likely
that these limits will change, or even become a flexible depending on what
rendering backend is used (although I'd like to avoid that).

Although it is possible to setup shader objects in C++ 'manually', they pretty much
depend on Oryol's integrated shader code generation. The shader-code, not the
C++ code defines the layout of uniform- and texture-blocks, and the shader-
code-generator writes the C++ code which creates a shader-setup object.

For shader uniform blocks, a C++ 'uniform block layout' object
is created by the code generator, this contains the name, order and types of 
the uniforms in the block (this is necessary for GLES2/WebGL which need to 
uniform values one by one => may be this will be optimized later by grouping 
uniforms into arrays by type).

Additionally to the uniform-block-layout, a C-struct will be written which
maps the shader-uniform-block to its C-equivalent using GLM library data types
and the right padding.
