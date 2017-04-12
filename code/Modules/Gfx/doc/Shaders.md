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
// shader inputs must match the vertex shader outputs)
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
(in this case the generated pair would be shaders.h / shaders.cc).

The generated source code contains the code to create
a ready-to-use shader resource object:

```cpp
#include "shaders.h"
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

On the C++ side, texture blocks are not exposed as C structs
like uniform blocks, instead binding slot indices will be
written to the generated C++ header:

```cpp
struct Textures {
    static const int Texture = 0;
};
```

The struct name 'Textures' and constant name 'Texture' is
taken from the texture block definition in shaders.shd:

```glsl
@texture_block textures Textures
sampler2D tex Texture
@end
```

To bind a texture to this slot in the fragment-shader stage in
C++ (along with a pipeline state object and mesh):

```cpp
DrawState ds;
ds.Pipeline = myPipeline;
ds.Meshes[0] = myMesh;
ds.FSTexture[Textures::Texture] = myTexture;
Gfx::ApplyDrawState(ds);
```

### Shader Programming Reference

#### @-Tags

@-tags are used as meta-keywords which are parsed by the
shader code generator and removed before the code is handed
to the platform-specific shader compilers.

##### @vs \[name\]

Defines a vertex shader main-function, must be finished with an ```@end``` tag. Inside the (```@vs```,```@end```) tags the
following tags a valid:

* ```@in```: the vertex-shader input attributes
* ```@out```: the vertex-shader output attributes
* ```@use_uniform_block```: one for each uniform-block used by the vertex shader
* ```@use_texture_block```: one for each texture-block used by the vertex shader
* ```@use_code_block```: one for each code-block used by the vertex shader

In addition to its custom output parameters, a vertex shader function
must write the special 'homogeneous vertex position' variable 
```_position```.

An example, non-trivial vertex shader could look like this:

```glsl
@vs shapeVS
@use_uniform_block shapeVSParams
@in vec4 position
@in vec3 normal
@out vec3 worldPosition
@out vec3 worldNormal
@out vec3 worldEyePos
@out vec3 worldLightDir
@out vec4 color
{
    _position = mul(mvp, position);
    worldPosition = mul(model, position).xyz;
    worldNormal = mul(model, vec4(normal, 0.0)).xyz;
    worldEyePos = eyePos;
    worldLightDir = lightDir;
    color = shapeColor;
}
@end
```

##### @fs \[name\]

Defines a fragment-shader main-function, must be finished with an
```@end``` tag. Inside (```@fs```,```@end```) the following tags
are valid:

* ```@in```: the fragment-shader input attributes
* ```@use_uniform_block```: one for each uniform-block used by the fragment shader
* ```@use_texture_block```: one for each texture-block used by the fragment shader
* ```@use_code_block```: one for each code-block used by the fragment shader

A fragment shader is expected to write the output color to the special
variable ```_color```, or for multiple-render-target-rendering, to
```_color```, ```_color1```, ```_color2``` and ```_color3``` (depending
on the number of render targets).

A non-trivial fragment shader could look like this:

```glsl
@fs sphereFS
@use_texture_block textures
@use_code_block lighting
@use_code_block util
@in vec3 worldPosition
@in vec3 worldNormal
@in vec3 worldEyePos
@in vec3 worldLightDir
{
    vec3 eyeVec = normalize(worldEyePos.xyz - worldPosition.xyz);
    vec3 nrm = normalize(worldNormal.xyz);

    // reflection, refraction and fresnel
    vec3 refl = reflect(-eyeVec, nrm);
    vec3 refr = refract(-eyeVec, nrm, 0.7);
    vec4 reflectColor = texCUBE(tex, refl);
    vec4 refractColor;
    if (length(refr) > 0.0) {
        refractColor = texCUBE(tex, refr);
    }
    else {
        refractColor = reflectColor;
    }
    float fr = fresnel(eyeVec, nrm, 0.4);
    vec3 c = vec3(1.5, 1.2, 1.2) * fr * mix(refractColor.xyz, reflectColor.xyz, fr);
    c = light(c, eyeVec, nrm, normalize(worldLightDir));
    _color = vec4(gamma(c), 1.0);
}
@end
```

##### @program \[name\] \[vs\_name\] \[fs\_name\]

The program tag is used to associate a vertex-shader with a
fragment-shader. Note that outputs of the vertex shader must
match exactly the inputs of the fragment shader (names, types and
order).

For example:

```glsl
@vs sphereVS
@in vec4 position
@in vec3 normal
@out vec3 worldPosition
@out vec3 worldNormal
@out vec3 worldEyePos
@out vec3 worldLightDir
    ...
@end

@fs sphereFS
@in vec3 worldPosition
@in vec3 worldNormal
@in vec3 worldEyePos
@in vec3 worldLightDir
    ...
