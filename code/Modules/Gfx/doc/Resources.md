## Resource Objects

Gfx resource objects encapsulate render state and data required
for rendering. Oryol follows the philosophy of modern 
3D-APIs to bundle render state and data into a few, precompiled
resource objects which are combined into final render pipeline
configurations by plugging the resource objects into 
a number of predefined resource slots.

The resource binding model is very similar to D3D11 and Metal,
with a focus on ease-of-use over 'explicitness'.

### Resource Concepts

For a general overview of resource objects in Oryol, 
please read the [Resource module documentation](../../Resource/README.md).

### Gfx Module Resource Types

The Oryol Gfx module has the following resource types (it is recommended to first continue reading the general resource documentation below before diving into the different resource types):

* **[Meshes](Meshes.md)**: immutable or dynamic vertex and index data, 'primitive groups'
* **[Textures](Textures.md)**: immutable or dynamic 2D-, 3D-, cubemap- and array-textures
* **[Render Passes](RenderPasses.md)**: wrap rendering operations to a render target, clearing, offscreen rendering, multiple render targets, MSAA resolve
* **[Shaders](Shaders.md)**: vertex-shaders, fragment-shaders and uniform-blocks
* **[Pipelines](Pipelines.md)**: pipeline-state-objects containing immutable, precompiled render state and vertex-layout

There are also **UniformBlocks**, these are similar to resources in that
they are 'bound' before draw calls take place. But UniformBlocks 
are not resources in the typical sense since they are just simple
C structures, they are not explicitely created or destroyed through the Gfx module.

### Resource Creation

All resource types can be created instantly from data in memory,
meshes and textures can also be loaded asynchronously from the local
hard disc or web servers.

To create a resource, a **Setup** object must be initialized with
the required resource creation parameters which is handed to one
of the Gfx resource creation methods:

* **PassSetup** => Gfx::CreateResource() => render pass
* **MeshSetup** => Gfx::CreateResource() or Gfx::LoadResource() => mesh
* **TextureSetup** => Gfx::CreateResource() or Gfx::LoadResource() => texture
* **ShaderSetup** => Gfx::CreateResource() => shader
* **PipelineSetup** => Gfx::CreateResource() => pipeline

For more details on the required creation parameters and code
samples, see the respective resource type documentation files
for [Render Passes](RenderPasses.md), [Meshes](Meshes.md), [Textures](Textures.md), [Shaders](Shaders.md) and [Pipelines](Pipelines.md).

The difference between Gfx::CreateResource() and Gfx::LoadResource()
is that CreateResource() instantly creates a resource from data
in memory, while Gfx::LoadResource() asynchronously loads a resource
from a filesystem. Both methods immediately return a usable
resource Id, but in the case of LoadResource() all rendering 
operations using a resource that's still loading will be silently
dropped (this simply means that a 3D object will not be rendered
until all its resources have finished loading).

### Resource Binding

Resource binding in the Gfx module is conceptually similar to
D3D11 or Metal. There's a number of pre-defined resource slots,
which are plugged by the application with resource objects
to reconfigure the render pipeline for the next draw call.

The whole process is best explained with a piece of sample code:

```cpp
Gfx::BeginPass(passId);
Gfx::ApplyDrawState(drawState);
Gfx::ApplyUniformBlock(uniformBlock);
Gfx::Draw();
Gfx::EndPass();
```

The function **Gfx::BeginPass()** optionally takes the resource id of
a render pass object, in this case, offscreen rendering will be 
performed (if no pass id is given to Gfx::BeginPass() rendering
will go into the default framebuffer).

**Gfx::ApplyDrawState()** is the most important resource binding function,
since this binds a pipeline state object, meshes and textures. A
**DrawState** is a simple C structure with bind-slots for one pipeline
object, and a fixed number of resource slots for meshes and textures.

Since a draw state object is just a simple C structure (similar to a
UniformBlock) it can be reconfigured any time by pluggin different resources
into the resource slots between calls to Gfx::ApplyDrawState().

The DrawState structure looks like this:

```cpp
struct DrawState {
    /// the pipeline state object
    Id Pipeline;
    /// input meshes
    StaticArray<Id, GfxConfig::MaxNumInputMeshes> Mesh;
    /// vertex shader stage textures
    StaticArray<Id, GfxConfig::MaxNumVertexTextures> VSTexture;
    /// fragment shader stage textures
    StaticArray<Id, GfxConfig::MaxNumFragmentTextures> FSTexture;
};
```
Right now, up to 4 meshes, 4 vertex stage textures, and 12 
fragment stage textures can be bound.

Please note that the Shader used for rendering is part of the
compiled pipeline object state, so there is no separate 
binding slot for shaders in the DrawState structure.

```cpp
// bind resources to render a cube with a textured material
// (all of those had been created previously)
DrawState ds;
ds.Pipeline = material0;
ds.Mesh[0]  = cubeMesh;
ds.FSTexture[0] = brickTexture;
Gfx::ApplyDrawState(ds);

// render 5 cubes at different positions (defined by a model-view-projection
// matrix in the vsParams uniform block structure)
Shader::VSParams vsParams;
for (int i = 0; i < 5; i++) {
    // write a new model-view-proj matrix to the uniform block
    vsParams.ModelViewProj = ...;
    Gfx::ApplyUniformBlock(vsParams);
    // this draws the first (and only) primitive group of the mesh
    Gfx::Draw(0);
}
```

There's a bit of implicit magic happening in Gfx::ApplyUniformBlock(), which
we'll glance over for now, more information about uniform blocks can 
be found in the [Shaders documentation](Shaders.md).

### Writing your own Resource Loaders

FIXME

