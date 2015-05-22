//-----------------------------------------------------------------------------
// #version:17# machine generated, do not edit!
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "SynthShaders.h"

namespace Oryol {
namespace Shaders {
#if ORYOL_OPENGL
const char* vsSynth_glsl100_src = 
"#define _POSITION gl_Position\n"
"attribute vec4 position;\n"
"void main() {\n"
"_POSITION = position;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* fsSynth_glsl100_src = 
"precision mediump float;\n"
"#define _COLOR gl_FragColor\n"
"varying vec2 uv;\n"
"void main() {\n"
"vec2 s0 = vec2(1.0 - 1.0/32767.0, 0.5 - 1.0/32767.0);\n"
"vec2 s1 = vec2(0.0, 0.5);\n"
"_COLOR = vec4(s0, s1);\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* vsSynth_glsl120_src = 
"#version 120\n"
"#define _POSITION gl_Position\n"
"attribute vec4 position;\n"
"void main() {\n"
"_POSITION = position;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* fsSynth_glsl120_src = 
"#version 120\n"
"#define _COLOR gl_FragColor\n"
"varying vec2 uv;\n"
"void main() {\n"
"vec2 s0 = vec2(1.0 - 1.0/32767.0, 0.5 - 1.0/32767.0);\n"
"vec2 s1 = vec2(0.0, 0.5);\n"
"_COLOR = vec4(s0, s1);\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* vsSynth_glsl150_src = 
"#version 150\n"
"#define _POSITION gl_Position\n"
"in vec4 position;\n"
"void main() {\n"
"_POSITION = position;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* fsSynth_glsl150_src = 
"#version 150\n"
"#define _COLOR _FragColor\n"
"in vec2 uv;\n"
"out vec4 _FragColor;\n"
"void main() {\n"
"vec2 s0 = vec2(1.0 - 1.0/32767.0, 0.5 - 1.0/32767.0);\n"
"vec2 s1 = vec2(0.0, 0.5);\n"
"_COLOR = vec4(s0, s1);\n"
"}\n"
;
#endif
ProgramBundleSetup Synth::CreateSetup() {
    ProgramBundleSetup setup("Synth");
    #if ORYOL_OPENGL
    setup.AddProgramFromSources(0, ShaderLang::GLSL100, vsSynth_glsl100_src, fsSynth_glsl100_src);
    #endif
    #if ORYOL_OPENGL
    setup.AddProgramFromSources(0, ShaderLang::GLSL120, vsSynth_glsl120_src, fsSynth_glsl120_src);
    #endif
    #if ORYOL_OPENGL
    setup.AddProgramFromSources(0, ShaderLang::GLSL150, vsSynth_glsl150_src, fsSynth_glsl150_src);
    #endif
    #if ORYOL_D3D11
    setup.AddProgramFromSources(0, ShaderLang::HLSL5, vsSynth_hlsl5_src, fsSynth_hlsl5_src);
    #endif
    return setup;
}
}
}