@end

@program SphereShader sphereVS sphereFS
```

Note how the ```@out```puts of the vertex shader match the ```@in```puts of the fragment shader.

##### @in \[type\]\[name\]

The ```@in``` tag must appear inside a ```@vs``` or ```@fs``` block
and defines an input argument to the vertex- or fragment-shader main
function.

Only the following ```types``` are allowed as input- or output
arguments: ```float, vec2, vec3 or vec4```.

For the vertex shader, ```name``` must be a valid vertex attribute name
(this is used for matching mesh-data vertex components to vertex-shader-
inputs):

```position, normal, texcoord0..3, tangent, binormal, weights, indices, color0..1, instance0..3```

Note how these match the members of the C++ VertexAttr enum class:

```cpp
class VertexAttr {
public:
    /// vertex attribute enum
    enum Code : uint8_t {
        Position = 0,   ///< "position"
        Normal,         ///< "normal"
        TexCoord0,      ///< "texcoord0"
        TexCoord1,      ///< "texcoord1"
        TexCoord2,      ///< "texcoord2"
        TexCoord3,      ///< "texcoord3"
        Tangent,        ///< "tangent
        Binormal,       ///< "binormal"
        Weights,        ///< "weights" (skin weights)
        Indices,        ///< "indices" (skin indices)
        Color0,         ///< "color0"
        Color1,         ///< "color1"
        Instance0,      ///< "instance0"
        Instance1,      ///< "instance1"
        Instance2,      ///< "instance2"
        Instance3,      ///< "instance3"

        NumVertexAttrs,
        InvalidVertexAttr,
    };

    /// convert to string
    static const char* ToString(Code c);
};
```

For fragment-shader inputs, the ```name``` can be choosen freely,
but must match the ```@out``` names of the vertex-shader.

##### @out \[type\]\[name\]

The ```@out``` tag defines the vertex-shader outputs which are handed
to the fragment-shader as inputs. It must appear inside a ```@vs```,```@end``` block. Only the following ```type```s are valid as vertex shader outputs:

```float, vec2, vec3, vec4```

The ```name``` can be choosen freely, but must match the name of the
corresponding fragment shader input.

##### @uniform\_block \[name\]\[export\_name\]

A uniform block groups shader uniform parameters for the vertex or fragment
shader. A shader can use up to 4 uniform blocks. During code-generation, a C
struct will be generated for each shader uniform block to allow easy
updating of uniform parameters from the C++ side (but note that a call to
Gfx::ApplyUniformBlock() is required after updating the C structs)

The ```name``` parameter of the uniform blocks is used to reference the
uniform block inside the shader source via ```@use_uniform_block```,
while the second ```export_name``` parameter is used as name of the generated
C struct on the C++ side.

Here's an example what a uniform block definition:

```glsl
@uniform_block shapeVSParams VSParams
mat4 mvp ModelViewProjection
mat4 model Model
vec4 shapeColor Color
vec3 lightDir LightDir
vec3 eyePos EyePos
@end
```

Each uniform block member has a ```type``` (e.g. float, vec2, ...), an
internal name for use in shader functions (e.g. mvp, shapeColor, ...), and
an export name which is used in the generated C struct (e.g. ModelViewProject,
Color, ...).

Here's a code example how to use the above uniform block in a
vertex shader function:

```glsl
@vs shapeVS
@use_uniform_block shapeVSParams
@in vec4 position
@in vec3 normal
@out vec3 worldPosition
@out vec3 worldNormal
@out vec3 worldEyePos
@out vec3 worldLightDir
@out vec4 color
    _position = mul(mvp, position);
    worldPosition = mul(model, position).xyz;
    worldNormal = mul(model, vec4(normal, 0.0)).xyz;
    worldEyePos = eyePos;
    worldLightDir = lightDir;
    color = shapeColor;
