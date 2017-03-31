## Shaders

Shader objects wrap a vertex- and fragment-shader, and shader-meta-information,
such as the expected input vertex-layout, the number and layout of
uniform blocks, and the number of types of textures.

Oryol has a shader code-generation system which cross-translates a generic
shader language into different GLSL versions, HLSL and MetalSL. Even though
the 2 concepts (shader code generation and Shader resources) are closely
integrated, it is also possible to use the shader system without the
shader code generator, this is less convenient though and requires per-platform
code paths.

### A simple step-by-step example

The following code is taken from the [Triangle sample](../../../Samples/Shapes/Shapes.cc).

First, create a separate shader source file called **shaders.shd** (the name isn't relevant though). This file contains the vertex- and 
fragment-shader. Note the
custom '@-tags':

```glsl
// a uniform-block with a single model-view-projection matrix
@uniform_block params Params
mat4 mvp ModelViewProjection
@end

// the vertex shader, this reads from the uniform block
// defined above, takes a vec4 position and vec4 color
// as vertex-inputs, and passes a vec4 color to the
// fragment shader
@vs vs
@use_uniform_block params
@in vec4 position
@in vec4 color0
@out vec4 color
    // since matrix multiplication is handled differently
    // between GLSL, HLSL and MetalSL it must be wrapped
    // in a built-in macro:
    _position = mul(mvp, position);
    color = color0;
@end

// the fragment shader, this takes a vec4 color from the
// vertex shader (the number, order and types of fragment
shader inputs must match the vertex shader outputs)
@fs fs
@in vec4 color
    _color = color;
@end

// finally define a shader program consisting of
// the vertex shader and fragment shader defined above,
// multiple shader programs can be defind in a single shader source
// file, and they can share the same vertex- and fragment-shaders
// (as long as the vertex-shader outputs match the fragment-shader
// inputs)
@program Shader vs fs
```
The new shader file must be added to the app's CMakeLists.txt
file:

```cmake
fips_begin_app(Shapes windowed)
    ...
    oryol_shader(shaders.shd)
    ...
fips_end_app()
```

After a _fips gen_ the new shader will be evaluated when the project
is built, and a C++ header/source pair will be generated
(in this case it would the pair would be shader.h / shader.cc).

The generated source code contains the code to create
a ready-to-use shader resource object:

```cpp
#include "shader.h"
...
Id shd = Gfx::CreateResource(Shader::Setup());
...
```
The class-name 'Shader' is generated from the program name
in the line:

```glsl
@program Shader vs fs
```

A shader object cannot be used directly for rendering, instead
it must be passed as creation parameter when creating pipeline
state objects. More on that in the [Pipelines documentation](Pipelines.md).

Apart from the code to create a ShaderSetup object, the shader
code-generation will also create a C structure for each shader 
uniform block.

Remember that the example shader has a uniform block with a single
4x4 matrix:

```glsl
@uniform_block params Params
mat4 mvp ModelViewProjection
@end
```

The associated C-struct will look like this:

```cpp
struct Shader {
    #pragma pack(push,1)
    struct Params {
        // ...some static members omitted
        glm::mat4 ModelViewProjection;
    };
    #pragma pack(pop)
};
```
This structure can be used to pass shader parameters
for the next draw call:

```cpp
Shader::Params vsParams;
vsParams.ModelViewProjection = ...;
Gfx::ApplyUniformBlock(vsParams);
```
The call to _Gfx::ApplyUniformBlock()_ must happen between a call to _Gfx::ApplyDrawState()_ and _Gfx::Draw()_.

The generated uniform-block C structures have a few
static members with additional information about
the uniform block. The complete C structure from
above actually looks like this:

```cpp
struct Shader {
    #pragma pack(push,1)
    struct Params {
            static const int _bindSlotIndex = 0;
            static const ShaderStage::Code _bindShaderStage = ShaderStage::VS;
            static const uint32_t _layoutHash = 2635524038;
        glm::mat4 ModelViewProjection;
    };
    #pragma pack(pop)
};
```

The **\_bindShaderStage** is the shader stage this uniform block must be bound to (either the vertex-shader-stage, or the fragment-shader-stage). The **\_bindSlotIndex** is the bind slot on that shader
stage. Each stage has a small number of bind slots, 
currently this is 4 slots per stage.

Finally the **\_layoutHash** is a hash value over
all uniform block member types, this hash is used
for a type-check in _Gfx::ApplyUniformBlock()_ to 
make sure that the uniform block is compatible with
the currently set shader.

### Using Textures in Shaders

Up to 4 textures can be bound to the vertex-shader-stage, 
and up to 12 textures to the fragment-shader-stage. Textures
are bound during _Gfx::ApplyDrawState()_, just like vertex- and
index-data.

A shader must declare the used textures and their types inside 
**@texture\_block**s, similar to uniform blocks, and a vertex-
or fragment-shader must have a **@use\_texture\_block** statement
if it wants to sample textures.

Here is the complete shader source code from the [DDSTextureLoading sample](../../../Samples/DDSTextureLoading/DDSTextureLoading.cc):

```glsl
// uniform block with mvp matrix
@uniform_block vsParams VSParams
mat4 mvp ModelViewProjection
@end

// the texture block with a single 2D texture
@texture_block textures Textures
sampler2D tex Texture
@end

// the vertex shader (not the texcoord0 input which is
// written to a 'vec2 uv' output)
@vs vs
@use_uniform_block vsParams
@in vec4 position
@in vec2 texcoord0
@out vec2 uv
{
    _position = mul(mvp, position);
    uv = texcoord0;
}
@end

// the fragment shader with the @use_texture_block
// statement, and the 'vec2 uv' input:
@fs fs
@use_texture_block textures
@in vec2 uv
{
    _color = vec4(tex2D(tex, uv).xyz, 1.0);
}
@end

// and finally the shader program:
@program Shader vs fs
```

FIXME: texture usage in C++ code



