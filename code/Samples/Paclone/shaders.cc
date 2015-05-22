//-----------------------------------------------------------------------------
// #version:18# machine generated, do not edit!
//-----------------------------------------------------------------------------
#include "Pre.h"
#include "shaders.h"

namespace Oryol {
namespace Shaders {
#if ORYOL_OPENGL
const char* canvasVS_glsl100_src = 
"#define _POSITION gl_Position\n"
"attribute vec2 position;\n"
"attribute vec2 texcoord0;\n"
"varying vec2 uv;\n"
"void main() {\n"
"_POSITION = vec4(((position.xy * 2.0) - 1.0), 0.0, 1.0);\n"
"_POSITION.y = -_POSITION.y;\n"
"uv = texcoord0;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* crtVS_glsl100_src = 
"#define _POSITION gl_Position\n"
"attribute vec4 position;\n"
"attribute vec2 texcoord0;\n"
"varying vec2 uv;\n"
"void main() {\n"
"_POSITION = position;\n"
"uv = texcoord0;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* canvasFS_glsl100_src = 
"precision mediump float;\n"
"#define _TEXTURE2D texture2D\n"
"#define _COLOR gl_FragColor\n"
"uniform sampler2D tex;\n"
"varying vec2 uv;\n"
"void main() {\n"
"_COLOR = _TEXTURE2D(tex, uv);\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* crtFS_glsl100_src = 
"precision mediump float;\n"
"#define _COLOR gl_FragColor\n"
"#define _TEXTURE2D texture2D\n"
"uniform vec2 iResolution;\n"
"uniform sampler2D iChannel0;\n"
"varying vec2 uv;\n"
"vec2 res = vec2(224, 288);\n"
"float hardScan=-8.0;\n"
"float hardPix=-2.0;\n"
"vec2 warp=vec2(1.0/32.0,1.0/24.0);\n"
"float maskDark=0.75;\n"
"float maskLight=2.0;\n"
"float ToLinear1(float c){return(c<=0.04045)?c/12.92:pow((c+0.055)/1.055,2.4);}\n"
"vec3 ToLinear(vec3 c){return vec3(ToLinear1(c.r),ToLinear1(c.g),ToLinear1(c.b));}\n"
"float ToSrgb1(float c){return(c<0.0031308?c*12.92:1.055*pow(c,0.41666)-0.055);}\n"
"vec3 ToSrgb(vec3 c){return vec3(ToSrgb1(c.r),ToSrgb1(c.g),ToSrgb1(c.b));}\n"
"vec3 Fetch(vec2 pos,vec2 off){\n"
"pos=floor(pos*res+off)/res;\n"
"if(max(abs(pos.x-0.5),abs(pos.y-0.5))>0.5)return vec3(0.0,0.0,0.0);\n"
"return ToLinear(_TEXTURE2D(iChannel0,pos.xy,-16.0).rgb);}\n"
"vec2 Dist(vec2 pos){pos=pos*res;return -((pos-floor(pos))-vec2(0.5));}\n"
"float Gaus(float pos,float scale){return exp2(scale*pos*pos);}\n"
"vec3 Horz3(vec2 pos,float off){\n"
"vec3 b=Fetch(pos,vec2(-1.0,off));\n"
"vec3 c=Fetch(pos,vec2( 0.0,off));\n"
"vec3 d=Fetch(pos,vec2( 1.0,off));\n"
"float dst=Dist(pos).x;\n"
"float scale=hardPix;\n"
"float wb=Gaus(dst-1.0,scale);\n"
"float wc=Gaus(dst+0.0,scale);\n"
"float wd=Gaus(dst+1.0,scale);\n"
"return (b*wb+c*wc+d*wd)/(wb+wc+wd);}\n"
"vec3 Horz5(vec2 pos,float off){\n"
"vec3 a=Fetch(pos,vec2(-2.0,off));\n"
"vec3 b=Fetch(pos,vec2(-1.0,off));\n"
"vec3 c=Fetch(pos,vec2( 0.0,off));\n"
"vec3 d=Fetch(pos,vec2( 1.0,off));\n"
"vec3 e=Fetch(pos,vec2( 2.0,off));\n"
"float dst=Dist(pos).x;\n"
"float scale=hardPix;\n"
"float wa=Gaus(dst-2.0,scale);\n"
"float wb=Gaus(dst-1.0,scale);\n"
"float wc=Gaus(dst+0.0,scale);\n"
"float wd=Gaus(dst+1.0,scale);\n"
"float we=Gaus(dst+2.0,scale);\n"
"return (a*wa+b*wb+c*wc+d*wd+e*we)/(wa+wb+wc+wd+we);}\n"
"float Scan(vec2 pos,float off){\n"
"float dst=Dist(pos).y;\n"
"return Gaus(dst+off,hardScan);}\n"
"vec3 Tri(vec2 pos){\n"
"vec3 a=Horz3(pos,-1.0);\n"
"vec3 b=Horz5(pos, 0.0);\n"
"vec3 c=Horz3(pos, 1.0);\n"
"float wa=Scan(pos,-1.0);\n"
"float wb=Scan(pos, 0.0);\n"
"float wc=Scan(pos, 1.0);\n"
"return a*wa+b*wb+c*wc;}\n"
"vec2 Warp(vec2 pos){\n"
"pos=pos*2.0-1.0;\n"
"pos*=vec2(1.0+(pos.y*pos.y)*warp.x,1.0+(pos.x*pos.x)*warp.y);\n"
"return pos*0.5+0.5;}\n"
"vec3 Mask(vec2 pos){\n"
"pos.x+=pos.y*3.0;\n"
"vec3 mask=vec3(maskDark,maskDark,maskDark);\n"
"pos.x=fract(pos.x/6.0);\n"
"if(pos.x<0.333)mask.r=maskLight;\n"
"else if(pos.x<0.666)mask.g=maskLight;\n"
"else mask.b=maskLight;\n"
"return mask;}\n"
"vec4 tl_crt(vec2 uv) {\n"
"vec2 pos=Warp(uv);\n"
"return vec4(ToSrgb(Tri(pos) * Mask(gl_FragCoord.xy)), 1.0);}\n"
"void main() {\n"
"#if 1\n"
"_COLOR = tl_crt(uv);\n"
"#else\n"
"_COLOR = _TEXTURE2D(iChannel0, uv);\n"
"#endif\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* canvasVS_glsl120_src = 
"#version 120\n"
"#define _POSITION gl_Position\n"
"attribute vec2 position;\n"
"attribute vec2 texcoord0;\n"
"varying vec2 uv;\n"
"void main() {\n"
"_POSITION = vec4(((position.xy * 2.0) - 1.0), 0.0, 1.0);\n"
"_POSITION.y = -_POSITION.y;\n"
"uv = texcoord0;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* crtVS_glsl120_src = 
"#version 120\n"
"#define _POSITION gl_Position\n"
"attribute vec4 position;\n"
"attribute vec2 texcoord0;\n"
"varying vec2 uv;\n"
"void main() {\n"
"_POSITION = position;\n"
"uv = texcoord0;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* canvasFS_glsl120_src = 
"#version 120\n"
"#define _TEXTURE2D texture2D\n"
"#define _COLOR gl_FragColor\n"
"uniform sampler2D tex;\n"
"varying vec2 uv;\n"
"void main() {\n"
"_COLOR = _TEXTURE2D(tex, uv);\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* crtFS_glsl120_src = 
"#version 120\n"
"#define _COLOR gl_FragColor\n"
"#define _TEXTURE2D texture2D\n"
"uniform vec2 iResolution;\n"
"uniform sampler2D iChannel0;\n"
"varying vec2 uv;\n"
"vec2 res = vec2(224, 288);\n"
"float hardScan=-8.0;\n"
"float hardPix=-2.0;\n"
"vec2 warp=vec2(1.0/32.0,1.0/24.0);\n"
"float maskDark=0.75;\n"
"float maskLight=2.0;\n"
"float ToLinear1(float c){return(c<=0.04045)?c/12.92:pow((c+0.055)/1.055,2.4);}\n"
"vec3 ToLinear(vec3 c){return vec3(ToLinear1(c.r),ToLinear1(c.g),ToLinear1(c.b));}\n"
"float ToSrgb1(float c){return(c<0.0031308?c*12.92:1.055*pow(c,0.41666)-0.055);}\n"
"vec3 ToSrgb(vec3 c){return vec3(ToSrgb1(c.r),ToSrgb1(c.g),ToSrgb1(c.b));}\n"
"vec3 Fetch(vec2 pos,vec2 off){\n"
"pos=floor(pos*res+off)/res;\n"
"if(max(abs(pos.x-0.5),abs(pos.y-0.5))>0.5)return vec3(0.0,0.0,0.0);\n"
"return ToLinear(_TEXTURE2D(iChannel0,pos.xy,-16.0).rgb);}\n"
"vec2 Dist(vec2 pos){pos=pos*res;return -((pos-floor(pos))-vec2(0.5));}\n"
"float Gaus(float pos,float scale){return exp2(scale*pos*pos);}\n"
"vec3 Horz3(vec2 pos,float off){\n"
"vec3 b=Fetch(pos,vec2(-1.0,off));\n"
"vec3 c=Fetch(pos,vec2( 0.0,off));\n"
"vec3 d=Fetch(pos,vec2( 1.0,off));\n"
"float dst=Dist(pos).x;\n"
"float scale=hardPix;\n"
"float wb=Gaus(dst-1.0,scale);\n"
"float wc=Gaus(dst+0.0,scale);\n"
"float wd=Gaus(dst+1.0,scale);\n"
"return (b*wb+c*wc+d*wd)/(wb+wc+wd);}\n"
"vec3 Horz5(vec2 pos,float off){\n"
"vec3 a=Fetch(pos,vec2(-2.0,off));\n"
"vec3 b=Fetch(pos,vec2(-1.0,off));\n"
"vec3 c=Fetch(pos,vec2( 0.0,off));\n"
"vec3 d=Fetch(pos,vec2( 1.0,off));\n"
"vec3 e=Fetch(pos,vec2( 2.0,off));\n"
"float dst=Dist(pos).x;\n"
"float scale=hardPix;\n"
"float wa=Gaus(dst-2.0,scale);\n"
"float wb=Gaus(dst-1.0,scale);\n"
"float wc=Gaus(dst+0.0,scale);\n"
"float wd=Gaus(dst+1.0,scale);\n"
"float we=Gaus(dst+2.0,scale);\n"
"return (a*wa+b*wb+c*wc+d*wd+e*we)/(wa+wb+wc+wd+we);}\n"
"float Scan(vec2 pos,float off){\n"
"float dst=Dist(pos).y;\n"
"return Gaus(dst+off,hardScan);}\n"
"vec3 Tri(vec2 pos){\n"
"vec3 a=Horz3(pos,-1.0);\n"
"vec3 b=Horz5(pos, 0.0);\n"
"vec3 c=Horz3(pos, 1.0);\n"
"float wa=Scan(pos,-1.0);\n"
"float wb=Scan(pos, 0.0);\n"
"float wc=Scan(pos, 1.0);\n"
"return a*wa+b*wb+c*wc;}\n"
"vec2 Warp(vec2 pos){\n"
"pos=pos*2.0-1.0;\n"
"pos*=vec2(1.0+(pos.y*pos.y)*warp.x,1.0+(pos.x*pos.x)*warp.y);\n"
"return pos*0.5+0.5;}\n"
"vec3 Mask(vec2 pos){\n"
"pos.x+=pos.y*3.0;\n"
"vec3 mask=vec3(maskDark,maskDark,maskDark);\n"
"pos.x=fract(pos.x/6.0);\n"
"if(pos.x<0.333)mask.r=maskLight;\n"
"else if(pos.x<0.666)mask.g=maskLight;\n"
"else mask.b=maskLight;\n"
"return mask;}\n"
"vec4 tl_crt(vec2 uv) {\n"
"vec2 pos=Warp(uv);\n"
"return vec4(ToSrgb(Tri(pos) * Mask(gl_FragCoord.xy)), 1.0);}\n"
"void main() {\n"
"#if 1\n"
"_COLOR = tl_crt(uv);\n"
"#else\n"
"_COLOR = _TEXTURE2D(iChannel0, uv);\n"
"#endif\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* canvasVS_glsl150_src = 
"#version 150\n"
"#define _POSITION gl_Position\n"
"in vec2 position;\n"
"in vec2 texcoord0;\n"
"out vec2 uv;\n"
"void main() {\n"
"_POSITION = vec4(((position.xy * 2.0) - 1.0), 0.0, 1.0);\n"
"_POSITION.y = -_POSITION.y;\n"
"uv = texcoord0;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* crtVS_glsl150_src = 
"#version 150\n"
"#define _POSITION gl_Position\n"
"in vec4 position;\n"
"in vec2 texcoord0;\n"
"out vec2 uv;\n"
"void main() {\n"
"_POSITION = position;\n"
"uv = texcoord0;\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* canvasFS_glsl150_src = 
"#version 150\n"
"#define _TEXTURE2D texture\n"
"#define _COLOR _FragColor\n"
"uniform sampler2D tex;\n"
"in vec2 uv;\n"
"out vec4 _FragColor;\n"
"void main() {\n"
"_COLOR = _TEXTURE2D(tex, uv);\n"
"}\n"
;
#endif
#if ORYOL_OPENGL
const char* crtFS_glsl150_src = 
"#version 150\n"
"#define _COLOR _FragColor\n"
"#define _TEXTURE2D texture\n"
"uniform vec2 iResolution;\n"
"uniform sampler2D iChannel0;\n"
"in vec2 uv;\n"
"out vec4 _FragColor;\n"
"vec2 res = vec2(224, 288);\n"
"float hardScan=-8.0;\n"
"float hardPix=-2.0;\n"
"vec2 warp=vec2(1.0/32.0,1.0/24.0);\n"
"float maskDark=0.75;\n"
"float maskLight=2.0;\n"
"float ToLinear1(float c){return(c<=0.04045)?c/12.92:pow((c+0.055)/1.055,2.4);}\n"
"vec3 ToLinear(vec3 c){return vec3(ToLinear1(c.r),ToLinear1(c.g),ToLinear1(c.b));}\n"
"float ToSrgb1(float c){return(c<0.0031308?c*12.92:1.055*pow(c,0.41666)-0.055);}\n"
"vec3 ToSrgb(vec3 c){return vec3(ToSrgb1(c.r),ToSrgb1(c.g),ToSrgb1(c.b));}\n"
"vec3 Fetch(vec2 pos,vec2 off){\n"
"pos=floor(pos*res+off)/res;\n"
"if(max(abs(pos.x-0.5),abs(pos.y-0.5))>0.5)return vec3(0.0,0.0,0.0);\n"
"return ToLinear(_TEXTURE2D(iChannel0,pos.xy,-16.0).rgb);}\n"
"vec2 Dist(vec2 pos){pos=pos*res;return -((pos-floor(pos))-vec2(0.5));}\n"
"float Gaus(float pos,float scale){return exp2(scale*pos*pos);}\n"
"vec3 Horz3(vec2 pos,float off){\n"
"vec3 b=Fetch(pos,vec2(-1.0,off));\n"
"vec3 c=Fetch(pos,vec2( 0.0,off));\n"
"vec3 d=Fetch(pos,vec2( 1.0,off));\n"
"float dst=Dist(pos).x;\n"
"float scale=hardPix;\n"
"float wb=Gaus(dst-1.0,scale);\n"
"float wc=Gaus(dst+0.0,scale);\n"
"float wd=Gaus(dst+1.0,scale);\n"
"return (b*wb+c*wc+d*wd)/(wb+wc+wd);}\n"
"vec3 Horz5(vec2 pos,float off){\n"
"vec3 a=Fetch(pos,vec2(-2.0,off));\n"
"vec3 b=Fetch(pos,vec2(-1.0,off));\n"
"vec3 c=Fetch(pos,vec2( 0.0,off));\n"
"vec3 d=Fetch(pos,vec2( 1.0,off));\n"
"vec3 e=Fetch(pos,vec2( 2.0,off));\n"
"float dst=Dist(pos).x;\n"
"float scale=hardPix;\n"
"float wa=Gaus(dst-2.0,scale);\n"
"float wb=Gaus(dst-1.0,scale);\n"
"float wc=Gaus(dst+0.0,scale);\n"
"float wd=Gaus(dst+1.0,scale);\n"
"float we=Gaus(dst+2.0,scale);\n"
"return (a*wa+b*wb+c*wc+d*wd+e*we)/(wa+wb+wc+wd+we);}\n"
"float Scan(vec2 pos,float off){\n"
"float dst=Dist(pos).y;\n"
"return Gaus(dst+off,hardScan);}\n"
"vec3 Tri(vec2 pos){\n"
"vec3 a=Horz3(pos,-1.0);\n"
"vec3 b=Horz5(pos, 0.0);\n"
"vec3 c=Horz3(pos, 1.0);\n"
"float wa=Scan(pos,-1.0);\n"
"float wb=Scan(pos, 0.0);\n"
"float wc=Scan(pos, 1.0);\n"
"return a*wa+b*wb+c*wc;}\n"
"vec2 Warp(vec2 pos){\n"
"pos=pos*2.0-1.0;\n"
"pos*=vec2(1.0+(pos.y*pos.y)*warp.x,1.0+(pos.x*pos.x)*warp.y);\n"
"return pos*0.5+0.5;}\n"
"vec3 Mask(vec2 pos){\n"
"pos.x+=pos.y*3.0;\n"
"vec3 mask=vec3(maskDark,maskDark,maskDark);\n"
"pos.x=fract(pos.x/6.0);\n"
"if(pos.x<0.333)mask.r=maskLight;\n"
"else if(pos.x<0.666)mask.g=maskLight;\n"
"else mask.b=maskLight;\n"
"return mask;}\n"
"vec4 tl_crt(vec2 uv) {\n"
"vec2 pos=Warp(uv);\n"
"return vec4(ToSrgb(Tri(pos) * Mask(gl_FragCoord.xy)), 1.0);}\n"
"void main() {\n"
"#if 1\n"
"_COLOR = tl_crt(uv);\n"
"#else\n"
"_COLOR = _TEXTURE2D(iChannel0, uv);\n"
"#endif\n"
"}\n"
;
#endif
ProgramBundleSetup Canvas::CreateSetup() {
    ProgramBundleSetup setup("Canvas");
    #if ORYOL_OPENGL
    setup.AddProgramFromSources(0, ShaderLang::GLSL100, canvasVS_glsl100_src, canvasFS_glsl100_src);
    #endif
    #if ORYOL_OPENGL
    setup.AddProgramFromSources(0, ShaderLang::GLSL120, canvasVS_glsl120_src, canvasFS_glsl120_src);
    #endif
    #if ORYOL_OPENGL
    setup.AddProgramFromSources(0, ShaderLang::GLSL150, canvasVS_glsl150_src, canvasFS_glsl150_src);
    #endif
    #if ORYOL_D3D11
    setup.AddProgramFromSources(0, ShaderLang::HLSL5, canvasVS_hlsl5_src, canvasFS_hlsl5_src);
    #endif
    setup.AddTextureUniform("tex", Texture);
    return setup;
}
ProgramBundleSetup CRT::CreateSetup() {
    ProgramBundleSetup setup("CRT");
    #if ORYOL_OPENGL
    setup.AddProgramFromSources(0, ShaderLang::GLSL100, crtVS_glsl100_src, crtFS_glsl100_src);
    #endif
    #if ORYOL_OPENGL
    setup.AddProgramFromSources(0, ShaderLang::GLSL120, crtVS_glsl120_src, crtFS_glsl120_src);
    #endif
    #if ORYOL_OPENGL
    setup.AddProgramFromSources(0, ShaderLang::GLSL150, crtVS_glsl150_src, crtFS_glsl150_src);
    #endif
    #if ORYOL_D3D11
    setup.AddProgramFromSources(0, ShaderLang::HLSL5, crtVS_hlsl5_src, crtFS_hlsl5_src);
    #endif
    setup.AddUniform("iResolution", Resolution);
    setup.AddTextureUniform("iChannel0", Canvas);
    return setup;
}
}
}

