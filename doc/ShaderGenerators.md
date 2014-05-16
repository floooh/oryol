## Shader Generators

Generate C++ code with embedded GLSL shaders from annotated shader source snippets through a python generator script.

**NOTE:** this is work-in-progress, and currently the focus is on simple OpenGL ES GLSL (however later hopefully special features of higher GLSL versions will be supported by the code generators).

### Problems:

Creating and maintaining shader libraries can be tricky, especially in GLSL:

* incompatibilities between different GLSL versions
* no include or 'external dependencies' mechanism
* vertex shaders and fragment shaders must be compiled separately
* cannot add custom meta-data / annotations which can be queried by the C++ side or content-creation tools
* need several variations of the same shader, sometimes uebershader approach is better, sometimes separate shaders  
are better
* different rendering strategies (e.g. forward rendering vs deferred rendering) require different shader types, but
a lot of the low-level code is the same

### Goals:

* basic idea is a very simple 'template engine' which generates C++ sources with embedded GLSL code from
'annotated GLSL code snippets'
* integrated into the edit-compile-test cycle, can directly iterate in the IDE, error messages must be 
parsable by the IDE (see UnitTest++ for an example)
* a shader library is a group of related shaders (usually dozens, but not hundreds)
* before code generation, a shader library is a bunch of shader source files (there's no difference between spreading 
the code across different files and directories, or merging all into one file)
* after code generation a shader library is one C++ header, defining one class for the shader library,
embedded classes (one per shader), embedded strings with generated GLSL shader source, and C++ enums/constants 
with binding / reflection information, these classes are only data providers and have no functionality
* an Oryol application can include any number of shader libraries 

### Details:

#### Build Process Integration

Oryol already comes with a general source code generation mechanism called 'generator 
scripts'. This usually converts one XML file into one C++ header and one C++ source file through a python 
script which is selected in the header of the XML file.

This process is already integrated into the build process as a CMake pre-build-step.

For shader generation there is only a very minimal XML file per shader library (since I didn't want to embed GLSL into XML) which contains the name of the library and where to find the shader snippet files for the library, e.g.:

```xml
<Generator type="ShaderLibrary" name="PreLightPassLib">
    <AddDir path="shd/prelightpass"/>
    <AddDir path="shd/common"/>
</Generator>
```

This would gather all GLSL snippet files from the directories "shd/prelightpass" and "shd/common" and 
create a C++ header/source pair "PreLightPassLib.h/.cc" as part of the build process.

#### Shader Snippet Parsing

The generator script only does very simple line-parsing, it doesn't understand actual GLSL syntax:

1. first all comments are removed
2. any superfluous white space is removed
3. template tags are parsed and removed or replaced

The last step is the important one, the parser will look for special tags in the source looking like this:

```
${tag: arg0 arg1 ...}
or
${tag}
```

Everything within **${ }** contains special information for the parser and will either be removed from the final GLSL code, or replaced with an actual GLSL statement.

##### Function Tags

Function tags are the basis for the function-level dependency mechanism. To make a GLSL function visible to the parser, the 
**${func:[name]}** tag is used. When a function is defined through a function tag, it can be automatically included 
somewhere else as a dependency. Note how template tags replace the entire function header with input arguments and 
return values (yes, this is quite verbose, the syntax is not final):

```
${func: MyFunction}
${in: float arg0}
${in: vec2 arg1}
${return: vec4}
{
    vec4 result;
    // normal GLSL code...
    return result;
}
```

When the parser encounters this code it will add the function MyFunction, the following tags, and the actual
function body to a lookup table where it can later be used by the code generator.

The GLSL code for this function would look like this:

```
vec4 MyFunction(float arg0, vec2 arg1)
{
vec4 result;
... (comments and whitespace is removed)
return result;
}
```

Multiple output and in/out arguments are possible:

```
${func: MyFunction}
${in: float bla}
${in: vec2 blub}
${inout: vec3 blob}
${out: vec4 foo}
${out: vec4 baz}
{
    // GLSL code which assigns values to blob, foo and baz
}
```

Functions can also directly access uniform variables, these will be added to a shader's uniform variables at code generation time. Uniforms have a type, a GLSL variable name, and finally a binding name for the C++ side (the generated
shader class will contain a constant of that name). Here's an example of a function which directly uses
the ModelViewProj uniform:

```
${func: MyTransform}
${uniform: mat4 mvp ModelViewProj}
${in: vec4 pos}
${return: vec4}
{
    return mvp * pos;
}
```

To call a function defined with a ${func:} tag, just embed the function name in **${ }** somewhere else:

```
    ...
    vec4 transformedPos = ${MyTransform}(position);
    ...
```

This will also automatically pull in the MyTransform function as a dependency into the generated GLSL code.


##### Vertex Shader Tags

Vertex shaders are special function tags which describe a vertex shader main function. The transformed
position must be assigned to the special tag ${position} (this will be replaced with gl_Position by the
code generator):

```
${vs:MyVertexShader}
${uniform: mat4 mvp ModelViewProj}
${in: vec4 position}
${in: vec2 texcoord0}
${out: vec2 uv}
{
    ${position} = ${MyTransform}(pos);
    uv = texcoord0
}
```

The ${in:} and ${out:} tag have a special meaning in vertex shaders:

* **${in:}** describes a vertex attribute, these have special names in Oryol (e.g. position, normal, tangent, texcoord0..7 etc).
* **${out:}** describes a varying which is handed over to the fragment shader.
* **${uniform:}** tags behave the same as described above.

##### Fragment Shader Tags

Same idea as vertex shader tags, but for fragment shaders. The resulting color must be assigned to the
special tag ${color} (this will be replaced with gl_FragColor, or a special output variable, depending on the GLSL
version:

```
${fs:MyFragmentShader}
${uniform: sampler2D tex ColorTexture}
${in: vec2 uv}
{
    ${color} = ${texture2D}(ColorTexture, uv);
}
```

Also note the special ${texture2D} tag. Since different GLSL version have different incompatible texture
sampling functions it is necessary to handle them as special tags which will be replaced by the code
generator with the right function.

##### Bundles and Program Tags:

A program tag associates a vertex- with a fragment-shader, these will be linked into a GL shader program at
runtime.

A bundle groups several related shader-programs and under a single handle and adds a common set of uniform variables.
These are basically different variations of the same shader, e.g. a skinned vs a static, lit vs unlit, surface
material vs shadow map shader...

```
${bundle:MyShaderBundle}
${program:MySkinnedProgram MySkinnedVertexShader MyFragmentShader}
${program:MyRigidProgram MyRigidVertexShader MyFragmentShader}
...
```

### What's next

From here on it gets rather murky, what's missing are high-level 'rendering technique template' which
would differentiate a forward renderer shader library from a deferred renderer shader library.

After that it may be useful to think about adding support for a 'data-flow' shader editor, which would
require new tags for input- and output slots, and some limited support for code generation at runtime.

