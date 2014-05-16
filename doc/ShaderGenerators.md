## Shader Generators

Generate C++ code with embedded GLSL shaders from annotated shader source snippets.

### Problems:

Creating and managing shader libraries can be tricky, especially in GLSL:

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

Oryol already comes with a general source code generation mechanism integrated into the build process, called 'generator 
scripts'. This usually converts one XML file into one C++ header and one C++ source file through a python script selected 
in the header of the XML file.

This process is already integrated transparently as a CMake pre-build-step into the compile process.

For shader generation there is only a minimal XML file per shader library which contains the name of the 
library and where to find the shader snippet files for the library, e.g.:

```xml
<Generator type="ShaderLibrary" name="PreLightPassLib">
    <AddDir path="shd/prelightpass"/>
    <AddDir path="shd/common"/>
</Generator>
```

This would gather all snippet sources from the directories "shd/prelightpass" and "shd/common" and 
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

Everything within **${ }** contains meta-data for the parser and either be removed from the final GLSL code, or replaced
with an actual GLSL statement.

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
    // normal GLSL code...
    return result;
}
