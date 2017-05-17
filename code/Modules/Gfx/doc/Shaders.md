## Shaders

Shader objects wrap a vertex- and fragment-shader, and shader-meta-information,
such as the expected input vertex-layout, the number and layout of
uniform blocks, and the number of types of textures.

Oryol has a SPIRV-based shader code-generation system, shaders are written in GLSL v330 
syntax, and translated into different GLSL versions, HLSL, and MetalSL. 

Even though the 2 concepts (shader code generation and Shader resources) are
closely integrated, it is also possible to use the shader system without the
shader code generator, this is less convenient though and requires
per-platform code paths.

### A simple step-by-step example

The following code is taken from the [Triangle sample](../../../Samples/Shapes/Shapes.cc).

First, create a separate shader source file called **shaders.glsl** (the
 name isn't relevant though). This file contains the vertex- and
 fragment-shader. Note the custom '@-tags':

```glsl
// the @vs tag starts a vertex-shader block, all source code
// between an @vs and @end tag will be compiled as vertex shader
@vs vs

uniform params {
    mat4 mvp;
};

in vec4 position;
in vec4 color0;
out vec4 color;

void main() {
    gl_Position = mvp * position;
    color = color0;
}
@end

// similar to @vs for vertex-shaders, the @fs tag starts a fragment shader block
@fs fs 
in vec4 color;
out vec4 fragColor;
void main() {
    fragColor = color;
}
@end

// the @program tag creates a linked shader program from one vertex-
// and one fragment-shader
@program Shader vs fs
```
The new shader file must be added to the app's CMakeLists.txt
file:

```cmake
fips_begin_app(Shapes windowed)
    ...
    oryol_shader(shaders.glsl)
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
The namespace 'Shader' is generated from the program name
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
uniform params {
    mat4 mvp;
};
```

The associated C-struct will look like this:

```cpp
namespace Shader {
    #pragma pack(push,1)
    struct params {
        // ...some static members omitted
        glm::mat4 mvp;
    };
    #pragma pack(pop)
}
```
This structure can be used to pass shader parameters
for the next draw call:

```cpp
Shader::params vsParams;
vsParams.mvp = ...;
Gfx::ApplyUniformBlock(vsParams);
```
The call to _Gfx::ApplyUniformBlock()_ must happen between a call to _Gfx::ApplyDrawState()_ and _Gfx::Draw()_.

The generated uniform-block C structures have a few static members with
additional information about the uniform block. The complete C structure
from above actually looks like this:

```cpp
namespace Shader {
    #pragma pack(push,1)
    struct Params {
            static const int _bindSlotIndex = 0;
            static const ShaderStage::Code _bindShaderStage = ShaderStage::VS;
            static const uint32_t _layoutHash = 2635524038;
        glm::mat4 mvp;
    };
    #pragma pack(pop)
}
```

The **\_bindShaderStage** is the shader stage this uniform block must be
bound to (either the vertex-shader-stage, or the fragment-shader-stage). The
**\_bindSlotIndex** is the bind slot on that shader stage. Each stage has a
small number of bind slots, currently this is 4 slots per stage.

Finally the **\_layoutHash** is a hash value over all uniform block member
types, this hash is used for a type-check in _Gfx::ApplyUniformBlock()_ to
make sure that the uniform block is compatible with the currently set
shader.

### Using Textures in Shaders

Up to 4 textures can be bound to the vertex-shader-stage, 
and up to 12 textures to the fragment-shader-stage. Textures
are bound during _Gfx::ApplyDrawState()_, just like vertex- and
index-data.

Standard GLSL syntax applys for declaring and using texture.

Here is the complete shader source code from the [DDSTextureLoading sample](../../../Samples/DDSTextureLoading/DDSTextureLoading.cc):

```glsl
// the vertex shader, nothing unusual here...
@vs vs
uniform vsParams {
    mat4 mvp;
};
in vec4 position;
in vec2 texcoord0;
out vec2 uv;

void main() {
    gl_Position = mvp * position;
    uv = texcoord0;
}
@end

// the fragment shader declares a single 2D texture and samples it
@fs fs
uniform sampler2D tex;
in vec2 uv;
out vec4 fragColor;
void main() {
    fragColor = vec4(texture(tex, uv).xyz, 1.0);
}
@end

@program Shader vs fs
```
On the C++ a bind-slot constant is created for each texture:

```cpp
namespace Shader {
    static const int tex = 0;
};
```

...use this constant as bind-slot index when initializing a DrawState
(make sure to use the right shader stage, FSTextures for fragment-shader 
textures, and VSTextures for vertex-shader textures):

```cpp
DrawState ds;
ds.Pipeline = myPipeline;
ds.Meshes[0] = myMesh;
ds.FSTexture[Shader::tex] = myTexture;
Gfx::ApplyDrawState(ds);
```

### Shader Programming Reference

#### @-Tags

@-tags are used as meta-keywords which are parsed by the
shader code generator and removed before the code is handed
to the platform-specific shader compilers.

#### @vs \[name\]

The ```@vs``` tag starts a vertex-shader code block. All code inside a
```@vs```/```@end``` pair will be compiled as GLSL v330 vertex shader.

#### @fs \[name\]

Same as ```@vs```, but for fragment shaders.

#### @program \[name\] \[vs\_name\] \[fs\_name\]

The program tag is used to link a vertex-shader with a fragment-shader into
 a shader-program. Note that outputs of the vertex shader must match exactly
 the inputs of the fragment shader (names, types and order).

For example:

```glsl
@vs sphereVS
in vec4 position;
in vec3 normal;
out vec3 worldPosition;
out vec3 worldNormal;
out vec3 worldEyePos;
out vec3 worldLightDir;
    ...
@end

@fs sphereFS
in vec3 worldPosition;
in vec3 worldNormal;
in vec3 worldEyePos;
in vec3 worldLightDir;
    ...
@end

@program SphereShader sphereVS sphereFS
```

Note how the outputs of the vertex shader match the inputs of the fragment shader.

#### @block \[name\]

A code block is used to group shader subroutines under a name that can be
'included' anywhere into a vertex- or fragment-shader with the ```@include
[name]```.

```glsl
@block lighting
vec3 light(vec3 baseColor, vec3 eyeVec, vec3 normal, vec3 lightVec) {
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

in vec3 worldPosition;
in vec3 worldNormal;
in vec3 worldEyePos;
in vec3 worldLightDir;
in vec4 color;
out vec4 fragColor;

// bring in lighting helper functions
@include  lighting

void main() {
    vec3 eyeVec = normalize(worldEyePos - worldPosition);
    vec3 nrm = normalize(worldNormal);
    vec3 lightDir = normalize(worldLightDir);
    fragColor = vec4(light(color.xyz, eyeVec, nrm, lightDir), 1.0);
}
@end
```
#### @end

The ```@end``` tag is to finish a ```@vs```, ```@fs```, ```@block```.

#### @include \[cb_name\]

The ```@include``` tag is used to import a ```@block``` into a vertex-
or fragment-shader.


#### Vertex Shader Input Names

Only the following names can be used for vertex shader inputs:

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
float,
mat4[], mat2[], vec4[]
```

These are the valid texture types used in texture blocks:
```
sampler2D, samplerCube, sampler3D, sampler2DArray
```

### Further Info

See this [blog post](http://floooh.github.io/2017/05/15/oryol-spirv.html) 
for more background information about the shader system.