@end
```

This is the C structure that is created for the above uniform block:

```cpp
struct VSParams {
    glm::mat4 ModelViewProjection;
    glm::mat4 Model;
    glm::vec4 Color;
    glm::vec3 LightDir;
    float _pad_LightDir;
    glm::vec3 EyePos;
    float _pad_EyePos;
};
```
The automatically generated \_pad\_* fields are required to enforce the same
memory layout in the C structure as in the GPU-side uniform block.

##### @texture\_block \[name\]\[export\_name\]

A texture block groups textures used by a vertex- or
fragment-program, it must be finished with a ```@end```
tag. The ```name``` parameter is used to reference the
texture block inside the shader source file via
```@use_texture_block```, and the ```export_name```
is used to identify the texture on the C++ side.

```glsl
@texture_block
sampler2D tex Texture
@end
```
Inside a texture block, textures are defined with the following
keywords:

* **sampler2D**: a 2D texture
* **samplerCube**: a cubemap texture
* **sampler2DArray**: a 2D-array texture
* **sampler3D**: a 3D texture

##### @code\_block \[name\]

A code block is used to group shader subroutines under a name
that can be 'included' in a vertex- or fragment-shader with the
```@use_code_block [name]``` tag. Functions inside a code block
must be annotated with the ```_func``` keyword:

```glsl
@code_block lighting
func vec3 light(vec3 baseColor, vec3 eyeVec, vec3 normal, vec3 lightVec) {
    // ambient intensity
    float ambient = 0.25;

    // diffuse
    float n_dot_l = max(dot(normal, lightVec), 0.0);
    float diff = n_dot_l + ambient;

    // specular
    float specPower = 16.0;
    vec3 r = reflect(-lightVec, normal);
    float r_dot_v = max(dot(r, eyeVec), 0.0);
    float spec = pow(r_dot_v, specPower) * n_dot_l;

    return baseColor * (diff+ambient) + vec3(spec,spec,spec);
}
@end

@fs shapeFS
@use_code_block lighting
@in vec3 worldPosition
@in vec3 worldNormal
@in vec3 worldEyePos
@in vec3 worldLightDir
@in vec4 color
    vec3 eyeVec = normalize(worldEyePos - worldPosition);
    vec3 nrm = normalize(worldNormal);
    vec3 lightDir = normalize(worldLightDir);
    _color = vec4(light(color.xyz, eyeVec, nrm, lightDir), 1.0);
@end
```

##### @end

 The ```@end``` tag is to finish a ```@vs```, ```@fs```,
 ```@uniform_block```, ```@texture_block``` or ```@code_block```.

##### @use\_uniform\_block \[ub_name\]

The ```@use_uniform_block``` tag is used to import a ```@uniform_block```
into a vertex- or fragment-shader.

##### @use\_texture\_block \[tb_name\]

The ```@use_texture_block``` tag is used to import a ```@texture_block```
into a vertex- or fragment-shader.

##### @use\_code\_block \[cb_name\]

The ```@use_code_block``` tag is used to import functions inside a
```@code_block``` into a vertex- or fragment-shader.

##### @highp [type]

The ```@highp``` tag is used on mobile platforms to 
use 'high precision' for a specific type. Valid types
are ```float``` and all ```sampler*``` types.

#### Vertex Shader Input Names

* **position**:
* **normal**:
* **texcoord0..3**:
* **tangent**:
* **binormal**:
* **weights**:
* **indices**:
* **color0..1**:
* **instance0..3**:

#### Data Types

The following types can be used as inputs for the vertex-
or fragment-shader function:
```
float, vec2, vec3, vec4
```

The following types can be used in uniform blocks:
```
mat4, mat3, mat2, 
vec4, vec3, vec2, 
float, int, bool,
mat4[], vec4[]
```

These are the valid texture types used in texture blocks:
```
sampler2D, samplerCube, sampler3D, sampler2DArray
```

#### Compatibility Wrappers

These are macros which for compatibility between the different GLSL
versions, HLSL and MetalSL:

* **\_vertexid**: use this instead of gl_VertexID in vertex shaders (not available in GLES2/WebGL)
* **\_instanceid**: use this instead of gl_InstanceID in vertex shaders (not available in GLES2/WebGL)
* **\_position**: replaces gl_Position (vertex shader output)
* **\_pointsize**: replaces gl_PointSize in vertex shader
* **\_color, \_color1, \_color2, \_color3**: fragment shader color outputs (replace gl_FragColor etc)
* **\_fragcoord**: replaces gl_FragCoord in fragment shaders
* **\_const**: use this for global constants (resolves to _const_ in GLSL, _static const_ in HLSL and _constant_ in MetalSL
* **\_func**: use this in front of functions which are not the main vertex- or fragment-shader functions (resolves to nothing in GLSL and HLSL, and to _static_ in MetalSL)

Use the following function macros which ensure compatibility between
the different shader languages:

* **mul(m,v)**: use the mul() macros for a matrix*vector multiplication  (resolves to _(m\*v)_ in GLSL and MetalSL, and to _mul(m,v)_ in HLSL)
* **tex2D(s,t), tex3D(s,t), tex2DArray(s,t), texCUBE(s,t)**: texture sampling wrappers in _fragment shader_
* **tex2Dvs(s,t), tex3Dvs(s,t), tex2DArrayvs(s,t)**: texture sampling wrappers in _vertex shader_
* **mix(a,b,c)**: resolved to lerp(a,b,c) in HLSL
* **mod(x,y)**: resolved to (x-y*floor(x/y)) in HLSL
* **fract(x)**: resolved to frac(x) in HLSL
* **discard**: resolved to discard\_fragment() in MetalSL