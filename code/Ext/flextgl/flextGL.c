/* WARNING: This file was automatically generated */
/* Do not edit. */

#include "flextGL.h"
#include "GLFW/glfw3.h"

#include <stdio.h>


#ifdef __cplusplus
extern "C" {
#endif


void flextLoadOpenGLFunctions(void);

int flextInit(GLFWwindow* window)
{
  
    int major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
    int minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);

    flextLoadOpenGLFunctions();
    
    /* --- Check for minimal version and profile --- */

    if (major * 10 + minor < 33) {
        fprintf(stderr, "Error: OpenGL version 3.3 not supported.\n");
        fprintf(stderr, "       Your version is %d.%d.\n", major, minor);
        fprintf(stderr, "       Try updating your graphics driver.\n");
        return GL_FALSE;
    }


    /* --- Check for extensions --- */

    if (glfwExtensionSupported("GL_ARB_debug_output")) {
        FLEXT_ARB_debug_output = GL_TRUE;
    }


    return GL_TRUE;
}



void flextLoadOpenGLFunctions(void)
{
    /* --- Function pointer loading --- */


    /* GL_VERSION_1_2 */

    glpfDrawRangeElements = (PFNGLDRAWRANGEELEMENTS_PROC*)glfwGetProcAddress("glDrawRangeElements");
    glpfTexImage3D = (PFNGLTEXIMAGE3D_PROC*)glfwGetProcAddress("glTexImage3D");
    glpfTexSubImage3D = (PFNGLTEXSUBIMAGE3D_PROC*)glfwGetProcAddress("glTexSubImage3D");
    glpfCopyTexSubImage3D = (PFNGLCOPYTEXSUBIMAGE3D_PROC*)glfwGetProcAddress("glCopyTexSubImage3D");


    /* GL_VERSION_1_3 */

    glpfActiveTexture = (PFNGLACTIVETEXTURE_PROC*)glfwGetProcAddress("glActiveTexture");
    glpfSampleCoverage = (PFNGLSAMPLECOVERAGE_PROC*)glfwGetProcAddress("glSampleCoverage");
    glpfCompressedTexImage3D = (PFNGLCOMPRESSEDTEXIMAGE3D_PROC*)glfwGetProcAddress("glCompressedTexImage3D");
    glpfCompressedTexImage2D = (PFNGLCOMPRESSEDTEXIMAGE2D_PROC*)glfwGetProcAddress("glCompressedTexImage2D");
    glpfCompressedTexImage1D = (PFNGLCOMPRESSEDTEXIMAGE1D_PROC*)glfwGetProcAddress("glCompressedTexImage1D");
    glpfCompressedTexSubImage3D = (PFNGLCOMPRESSEDTEXSUBIMAGE3D_PROC*)glfwGetProcAddress("glCompressedTexSubImage3D");
    glpfCompressedTexSubImage2D = (PFNGLCOMPRESSEDTEXSUBIMAGE2D_PROC*)glfwGetProcAddress("glCompressedTexSubImage2D");
    glpfCompressedTexSubImage1D = (PFNGLCOMPRESSEDTEXSUBIMAGE1D_PROC*)glfwGetProcAddress("glCompressedTexSubImage1D");
    glpfGetCompressedTexImage = (PFNGLGETCOMPRESSEDTEXIMAGE_PROC*)glfwGetProcAddress("glGetCompressedTexImage");


    /* GL_VERSION_1_4 */

    glpfBlendFuncSeparate = (PFNGLBLENDFUNCSEPARATE_PROC*)glfwGetProcAddress("glBlendFuncSeparate");
    glpfMultiDrawArrays = (PFNGLMULTIDRAWARRAYS_PROC*)glfwGetProcAddress("glMultiDrawArrays");
    glpfMultiDrawElements = (PFNGLMULTIDRAWELEMENTS_PROC*)glfwGetProcAddress("glMultiDrawElements");
    glpfPointParameterf = (PFNGLPOINTPARAMETERF_PROC*)glfwGetProcAddress("glPointParameterf");
    glpfPointParameterfv = (PFNGLPOINTPARAMETERFV_PROC*)glfwGetProcAddress("glPointParameterfv");
    glpfPointParameteri = (PFNGLPOINTPARAMETERI_PROC*)glfwGetProcAddress("glPointParameteri");
    glpfPointParameteriv = (PFNGLPOINTPARAMETERIV_PROC*)glfwGetProcAddress("glPointParameteriv");
    glpfBlendColor = (PFNGLBLENDCOLOR_PROC*)glfwGetProcAddress("glBlendColor");
    glpfBlendEquation = (PFNGLBLENDEQUATION_PROC*)glfwGetProcAddress("glBlendEquation");


    /* GL_VERSION_1_5 */

    glpfGenQueries = (PFNGLGENQUERIES_PROC*)glfwGetProcAddress("glGenQueries");
    glpfDeleteQueries = (PFNGLDELETEQUERIES_PROC*)glfwGetProcAddress("glDeleteQueries");
    glpfIsQuery = (PFNGLISQUERY_PROC*)glfwGetProcAddress("glIsQuery");
    glpfBeginQuery = (PFNGLBEGINQUERY_PROC*)glfwGetProcAddress("glBeginQuery");
    glpfEndQuery = (PFNGLENDQUERY_PROC*)glfwGetProcAddress("glEndQuery");
    glpfGetQueryiv = (PFNGLGETQUERYIV_PROC*)glfwGetProcAddress("glGetQueryiv");
    glpfGetQueryObjectiv = (PFNGLGETQUERYOBJECTIV_PROC*)glfwGetProcAddress("glGetQueryObjectiv");
    glpfGetQueryObjectuiv = (PFNGLGETQUERYOBJECTUIV_PROC*)glfwGetProcAddress("glGetQueryObjectuiv");
    glpfBindBuffer = (PFNGLBINDBUFFER_PROC*)glfwGetProcAddress("glBindBuffer");
    glpfDeleteBuffers = (PFNGLDELETEBUFFERS_PROC*)glfwGetProcAddress("glDeleteBuffers");
    glpfGenBuffers = (PFNGLGENBUFFERS_PROC*)glfwGetProcAddress("glGenBuffers");
    glpfIsBuffer = (PFNGLISBUFFER_PROC*)glfwGetProcAddress("glIsBuffer");
    glpfBufferData = (PFNGLBUFFERDATA_PROC*)glfwGetProcAddress("glBufferData");
    glpfBufferSubData = (PFNGLBUFFERSUBDATA_PROC*)glfwGetProcAddress("glBufferSubData");
    glpfGetBufferSubData = (PFNGLGETBUFFERSUBDATA_PROC*)glfwGetProcAddress("glGetBufferSubData");
    glpfMapBuffer = (PFNGLMAPBUFFER_PROC*)glfwGetProcAddress("glMapBuffer");
    glpfUnmapBuffer = (PFNGLUNMAPBUFFER_PROC*)glfwGetProcAddress("glUnmapBuffer");
    glpfGetBufferParameteriv = (PFNGLGETBUFFERPARAMETERIV_PROC*)glfwGetProcAddress("glGetBufferParameteriv");
    glpfGetBufferPointerv = (PFNGLGETBUFFERPOINTERV_PROC*)glfwGetProcAddress("glGetBufferPointerv");


    /* GL_VERSION_2_0 */

    glpfBlendEquationSeparate = (PFNGLBLENDEQUATIONSEPARATE_PROC*)glfwGetProcAddress("glBlendEquationSeparate");
    glpfDrawBuffers = (PFNGLDRAWBUFFERS_PROC*)glfwGetProcAddress("glDrawBuffers");
    glpfStencilOpSeparate = (PFNGLSTENCILOPSEPARATE_PROC*)glfwGetProcAddress("glStencilOpSeparate");
    glpfStencilFuncSeparate = (PFNGLSTENCILFUNCSEPARATE_PROC*)glfwGetProcAddress("glStencilFuncSeparate");
    glpfStencilMaskSeparate = (PFNGLSTENCILMASKSEPARATE_PROC*)glfwGetProcAddress("glStencilMaskSeparate");
    glpfAttachShader = (PFNGLATTACHSHADER_PROC*)glfwGetProcAddress("glAttachShader");
    glpfBindAttribLocation = (PFNGLBINDATTRIBLOCATION_PROC*)glfwGetProcAddress("glBindAttribLocation");
    glpfCompileShader = (PFNGLCOMPILESHADER_PROC*)glfwGetProcAddress("glCompileShader");
    glpfCreateProgram = (PFNGLCREATEPROGRAM_PROC*)glfwGetProcAddress("glCreateProgram");
    glpfCreateShader = (PFNGLCREATESHADER_PROC*)glfwGetProcAddress("glCreateShader");
    glpfDeleteProgram = (PFNGLDELETEPROGRAM_PROC*)glfwGetProcAddress("glDeleteProgram");
    glpfDeleteShader = (PFNGLDELETESHADER_PROC*)glfwGetProcAddress("glDeleteShader");
    glpfDetachShader = (PFNGLDETACHSHADER_PROC*)glfwGetProcAddress("glDetachShader");
    glpfDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAY_PROC*)glfwGetProcAddress("glDisableVertexAttribArray");
    glpfEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAY_PROC*)glfwGetProcAddress("glEnableVertexAttribArray");
    glpfGetActiveAttrib = (PFNGLGETACTIVEATTRIB_PROC*)glfwGetProcAddress("glGetActiveAttrib");
    glpfGetActiveUniform = (PFNGLGETACTIVEUNIFORM_PROC*)glfwGetProcAddress("glGetActiveUniform");
    glpfGetAttachedShaders = (PFNGLGETATTACHEDSHADERS_PROC*)glfwGetProcAddress("glGetAttachedShaders");
    glpfGetAttribLocation = (PFNGLGETATTRIBLOCATION_PROC*)glfwGetProcAddress("glGetAttribLocation");
    glpfGetProgramiv = (PFNGLGETPROGRAMIV_PROC*)glfwGetProcAddress("glGetProgramiv");
    glpfGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOG_PROC*)glfwGetProcAddress("glGetProgramInfoLog");
    glpfGetShaderiv = (PFNGLGETSHADERIV_PROC*)glfwGetProcAddress("glGetShaderiv");
    glpfGetShaderInfoLog = (PFNGLGETSHADERINFOLOG_PROC*)glfwGetProcAddress("glGetShaderInfoLog");
    glpfGetShaderSource = (PFNGLGETSHADERSOURCE_PROC*)glfwGetProcAddress("glGetShaderSource");
    glpfGetUniformLocation = (PFNGLGETUNIFORMLOCATION_PROC*)glfwGetProcAddress("glGetUniformLocation");
    glpfGetUniformfv = (PFNGLGETUNIFORMFV_PROC*)glfwGetProcAddress("glGetUniformfv");
    glpfGetUniformiv = (PFNGLGETUNIFORMIV_PROC*)glfwGetProcAddress("glGetUniformiv");
    glpfGetVertexAttribdv = (PFNGLGETVERTEXATTRIBDV_PROC*)glfwGetProcAddress("glGetVertexAttribdv");
    glpfGetVertexAttribfv = (PFNGLGETVERTEXATTRIBFV_PROC*)glfwGetProcAddress("glGetVertexAttribfv");
    glpfGetVertexAttribiv = (PFNGLGETVERTEXATTRIBIV_PROC*)glfwGetProcAddress("glGetVertexAttribiv");
    glpfGetVertexAttribPointerv = (PFNGLGETVERTEXATTRIBPOINTERV_PROC*)glfwGetProcAddress("glGetVertexAttribPointerv");
    glpfIsProgram = (PFNGLISPROGRAM_PROC*)glfwGetProcAddress("glIsProgram");
    glpfIsShader = (PFNGLISSHADER_PROC*)glfwGetProcAddress("glIsShader");
    glpfLinkProgram = (PFNGLLINKPROGRAM_PROC*)glfwGetProcAddress("glLinkProgram");
    glpfShaderSource = (PFNGLSHADERSOURCE_PROC*)glfwGetProcAddress("glShaderSource");
    glpfUseProgram = (PFNGLUSEPROGRAM_PROC*)glfwGetProcAddress("glUseProgram");
    glpfUniform1f = (PFNGLUNIFORM1F_PROC*)glfwGetProcAddress("glUniform1f");
    glpfUniform2f = (PFNGLUNIFORM2F_PROC*)glfwGetProcAddress("glUniform2f");
    glpfUniform3f = (PFNGLUNIFORM3F_PROC*)glfwGetProcAddress("glUniform3f");
    glpfUniform4f = (PFNGLUNIFORM4F_PROC*)glfwGetProcAddress("glUniform4f");
    glpfUniform1i = (PFNGLUNIFORM1I_PROC*)glfwGetProcAddress("glUniform1i");
    glpfUniform2i = (PFNGLUNIFORM2I_PROC*)glfwGetProcAddress("glUniform2i");
    glpfUniform3i = (PFNGLUNIFORM3I_PROC*)glfwGetProcAddress("glUniform3i");
    glpfUniform4i = (PFNGLUNIFORM4I_PROC*)glfwGetProcAddress("glUniform4i");
    glpfUniform1fv = (PFNGLUNIFORM1FV_PROC*)glfwGetProcAddress("glUniform1fv");
    glpfUniform2fv = (PFNGLUNIFORM2FV_PROC*)glfwGetProcAddress("glUniform2fv");
    glpfUniform3fv = (PFNGLUNIFORM3FV_PROC*)glfwGetProcAddress("glUniform3fv");
    glpfUniform4fv = (PFNGLUNIFORM4FV_PROC*)glfwGetProcAddress("glUniform4fv");
    glpfUniform1iv = (PFNGLUNIFORM1IV_PROC*)glfwGetProcAddress("glUniform1iv");
    glpfUniform2iv = (PFNGLUNIFORM2IV_PROC*)glfwGetProcAddress("glUniform2iv");
    glpfUniform3iv = (PFNGLUNIFORM3IV_PROC*)glfwGetProcAddress("glUniform3iv");
    glpfUniform4iv = (PFNGLUNIFORM4IV_PROC*)glfwGetProcAddress("glUniform4iv");
    glpfUniformMatrix2fv = (PFNGLUNIFORMMATRIX2FV_PROC*)glfwGetProcAddress("glUniformMatrix2fv");
    glpfUniformMatrix3fv = (PFNGLUNIFORMMATRIX3FV_PROC*)glfwGetProcAddress("glUniformMatrix3fv");
    glpfUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FV_PROC*)glfwGetProcAddress("glUniformMatrix4fv");
    glpfValidateProgram = (PFNGLVALIDATEPROGRAM_PROC*)glfwGetProcAddress("glValidateProgram");
    glpfVertexAttrib1d = (PFNGLVERTEXATTRIB1D_PROC*)glfwGetProcAddress("glVertexAttrib1d");
    glpfVertexAttrib1dv = (PFNGLVERTEXATTRIB1DV_PROC*)glfwGetProcAddress("glVertexAttrib1dv");
    glpfVertexAttrib1f = (PFNGLVERTEXATTRIB1F_PROC*)glfwGetProcAddress("glVertexAttrib1f");
    glpfVertexAttrib1fv = (PFNGLVERTEXATTRIB1FV_PROC*)glfwGetProcAddress("glVertexAttrib1fv");
    glpfVertexAttrib1s = (PFNGLVERTEXATTRIB1S_PROC*)glfwGetProcAddress("glVertexAttrib1s");
    glpfVertexAttrib1sv = (PFNGLVERTEXATTRIB1SV_PROC*)glfwGetProcAddress("glVertexAttrib1sv");
    glpfVertexAttrib2d = (PFNGLVERTEXATTRIB2D_PROC*)glfwGetProcAddress("glVertexAttrib2d");
    glpfVertexAttrib2dv = (PFNGLVERTEXATTRIB2DV_PROC*)glfwGetProcAddress("glVertexAttrib2dv");
    glpfVertexAttrib2f = (PFNGLVERTEXATTRIB2F_PROC*)glfwGetProcAddress("glVertexAttrib2f");
    glpfVertexAttrib2fv = (PFNGLVERTEXATTRIB2FV_PROC*)glfwGetProcAddress("glVertexAttrib2fv");
    glpfVertexAttrib2s = (PFNGLVERTEXATTRIB2S_PROC*)glfwGetProcAddress("glVertexAttrib2s");
    glpfVertexAttrib2sv = (PFNGLVERTEXATTRIB2SV_PROC*)glfwGetProcAddress("glVertexAttrib2sv");
    glpfVertexAttrib3d = (PFNGLVERTEXATTRIB3D_PROC*)glfwGetProcAddress("glVertexAttrib3d");
    glpfVertexAttrib3dv = (PFNGLVERTEXATTRIB3DV_PROC*)glfwGetProcAddress("glVertexAttrib3dv");
    glpfVertexAttrib3f = (PFNGLVERTEXATTRIB3F_PROC*)glfwGetProcAddress("glVertexAttrib3f");
    glpfVertexAttrib3fv = (PFNGLVERTEXATTRIB3FV_PROC*)glfwGetProcAddress("glVertexAttrib3fv");
    glpfVertexAttrib3s = (PFNGLVERTEXATTRIB3S_PROC*)glfwGetProcAddress("glVertexAttrib3s");
    glpfVertexAttrib3sv = (PFNGLVERTEXATTRIB3SV_PROC*)glfwGetProcAddress("glVertexAttrib3sv");
    glpfVertexAttrib4Nbv = (PFNGLVERTEXATTRIB4NBV_PROC*)glfwGetProcAddress("glVertexAttrib4Nbv");
    glpfVertexAttrib4Niv = (PFNGLVERTEXATTRIB4NIV_PROC*)glfwGetProcAddress("glVertexAttrib4Niv");
    glpfVertexAttrib4Nsv = (PFNGLVERTEXATTRIB4NSV_PROC*)glfwGetProcAddress("glVertexAttrib4Nsv");
    glpfVertexAttrib4Nub = (PFNGLVERTEXATTRIB4NUB_PROC*)glfwGetProcAddress("glVertexAttrib4Nub");
    glpfVertexAttrib4Nubv = (PFNGLVERTEXATTRIB4NUBV_PROC*)glfwGetProcAddress("glVertexAttrib4Nubv");
    glpfVertexAttrib4Nuiv = (PFNGLVERTEXATTRIB4NUIV_PROC*)glfwGetProcAddress("glVertexAttrib4Nuiv");
    glpfVertexAttrib4Nusv = (PFNGLVERTEXATTRIB4NUSV_PROC*)glfwGetProcAddress("glVertexAttrib4Nusv");
    glpfVertexAttrib4bv = (PFNGLVERTEXATTRIB4BV_PROC*)glfwGetProcAddress("glVertexAttrib4bv");
    glpfVertexAttrib4d = (PFNGLVERTEXATTRIB4D_PROC*)glfwGetProcAddress("glVertexAttrib4d");
    glpfVertexAttrib4dv = (PFNGLVERTEXATTRIB4DV_PROC*)glfwGetProcAddress("glVertexAttrib4dv");
    glpfVertexAttrib4f = (PFNGLVERTEXATTRIB4F_PROC*)glfwGetProcAddress("glVertexAttrib4f");
    glpfVertexAttrib4fv = (PFNGLVERTEXATTRIB4FV_PROC*)glfwGetProcAddress("glVertexAttrib4fv");
    glpfVertexAttrib4iv = (PFNGLVERTEXATTRIB4IV_PROC*)glfwGetProcAddress("glVertexAttrib4iv");
    glpfVertexAttrib4s = (PFNGLVERTEXATTRIB4S_PROC*)glfwGetProcAddress("glVertexAttrib4s");
    glpfVertexAttrib4sv = (PFNGLVERTEXATTRIB4SV_PROC*)glfwGetProcAddress("glVertexAttrib4sv");
    glpfVertexAttrib4ubv = (PFNGLVERTEXATTRIB4UBV_PROC*)glfwGetProcAddress("glVertexAttrib4ubv");
    glpfVertexAttrib4uiv = (PFNGLVERTEXATTRIB4UIV_PROC*)glfwGetProcAddress("glVertexAttrib4uiv");
    glpfVertexAttrib4usv = (PFNGLVERTEXATTRIB4USV_PROC*)glfwGetProcAddress("glVertexAttrib4usv");
    glpfVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTER_PROC*)glfwGetProcAddress("glVertexAttribPointer");


    /* GL_VERSION_2_1 */

    glpfUniformMatrix2x3fv = (PFNGLUNIFORMMATRIX2X3FV_PROC*)glfwGetProcAddress("glUniformMatrix2x3fv");
    glpfUniformMatrix3x2fv = (PFNGLUNIFORMMATRIX3X2FV_PROC*)glfwGetProcAddress("glUniformMatrix3x2fv");
    glpfUniformMatrix2x4fv = (PFNGLUNIFORMMATRIX2X4FV_PROC*)glfwGetProcAddress("glUniformMatrix2x4fv");
    glpfUniformMatrix4x2fv = (PFNGLUNIFORMMATRIX4X2FV_PROC*)glfwGetProcAddress("glUniformMatrix4x2fv");
    glpfUniformMatrix3x4fv = (PFNGLUNIFORMMATRIX3X4FV_PROC*)glfwGetProcAddress("glUniformMatrix3x4fv");
    glpfUniformMatrix4x3fv = (PFNGLUNIFORMMATRIX4X3FV_PROC*)glfwGetProcAddress("glUniformMatrix4x3fv");


    /* GL_VERSION_3_0 */

    glpfColorMaski = (PFNGLCOLORMASKI_PROC*)glfwGetProcAddress("glColorMaski");
    glpfGetBooleani_v = (PFNGLGETBOOLEANI_V_PROC*)glfwGetProcAddress("glGetBooleani_v");
    glpfGetIntegeri_v = (PFNGLGETINTEGERI_V_PROC*)glfwGetProcAddress("glGetIntegeri_v");
    glpfEnablei = (PFNGLENABLEI_PROC*)glfwGetProcAddress("glEnablei");
    glpfDisablei = (PFNGLDISABLEI_PROC*)glfwGetProcAddress("glDisablei");
    glpfIsEnabledi = (PFNGLISENABLEDI_PROC*)glfwGetProcAddress("glIsEnabledi");
    glpfBeginTransformFeedback = (PFNGLBEGINTRANSFORMFEEDBACK_PROC*)glfwGetProcAddress("glBeginTransformFeedback");
    glpfEndTransformFeedback = (PFNGLENDTRANSFORMFEEDBACK_PROC*)glfwGetProcAddress("glEndTransformFeedback");
    glpfBindBufferRange = (PFNGLBINDBUFFERRANGE_PROC*)glfwGetProcAddress("glBindBufferRange");
    glpfBindBufferBase = (PFNGLBINDBUFFERBASE_PROC*)glfwGetProcAddress("glBindBufferBase");
    glpfTransformFeedbackVaryings = (PFNGLTRANSFORMFEEDBACKVARYINGS_PROC*)glfwGetProcAddress("glTransformFeedbackVaryings");
    glpfGetTransformFeedbackVarying = (PFNGLGETTRANSFORMFEEDBACKVARYING_PROC*)glfwGetProcAddress("glGetTransformFeedbackVarying");
    glpfClampColor = (PFNGLCLAMPCOLOR_PROC*)glfwGetProcAddress("glClampColor");
    glpfBeginConditionalRender = (PFNGLBEGINCONDITIONALRENDER_PROC*)glfwGetProcAddress("glBeginConditionalRender");
    glpfEndConditionalRender = (PFNGLENDCONDITIONALRENDER_PROC*)glfwGetProcAddress("glEndConditionalRender");
    glpfVertexAttribIPointer = (PFNGLVERTEXATTRIBIPOINTER_PROC*)glfwGetProcAddress("glVertexAttribIPointer");
    glpfGetVertexAttribIiv = (PFNGLGETVERTEXATTRIBIIV_PROC*)glfwGetProcAddress("glGetVertexAttribIiv");
    glpfGetVertexAttribIuiv = (PFNGLGETVERTEXATTRIBIUIV_PROC*)glfwGetProcAddress("glGetVertexAttribIuiv");
    glpfVertexAttribI1i = (PFNGLVERTEXATTRIBI1I_PROC*)glfwGetProcAddress("glVertexAttribI1i");
    glpfVertexAttribI2i = (PFNGLVERTEXATTRIBI2I_PROC*)glfwGetProcAddress("glVertexAttribI2i");
    glpfVertexAttribI3i = (PFNGLVERTEXATTRIBI3I_PROC*)glfwGetProcAddress("glVertexAttribI3i");
    glpfVertexAttribI4i = (PFNGLVERTEXATTRIBI4I_PROC*)glfwGetProcAddress("glVertexAttribI4i");
    glpfVertexAttribI1ui = (PFNGLVERTEXATTRIBI1UI_PROC*)glfwGetProcAddress("glVertexAttribI1ui");
    glpfVertexAttribI2ui = (PFNGLVERTEXATTRIBI2UI_PROC*)glfwGetProcAddress("glVertexAttribI2ui");
    glpfVertexAttribI3ui = (PFNGLVERTEXATTRIBI3UI_PROC*)glfwGetProcAddress("glVertexAttribI3ui");
    glpfVertexAttribI4ui = (PFNGLVERTEXATTRIBI4UI_PROC*)glfwGetProcAddress("glVertexAttribI4ui");
    glpfVertexAttribI1iv = (PFNGLVERTEXATTRIBI1IV_PROC*)glfwGetProcAddress("glVertexAttribI1iv");
    glpfVertexAttribI2iv = (PFNGLVERTEXATTRIBI2IV_PROC*)glfwGetProcAddress("glVertexAttribI2iv");
    glpfVertexAttribI3iv = (PFNGLVERTEXATTRIBI3IV_PROC*)glfwGetProcAddress("glVertexAttribI3iv");
    glpfVertexAttribI4iv = (PFNGLVERTEXATTRIBI4IV_PROC*)glfwGetProcAddress("glVertexAttribI4iv");
    glpfVertexAttribI1uiv = (PFNGLVERTEXATTRIBI1UIV_PROC*)glfwGetProcAddress("glVertexAttribI1uiv");
    glpfVertexAttribI2uiv = (PFNGLVERTEXATTRIBI2UIV_PROC*)glfwGetProcAddress("glVertexAttribI2uiv");
    glpfVertexAttribI3uiv = (PFNGLVERTEXATTRIBI3UIV_PROC*)glfwGetProcAddress("glVertexAttribI3uiv");
    glpfVertexAttribI4uiv = (PFNGLVERTEXATTRIBI4UIV_PROC*)glfwGetProcAddress("glVertexAttribI4uiv");
    glpfVertexAttribI4bv = (PFNGLVERTEXATTRIBI4BV_PROC*)glfwGetProcAddress("glVertexAttribI4bv");
    glpfVertexAttribI4sv = (PFNGLVERTEXATTRIBI4SV_PROC*)glfwGetProcAddress("glVertexAttribI4sv");
    glpfVertexAttribI4ubv = (PFNGLVERTEXATTRIBI4UBV_PROC*)glfwGetProcAddress("glVertexAttribI4ubv");
    glpfVertexAttribI4usv = (PFNGLVERTEXATTRIBI4USV_PROC*)glfwGetProcAddress("glVertexAttribI4usv");
    glpfGetUniformuiv = (PFNGLGETUNIFORMUIV_PROC*)glfwGetProcAddress("glGetUniformuiv");
    glpfBindFragDataLocation = (PFNGLBINDFRAGDATALOCATION_PROC*)glfwGetProcAddress("glBindFragDataLocation");
    glpfGetFragDataLocation = (PFNGLGETFRAGDATALOCATION_PROC*)glfwGetProcAddress("glGetFragDataLocation");
    glpfUniform1ui = (PFNGLUNIFORM1UI_PROC*)glfwGetProcAddress("glUniform1ui");
    glpfUniform2ui = (PFNGLUNIFORM2UI_PROC*)glfwGetProcAddress("glUniform2ui");
    glpfUniform3ui = (PFNGLUNIFORM3UI_PROC*)glfwGetProcAddress("glUniform3ui");
    glpfUniform4ui = (PFNGLUNIFORM4UI_PROC*)glfwGetProcAddress("glUniform4ui");
    glpfUniform1uiv = (PFNGLUNIFORM1UIV_PROC*)glfwGetProcAddress("glUniform1uiv");
    glpfUniform2uiv = (PFNGLUNIFORM2UIV_PROC*)glfwGetProcAddress("glUniform2uiv");
    glpfUniform3uiv = (PFNGLUNIFORM3UIV_PROC*)glfwGetProcAddress("glUniform3uiv");
    glpfUniform4uiv = (PFNGLUNIFORM4UIV_PROC*)glfwGetProcAddress("glUniform4uiv");
    glpfTexParameterIiv = (PFNGLTEXPARAMETERIIV_PROC*)glfwGetProcAddress("glTexParameterIiv");
    glpfTexParameterIuiv = (PFNGLTEXPARAMETERIUIV_PROC*)glfwGetProcAddress("glTexParameterIuiv");
    glpfGetTexParameterIiv = (PFNGLGETTEXPARAMETERIIV_PROC*)glfwGetProcAddress("glGetTexParameterIiv");
    glpfGetTexParameterIuiv = (PFNGLGETTEXPARAMETERIUIV_PROC*)glfwGetProcAddress("glGetTexParameterIuiv");
    glpfClearBufferiv = (PFNGLCLEARBUFFERIV_PROC*)glfwGetProcAddress("glClearBufferiv");
    glpfClearBufferuiv = (PFNGLCLEARBUFFERUIV_PROC*)glfwGetProcAddress("glClearBufferuiv");
    glpfClearBufferfv = (PFNGLCLEARBUFFERFV_PROC*)glfwGetProcAddress("glClearBufferfv");
    glpfClearBufferfi = (PFNGLCLEARBUFFERFI_PROC*)glfwGetProcAddress("glClearBufferfi");
    glpfGetStringi = (PFNGLGETSTRINGI_PROC*)glfwGetProcAddress("glGetStringi");
    glpfIsRenderbuffer = (PFNGLISRENDERBUFFER_PROC*)glfwGetProcAddress("glIsRenderbuffer");
    glpfBindRenderbuffer = (PFNGLBINDRENDERBUFFER_PROC*)glfwGetProcAddress("glBindRenderbuffer");
    glpfDeleteRenderbuffers = (PFNGLDELETERENDERBUFFERS_PROC*)glfwGetProcAddress("glDeleteRenderbuffers");
    glpfGenRenderbuffers = (PFNGLGENRENDERBUFFERS_PROC*)glfwGetProcAddress("glGenRenderbuffers");
    glpfRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGE_PROC*)glfwGetProcAddress("glRenderbufferStorage");
    glpfGetRenderbufferParameteriv = (PFNGLGETRENDERBUFFERPARAMETERIV_PROC*)glfwGetProcAddress("glGetRenderbufferParameteriv");
    glpfIsFramebuffer = (PFNGLISFRAMEBUFFER_PROC*)glfwGetProcAddress("glIsFramebuffer");
    glpfBindFramebuffer = (PFNGLBINDFRAMEBUFFER_PROC*)glfwGetProcAddress("glBindFramebuffer");
    glpfDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERS_PROC*)glfwGetProcAddress("glDeleteFramebuffers");
    glpfGenFramebuffers = (PFNGLGENFRAMEBUFFERS_PROC*)glfwGetProcAddress("glGenFramebuffers");
    glpfCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUS_PROC*)glfwGetProcAddress("glCheckFramebufferStatus");
    glpfFramebufferTexture1D = (PFNGLFRAMEBUFFERTEXTURE1D_PROC*)glfwGetProcAddress("glFramebufferTexture1D");
    glpfFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2D_PROC*)glfwGetProcAddress("glFramebufferTexture2D");
    glpfFramebufferTexture3D = (PFNGLFRAMEBUFFERTEXTURE3D_PROC*)glfwGetProcAddress("glFramebufferTexture3D");
    glpfFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFER_PROC*)glfwGetProcAddress("glFramebufferRenderbuffer");
    glpfGetFramebufferAttachmentParameteriv = (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIV_PROC*)glfwGetProcAddress("glGetFramebufferAttachmentParameteriv");
    glpfGenerateMipmap = (PFNGLGENERATEMIPMAP_PROC*)glfwGetProcAddress("glGenerateMipmap");
    glpfBlitFramebuffer = (PFNGLBLITFRAMEBUFFER_PROC*)glfwGetProcAddress("glBlitFramebuffer");
    glpfRenderbufferStorageMultisample = (PFNGLRENDERBUFFERSTORAGEMULTISAMPLE_PROC*)glfwGetProcAddress("glRenderbufferStorageMultisample");
    glpfFramebufferTextureLayer = (PFNGLFRAMEBUFFERTEXTURELAYER_PROC*)glfwGetProcAddress("glFramebufferTextureLayer");
    glpfMapBufferRange = (PFNGLMAPBUFFERRANGE_PROC*)glfwGetProcAddress("glMapBufferRange");
    glpfFlushMappedBufferRange = (PFNGLFLUSHMAPPEDBUFFERRANGE_PROC*)glfwGetProcAddress("glFlushMappedBufferRange");
    glpfBindVertexArray = (PFNGLBINDVERTEXARRAY_PROC*)glfwGetProcAddress("glBindVertexArray");
    glpfDeleteVertexArrays = (PFNGLDELETEVERTEXARRAYS_PROC*)glfwGetProcAddress("glDeleteVertexArrays");
    glpfGenVertexArrays = (PFNGLGENVERTEXARRAYS_PROC*)glfwGetProcAddress("glGenVertexArrays");
    glpfIsVertexArray = (PFNGLISVERTEXARRAY_PROC*)glfwGetProcAddress("glIsVertexArray");


    /* GL_VERSION_3_1 */

    glpfDrawArraysInstanced = (PFNGLDRAWARRAYSINSTANCED_PROC*)glfwGetProcAddress("glDrawArraysInstanced");
    glpfDrawElementsInstanced = (PFNGLDRAWELEMENTSINSTANCED_PROC*)glfwGetProcAddress("glDrawElementsInstanced");
    glpfTexBuffer = (PFNGLTEXBUFFER_PROC*)glfwGetProcAddress("glTexBuffer");
    glpfPrimitiveRestartIndex = (PFNGLPRIMITIVERESTARTINDEX_PROC*)glfwGetProcAddress("glPrimitiveRestartIndex");
    glpfCopyBufferSubData = (PFNGLCOPYBUFFERSUBDATA_PROC*)glfwGetProcAddress("glCopyBufferSubData");
    glpfGetUniformIndices = (PFNGLGETUNIFORMINDICES_PROC*)glfwGetProcAddress("glGetUniformIndices");
    glpfGetActiveUniformsiv = (PFNGLGETACTIVEUNIFORMSIV_PROC*)glfwGetProcAddress("glGetActiveUniformsiv");
    glpfGetActiveUniformName = (PFNGLGETACTIVEUNIFORMNAME_PROC*)glfwGetProcAddress("glGetActiveUniformName");
    glpfGetUniformBlockIndex = (PFNGLGETUNIFORMBLOCKINDEX_PROC*)glfwGetProcAddress("glGetUniformBlockIndex");
    glpfGetActiveUniformBlockiv = (PFNGLGETACTIVEUNIFORMBLOCKIV_PROC*)glfwGetProcAddress("glGetActiveUniformBlockiv");
    glpfGetActiveUniformBlockName = (PFNGLGETACTIVEUNIFORMBLOCKNAME_PROC*)glfwGetProcAddress("glGetActiveUniformBlockName");
    glpfUniformBlockBinding = (PFNGLUNIFORMBLOCKBINDING_PROC*)glfwGetProcAddress("glUniformBlockBinding");


    /* GL_VERSION_3_2 */

    glpfDrawElementsBaseVertex = (PFNGLDRAWELEMENTSBASEVERTEX_PROC*)glfwGetProcAddress("glDrawElementsBaseVertex");
    glpfDrawRangeElementsBaseVertex = (PFNGLDRAWRANGEELEMENTSBASEVERTEX_PROC*)glfwGetProcAddress("glDrawRangeElementsBaseVertex");
    glpfDrawElementsInstancedBaseVertex = (PFNGLDRAWELEMENTSINSTANCEDBASEVERTEX_PROC*)glfwGetProcAddress("glDrawElementsInstancedBaseVertex");
    glpfMultiDrawElementsBaseVertex = (PFNGLMULTIDRAWELEMENTSBASEVERTEX_PROC*)glfwGetProcAddress("glMultiDrawElementsBaseVertex");
    glpfProvokingVertex = (PFNGLPROVOKINGVERTEX_PROC*)glfwGetProcAddress("glProvokingVertex");
    glpfFenceSync = (PFNGLFENCESYNC_PROC*)glfwGetProcAddress("glFenceSync");
    glpfIsSync = (PFNGLISSYNC_PROC*)glfwGetProcAddress("glIsSync");
    glpfDeleteSync = (PFNGLDELETESYNC_PROC*)glfwGetProcAddress("glDeleteSync");
    glpfClientWaitSync = (PFNGLCLIENTWAITSYNC_PROC*)glfwGetProcAddress("glClientWaitSync");
    glpfWaitSync = (PFNGLWAITSYNC_PROC*)glfwGetProcAddress("glWaitSync");
    glpfGetInteger64v = (PFNGLGETINTEGER64V_PROC*)glfwGetProcAddress("glGetInteger64v");
    glpfGetSynciv = (PFNGLGETSYNCIV_PROC*)glfwGetProcAddress("glGetSynciv");
    glpfGetInteger64i_v = (PFNGLGETINTEGER64I_V_PROC*)glfwGetProcAddress("glGetInteger64i_v");
    glpfGetBufferParameteri64v = (PFNGLGETBUFFERPARAMETERI64V_PROC*)glfwGetProcAddress("glGetBufferParameteri64v");
    glpfFramebufferTexture = (PFNGLFRAMEBUFFERTEXTURE_PROC*)glfwGetProcAddress("glFramebufferTexture");
    glpfTexImage2DMultisample = (PFNGLTEXIMAGE2DMULTISAMPLE_PROC*)glfwGetProcAddress("glTexImage2DMultisample");
    glpfTexImage3DMultisample = (PFNGLTEXIMAGE3DMULTISAMPLE_PROC*)glfwGetProcAddress("glTexImage3DMultisample");
    glpfGetMultisamplefv = (PFNGLGETMULTISAMPLEFV_PROC*)glfwGetProcAddress("glGetMultisamplefv");
    glpfSampleMaski = (PFNGLSAMPLEMASKI_PROC*)glfwGetProcAddress("glSampleMaski");


    /* GL_VERSION_3_3 */

    glpfBindFragDataLocationIndexed = (PFNGLBINDFRAGDATALOCATIONINDEXED_PROC*)glfwGetProcAddress("glBindFragDataLocationIndexed");
    glpfGetFragDataIndex = (PFNGLGETFRAGDATAINDEX_PROC*)glfwGetProcAddress("glGetFragDataIndex");
    glpfGenSamplers = (PFNGLGENSAMPLERS_PROC*)glfwGetProcAddress("glGenSamplers");
    glpfDeleteSamplers = (PFNGLDELETESAMPLERS_PROC*)glfwGetProcAddress("glDeleteSamplers");
    glpfIsSampler = (PFNGLISSAMPLER_PROC*)glfwGetProcAddress("glIsSampler");
    glpfBindSampler = (PFNGLBINDSAMPLER_PROC*)glfwGetProcAddress("glBindSampler");
    glpfSamplerParameteri = (PFNGLSAMPLERPARAMETERI_PROC*)glfwGetProcAddress("glSamplerParameteri");
    glpfSamplerParameteriv = (PFNGLSAMPLERPARAMETERIV_PROC*)glfwGetProcAddress("glSamplerParameteriv");
    glpfSamplerParameterf = (PFNGLSAMPLERPARAMETERF_PROC*)glfwGetProcAddress("glSamplerParameterf");
    glpfSamplerParameterfv = (PFNGLSAMPLERPARAMETERFV_PROC*)glfwGetProcAddress("glSamplerParameterfv");
    glpfSamplerParameterIiv = (PFNGLSAMPLERPARAMETERIIV_PROC*)glfwGetProcAddress("glSamplerParameterIiv");
    glpfSamplerParameterIuiv = (PFNGLSAMPLERPARAMETERIUIV_PROC*)glfwGetProcAddress("glSamplerParameterIuiv");
    glpfGetSamplerParameteriv = (PFNGLGETSAMPLERPARAMETERIV_PROC*)glfwGetProcAddress("glGetSamplerParameteriv");
    glpfGetSamplerParameterIiv = (PFNGLGETSAMPLERPARAMETERIIV_PROC*)glfwGetProcAddress("glGetSamplerParameterIiv");
    glpfGetSamplerParameterfv = (PFNGLGETSAMPLERPARAMETERFV_PROC*)glfwGetProcAddress("glGetSamplerParameterfv");
    glpfGetSamplerParameterIuiv = (PFNGLGETSAMPLERPARAMETERIUIV_PROC*)glfwGetProcAddress("glGetSamplerParameterIuiv");
    glpfQueryCounter = (PFNGLQUERYCOUNTER_PROC*)glfwGetProcAddress("glQueryCounter");
    glpfGetQueryObjecti64v = (PFNGLGETQUERYOBJECTI64V_PROC*)glfwGetProcAddress("glGetQueryObjecti64v");
    glpfGetQueryObjectui64v = (PFNGLGETQUERYOBJECTUI64V_PROC*)glfwGetProcAddress("glGetQueryObjectui64v");
    glpfVertexAttribDivisor = (PFNGLVERTEXATTRIBDIVISOR_PROC*)glfwGetProcAddress("glVertexAttribDivisor");
    glpfVertexAttribP1ui = (PFNGLVERTEXATTRIBP1UI_PROC*)glfwGetProcAddress("glVertexAttribP1ui");
    glpfVertexAttribP1uiv = (PFNGLVERTEXATTRIBP1UIV_PROC*)glfwGetProcAddress("glVertexAttribP1uiv");
    glpfVertexAttribP2ui = (PFNGLVERTEXATTRIBP2UI_PROC*)glfwGetProcAddress("glVertexAttribP2ui");
    glpfVertexAttribP2uiv = (PFNGLVERTEXATTRIBP2UIV_PROC*)glfwGetProcAddress("glVertexAttribP2uiv");
    glpfVertexAttribP3ui = (PFNGLVERTEXATTRIBP3UI_PROC*)glfwGetProcAddress("glVertexAttribP3ui");
    glpfVertexAttribP3uiv = (PFNGLVERTEXATTRIBP3UIV_PROC*)glfwGetProcAddress("glVertexAttribP3uiv");
    glpfVertexAttribP4ui = (PFNGLVERTEXATTRIBP4UI_PROC*)glfwGetProcAddress("glVertexAttribP4ui");
    glpfVertexAttribP4uiv = (PFNGLVERTEXATTRIBP4UIV_PROC*)glfwGetProcAddress("glVertexAttribP4uiv");


    /* GL_ARB_debug_output */

    glpfDebugMessageControlARB = (PFNGLDEBUGMESSAGECONTROLARB_PROC*)glfwGetProcAddress("glDebugMessageControlARB");
    glpfDebugMessageInsertARB = (PFNGLDEBUGMESSAGEINSERTARB_PROC*)glfwGetProcAddress("glDebugMessageInsertARB");
    glpfDebugMessageCallbackARB = (PFNGLDEBUGMESSAGECALLBACKARB_PROC*)glfwGetProcAddress("glDebugMessageCallbackARB");
    glpfGetDebugMessageLogARB = (PFNGLGETDEBUGMESSAGELOGARB_PROC*)glfwGetProcAddress("glGetDebugMessageLogARB");


}

/* ----------------------- Extension flag definitions ---------------------- */
int FLEXT_ARB_debug_output = GL_FALSE;

/* ---------------------- Function pointer definitions --------------------- */

/* GL_VERSION_1_2 */

PFNGLDRAWRANGEELEMENTS_PROC* glpfDrawRangeElements = NULL;
PFNGLTEXIMAGE3D_PROC* glpfTexImage3D = NULL;
PFNGLTEXSUBIMAGE3D_PROC* glpfTexSubImage3D = NULL;
PFNGLCOPYTEXSUBIMAGE3D_PROC* glpfCopyTexSubImage3D = NULL;

/* GL_VERSION_1_3 */

PFNGLACTIVETEXTURE_PROC* glpfActiveTexture = NULL;
PFNGLSAMPLECOVERAGE_PROC* glpfSampleCoverage = NULL;
PFNGLCOMPRESSEDTEXIMAGE3D_PROC* glpfCompressedTexImage3D = NULL;
PFNGLCOMPRESSEDTEXIMAGE2D_PROC* glpfCompressedTexImage2D = NULL;
PFNGLCOMPRESSEDTEXIMAGE1D_PROC* glpfCompressedTexImage1D = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE3D_PROC* glpfCompressedTexSubImage3D = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE2D_PROC* glpfCompressedTexSubImage2D = NULL;
PFNGLCOMPRESSEDTEXSUBIMAGE1D_PROC* glpfCompressedTexSubImage1D = NULL;
PFNGLGETCOMPRESSEDTEXIMAGE_PROC* glpfGetCompressedTexImage = NULL;

/* GL_VERSION_1_4 */

PFNGLBLENDFUNCSEPARATE_PROC* glpfBlendFuncSeparate = NULL;
PFNGLMULTIDRAWARRAYS_PROC* glpfMultiDrawArrays = NULL;
PFNGLMULTIDRAWELEMENTS_PROC* glpfMultiDrawElements = NULL;
PFNGLPOINTPARAMETERF_PROC* glpfPointParameterf = NULL;
PFNGLPOINTPARAMETERFV_PROC* glpfPointParameterfv = NULL;
PFNGLPOINTPARAMETERI_PROC* glpfPointParameteri = NULL;
PFNGLPOINTPARAMETERIV_PROC* glpfPointParameteriv = NULL;
PFNGLBLENDCOLOR_PROC* glpfBlendColor = NULL;
PFNGLBLENDEQUATION_PROC* glpfBlendEquation = NULL;

/* GL_VERSION_1_5 */

PFNGLGENQUERIES_PROC* glpfGenQueries = NULL;
PFNGLDELETEQUERIES_PROC* glpfDeleteQueries = NULL;
PFNGLISQUERY_PROC* glpfIsQuery = NULL;
PFNGLBEGINQUERY_PROC* glpfBeginQuery = NULL;
PFNGLENDQUERY_PROC* glpfEndQuery = NULL;
PFNGLGETQUERYIV_PROC* glpfGetQueryiv = NULL;
PFNGLGETQUERYOBJECTIV_PROC* glpfGetQueryObjectiv = NULL;
PFNGLGETQUERYOBJECTUIV_PROC* glpfGetQueryObjectuiv = NULL;
PFNGLBINDBUFFER_PROC* glpfBindBuffer = NULL;
PFNGLDELETEBUFFERS_PROC* glpfDeleteBuffers = NULL;
PFNGLGENBUFFERS_PROC* glpfGenBuffers = NULL;
PFNGLISBUFFER_PROC* glpfIsBuffer = NULL;
PFNGLBUFFERDATA_PROC* glpfBufferData = NULL;
PFNGLBUFFERSUBDATA_PROC* glpfBufferSubData = NULL;
PFNGLGETBUFFERSUBDATA_PROC* glpfGetBufferSubData = NULL;
PFNGLMAPBUFFER_PROC* glpfMapBuffer = NULL;
PFNGLUNMAPBUFFER_PROC* glpfUnmapBuffer = NULL;
PFNGLGETBUFFERPARAMETERIV_PROC* glpfGetBufferParameteriv = NULL;
PFNGLGETBUFFERPOINTERV_PROC* glpfGetBufferPointerv = NULL;

/* GL_VERSION_2_0 */

PFNGLBLENDEQUATIONSEPARATE_PROC* glpfBlendEquationSeparate = NULL;
PFNGLDRAWBUFFERS_PROC* glpfDrawBuffers = NULL;
PFNGLSTENCILOPSEPARATE_PROC* glpfStencilOpSeparate = NULL;
PFNGLSTENCILFUNCSEPARATE_PROC* glpfStencilFuncSeparate = NULL;
PFNGLSTENCILMASKSEPARATE_PROC* glpfStencilMaskSeparate = NULL;
PFNGLATTACHSHADER_PROC* glpfAttachShader = NULL;
PFNGLBINDATTRIBLOCATION_PROC* glpfBindAttribLocation = NULL;
PFNGLCOMPILESHADER_PROC* glpfCompileShader = NULL;
PFNGLCREATEPROGRAM_PROC* glpfCreateProgram = NULL;
PFNGLCREATESHADER_PROC* glpfCreateShader = NULL;
PFNGLDELETEPROGRAM_PROC* glpfDeleteProgram = NULL;
PFNGLDELETESHADER_PROC* glpfDeleteShader = NULL;
PFNGLDETACHSHADER_PROC* glpfDetachShader = NULL;
PFNGLDISABLEVERTEXATTRIBARRAY_PROC* glpfDisableVertexAttribArray = NULL;
PFNGLENABLEVERTEXATTRIBARRAY_PROC* glpfEnableVertexAttribArray = NULL;
PFNGLGETACTIVEATTRIB_PROC* glpfGetActiveAttrib = NULL;
PFNGLGETACTIVEUNIFORM_PROC* glpfGetActiveUniform = NULL;
PFNGLGETATTACHEDSHADERS_PROC* glpfGetAttachedShaders = NULL;
PFNGLGETATTRIBLOCATION_PROC* glpfGetAttribLocation = NULL;
PFNGLGETPROGRAMIV_PROC* glpfGetProgramiv = NULL;
PFNGLGETPROGRAMINFOLOG_PROC* glpfGetProgramInfoLog = NULL;
PFNGLGETSHADERIV_PROC* glpfGetShaderiv = NULL;
PFNGLGETSHADERINFOLOG_PROC* glpfGetShaderInfoLog = NULL;
PFNGLGETSHADERSOURCE_PROC* glpfGetShaderSource = NULL;
PFNGLGETUNIFORMLOCATION_PROC* glpfGetUniformLocation = NULL;
PFNGLGETUNIFORMFV_PROC* glpfGetUniformfv = NULL;
PFNGLGETUNIFORMIV_PROC* glpfGetUniformiv = NULL;
PFNGLGETVERTEXATTRIBDV_PROC* glpfGetVertexAttribdv = NULL;
PFNGLGETVERTEXATTRIBFV_PROC* glpfGetVertexAttribfv = NULL;
PFNGLGETVERTEXATTRIBIV_PROC* glpfGetVertexAttribiv = NULL;
PFNGLGETVERTEXATTRIBPOINTERV_PROC* glpfGetVertexAttribPointerv = NULL;
PFNGLISPROGRAM_PROC* glpfIsProgram = NULL;
PFNGLISSHADER_PROC* glpfIsShader = NULL;
PFNGLLINKPROGRAM_PROC* glpfLinkProgram = NULL;
PFNGLSHADERSOURCE_PROC* glpfShaderSource = NULL;
PFNGLUSEPROGRAM_PROC* glpfUseProgram = NULL;
PFNGLUNIFORM1F_PROC* glpfUniform1f = NULL;
PFNGLUNIFORM2F_PROC* glpfUniform2f = NULL;
PFNGLUNIFORM3F_PROC* glpfUniform3f = NULL;
PFNGLUNIFORM4F_PROC* glpfUniform4f = NULL;
PFNGLUNIFORM1I_PROC* glpfUniform1i = NULL;
PFNGLUNIFORM2I_PROC* glpfUniform2i = NULL;
PFNGLUNIFORM3I_PROC* glpfUniform3i = NULL;
PFNGLUNIFORM4I_PROC* glpfUniform4i = NULL;
PFNGLUNIFORM1FV_PROC* glpfUniform1fv = NULL;
PFNGLUNIFORM2FV_PROC* glpfUniform2fv = NULL;
PFNGLUNIFORM3FV_PROC* glpfUniform3fv = NULL;
PFNGLUNIFORM4FV_PROC* glpfUniform4fv = NULL;
PFNGLUNIFORM1IV_PROC* glpfUniform1iv = NULL;
PFNGLUNIFORM2IV_PROC* glpfUniform2iv = NULL;
PFNGLUNIFORM3IV_PROC* glpfUniform3iv = NULL;
PFNGLUNIFORM4IV_PROC* glpfUniform4iv = NULL;
PFNGLUNIFORMMATRIX2FV_PROC* glpfUniformMatrix2fv = NULL;
PFNGLUNIFORMMATRIX3FV_PROC* glpfUniformMatrix3fv = NULL;
PFNGLUNIFORMMATRIX4FV_PROC* glpfUniformMatrix4fv = NULL;
PFNGLVALIDATEPROGRAM_PROC* glpfValidateProgram = NULL;
PFNGLVERTEXATTRIB1D_PROC* glpfVertexAttrib1d = NULL;
PFNGLVERTEXATTRIB1DV_PROC* glpfVertexAttrib1dv = NULL;
PFNGLVERTEXATTRIB1F_PROC* glpfVertexAttrib1f = NULL;
PFNGLVERTEXATTRIB1FV_PROC* glpfVertexAttrib1fv = NULL;
PFNGLVERTEXATTRIB1S_PROC* glpfVertexAttrib1s = NULL;
PFNGLVERTEXATTRIB1SV_PROC* glpfVertexAttrib1sv = NULL;
PFNGLVERTEXATTRIB2D_PROC* glpfVertexAttrib2d = NULL;
PFNGLVERTEXATTRIB2DV_PROC* glpfVertexAttrib2dv = NULL;
PFNGLVERTEXATTRIB2F_PROC* glpfVertexAttrib2f = NULL;
PFNGLVERTEXATTRIB2FV_PROC* glpfVertexAttrib2fv = NULL;
PFNGLVERTEXATTRIB2S_PROC* glpfVertexAttrib2s = NULL;
PFNGLVERTEXATTRIB2SV_PROC* glpfVertexAttrib2sv = NULL;
PFNGLVERTEXATTRIB3D_PROC* glpfVertexAttrib3d = NULL;
PFNGLVERTEXATTRIB3DV_PROC* glpfVertexAttrib3dv = NULL;
PFNGLVERTEXATTRIB3F_PROC* glpfVertexAttrib3f = NULL;
PFNGLVERTEXATTRIB3FV_PROC* glpfVertexAttrib3fv = NULL;
PFNGLVERTEXATTRIB3S_PROC* glpfVertexAttrib3s = NULL;
PFNGLVERTEXATTRIB3SV_PROC* glpfVertexAttrib3sv = NULL;
PFNGLVERTEXATTRIB4NBV_PROC* glpfVertexAttrib4Nbv = NULL;
PFNGLVERTEXATTRIB4NIV_PROC* glpfVertexAttrib4Niv = NULL;
PFNGLVERTEXATTRIB4NSV_PROC* glpfVertexAttrib4Nsv = NULL;
PFNGLVERTEXATTRIB4NUB_PROC* glpfVertexAttrib4Nub = NULL;
PFNGLVERTEXATTRIB4NUBV_PROC* glpfVertexAttrib4Nubv = NULL;
PFNGLVERTEXATTRIB4NUIV_PROC* glpfVertexAttrib4Nuiv = NULL;
PFNGLVERTEXATTRIB4NUSV_PROC* glpfVertexAttrib4Nusv = NULL;
PFNGLVERTEXATTRIB4BV_PROC* glpfVertexAttrib4bv = NULL;
PFNGLVERTEXATTRIB4D_PROC* glpfVertexAttrib4d = NULL;
PFNGLVERTEXATTRIB4DV_PROC* glpfVertexAttrib4dv = NULL;
PFNGLVERTEXATTRIB4F_PROC* glpfVertexAttrib4f = NULL;
PFNGLVERTEXATTRIB4FV_PROC* glpfVertexAttrib4fv = NULL;
PFNGLVERTEXATTRIB4IV_PROC* glpfVertexAttrib4iv = NULL;
PFNGLVERTEXATTRIB4S_PROC* glpfVertexAttrib4s = NULL;
PFNGLVERTEXATTRIB4SV_PROC* glpfVertexAttrib4sv = NULL;
PFNGLVERTEXATTRIB4UBV_PROC* glpfVertexAttrib4ubv = NULL;
PFNGLVERTEXATTRIB4UIV_PROC* glpfVertexAttrib4uiv = NULL;
PFNGLVERTEXATTRIB4USV_PROC* glpfVertexAttrib4usv = NULL;
PFNGLVERTEXATTRIBPOINTER_PROC* glpfVertexAttribPointer = NULL;

/* GL_VERSION_2_1 */

PFNGLUNIFORMMATRIX2X3FV_PROC* glpfUniformMatrix2x3fv = NULL;
PFNGLUNIFORMMATRIX3X2FV_PROC* glpfUniformMatrix3x2fv = NULL;
PFNGLUNIFORMMATRIX2X4FV_PROC* glpfUniformMatrix2x4fv = NULL;
PFNGLUNIFORMMATRIX4X2FV_PROC* glpfUniformMatrix4x2fv = NULL;
PFNGLUNIFORMMATRIX3X4FV_PROC* glpfUniformMatrix3x4fv = NULL;
PFNGLUNIFORMMATRIX4X3FV_PROC* glpfUniformMatrix4x3fv = NULL;

/* GL_VERSION_3_0 */

PFNGLCOLORMASKI_PROC* glpfColorMaski = NULL;
PFNGLGETBOOLEANI_V_PROC* glpfGetBooleani_v = NULL;
PFNGLGETINTEGERI_V_PROC* glpfGetIntegeri_v = NULL;
PFNGLENABLEI_PROC* glpfEnablei = NULL;
PFNGLDISABLEI_PROC* glpfDisablei = NULL;
PFNGLISENABLEDI_PROC* glpfIsEnabledi = NULL;
PFNGLBEGINTRANSFORMFEEDBACK_PROC* glpfBeginTransformFeedback = NULL;
PFNGLENDTRANSFORMFEEDBACK_PROC* glpfEndTransformFeedback = NULL;
PFNGLBINDBUFFERRANGE_PROC* glpfBindBufferRange = NULL;
PFNGLBINDBUFFERBASE_PROC* glpfBindBufferBase = NULL;
PFNGLTRANSFORMFEEDBACKVARYINGS_PROC* glpfTransformFeedbackVaryings = NULL;
PFNGLGETTRANSFORMFEEDBACKVARYING_PROC* glpfGetTransformFeedbackVarying = NULL;
PFNGLCLAMPCOLOR_PROC* glpfClampColor = NULL;
PFNGLBEGINCONDITIONALRENDER_PROC* glpfBeginConditionalRender = NULL;
PFNGLENDCONDITIONALRENDER_PROC* glpfEndConditionalRender = NULL;
PFNGLVERTEXATTRIBIPOINTER_PROC* glpfVertexAttribIPointer = NULL;
PFNGLGETVERTEXATTRIBIIV_PROC* glpfGetVertexAttribIiv = NULL;
PFNGLGETVERTEXATTRIBIUIV_PROC* glpfGetVertexAttribIuiv = NULL;
PFNGLVERTEXATTRIBI1I_PROC* glpfVertexAttribI1i = NULL;
PFNGLVERTEXATTRIBI2I_PROC* glpfVertexAttribI2i = NULL;
PFNGLVERTEXATTRIBI3I_PROC* glpfVertexAttribI3i = NULL;
PFNGLVERTEXATTRIBI4I_PROC* glpfVertexAttribI4i = NULL;
PFNGLVERTEXATTRIBI1UI_PROC* glpfVertexAttribI1ui = NULL;
PFNGLVERTEXATTRIBI2UI_PROC* glpfVertexAttribI2ui = NULL;
PFNGLVERTEXATTRIBI3UI_PROC* glpfVertexAttribI3ui = NULL;
PFNGLVERTEXATTRIBI4UI_PROC* glpfVertexAttribI4ui = NULL;
PFNGLVERTEXATTRIBI1IV_PROC* glpfVertexAttribI1iv = NULL;
PFNGLVERTEXATTRIBI2IV_PROC* glpfVertexAttribI2iv = NULL;
PFNGLVERTEXATTRIBI3IV_PROC* glpfVertexAttribI3iv = NULL;
PFNGLVERTEXATTRIBI4IV_PROC* glpfVertexAttribI4iv = NULL;
PFNGLVERTEXATTRIBI1UIV_PROC* glpfVertexAttribI1uiv = NULL;
PFNGLVERTEXATTRIBI2UIV_PROC* glpfVertexAttribI2uiv = NULL;
PFNGLVERTEXATTRIBI3UIV_PROC* glpfVertexAttribI3uiv = NULL;
PFNGLVERTEXATTRIBI4UIV_PROC* glpfVertexAttribI4uiv = NULL;
PFNGLVERTEXATTRIBI4BV_PROC* glpfVertexAttribI4bv = NULL;
PFNGLVERTEXATTRIBI4SV_PROC* glpfVertexAttribI4sv = NULL;
PFNGLVERTEXATTRIBI4UBV_PROC* glpfVertexAttribI4ubv = NULL;
PFNGLVERTEXATTRIBI4USV_PROC* glpfVertexAttribI4usv = NULL;
PFNGLGETUNIFORMUIV_PROC* glpfGetUniformuiv = NULL;
PFNGLBINDFRAGDATALOCATION_PROC* glpfBindFragDataLocation = NULL;
PFNGLGETFRAGDATALOCATION_PROC* glpfGetFragDataLocation = NULL;
PFNGLUNIFORM1UI_PROC* glpfUniform1ui = NULL;
PFNGLUNIFORM2UI_PROC* glpfUniform2ui = NULL;
PFNGLUNIFORM3UI_PROC* glpfUniform3ui = NULL;
PFNGLUNIFORM4UI_PROC* glpfUniform4ui = NULL;
PFNGLUNIFORM1UIV_PROC* glpfUniform1uiv = NULL;
PFNGLUNIFORM2UIV_PROC* glpfUniform2uiv = NULL;
PFNGLUNIFORM3UIV_PROC* glpfUniform3uiv = NULL;
PFNGLUNIFORM4UIV_PROC* glpfUniform4uiv = NULL;
PFNGLTEXPARAMETERIIV_PROC* glpfTexParameterIiv = NULL;
PFNGLTEXPARAMETERIUIV_PROC* glpfTexParameterIuiv = NULL;
PFNGLGETTEXPARAMETERIIV_PROC* glpfGetTexParameterIiv = NULL;
PFNGLGETTEXPARAMETERIUIV_PROC* glpfGetTexParameterIuiv = NULL;
PFNGLCLEARBUFFERIV_PROC* glpfClearBufferiv = NULL;
PFNGLCLEARBUFFERUIV_PROC* glpfClearBufferuiv = NULL;
PFNGLCLEARBUFFERFV_PROC* glpfClearBufferfv = NULL;
PFNGLCLEARBUFFERFI_PROC* glpfClearBufferfi = NULL;
PFNGLGETSTRINGI_PROC* glpfGetStringi = NULL;
PFNGLISRENDERBUFFER_PROC* glpfIsRenderbuffer = NULL;
PFNGLBINDRENDERBUFFER_PROC* glpfBindRenderbuffer = NULL;
PFNGLDELETERENDERBUFFERS_PROC* glpfDeleteRenderbuffers = NULL;
PFNGLGENRENDERBUFFERS_PROC* glpfGenRenderbuffers = NULL;
PFNGLRENDERBUFFERSTORAGE_PROC* glpfRenderbufferStorage = NULL;
PFNGLGETRENDERBUFFERPARAMETERIV_PROC* glpfGetRenderbufferParameteriv = NULL;
PFNGLISFRAMEBUFFER_PROC* glpfIsFramebuffer = NULL;
PFNGLBINDFRAMEBUFFER_PROC* glpfBindFramebuffer = NULL;
PFNGLDELETEFRAMEBUFFERS_PROC* glpfDeleteFramebuffers = NULL;
PFNGLGENFRAMEBUFFERS_PROC* glpfGenFramebuffers = NULL;
PFNGLCHECKFRAMEBUFFERSTATUS_PROC* glpfCheckFramebufferStatus = NULL;
PFNGLFRAMEBUFFERTEXTURE1D_PROC* glpfFramebufferTexture1D = NULL;
PFNGLFRAMEBUFFERTEXTURE2D_PROC* glpfFramebufferTexture2D = NULL;
PFNGLFRAMEBUFFERTEXTURE3D_PROC* glpfFramebufferTexture3D = NULL;
PFNGLFRAMEBUFFERRENDERBUFFER_PROC* glpfFramebufferRenderbuffer = NULL;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIV_PROC* glpfGetFramebufferAttachmentParameteriv = NULL;
PFNGLGENERATEMIPMAP_PROC* glpfGenerateMipmap = NULL;
PFNGLBLITFRAMEBUFFER_PROC* glpfBlitFramebuffer = NULL;
PFNGLRENDERBUFFERSTORAGEMULTISAMPLE_PROC* glpfRenderbufferStorageMultisample = NULL;
PFNGLFRAMEBUFFERTEXTURELAYER_PROC* glpfFramebufferTextureLayer = NULL;
PFNGLMAPBUFFERRANGE_PROC* glpfMapBufferRange = NULL;
PFNGLFLUSHMAPPEDBUFFERRANGE_PROC* glpfFlushMappedBufferRange = NULL;
PFNGLBINDVERTEXARRAY_PROC* glpfBindVertexArray = NULL;
PFNGLDELETEVERTEXARRAYS_PROC* glpfDeleteVertexArrays = NULL;
PFNGLGENVERTEXARRAYS_PROC* glpfGenVertexArrays = NULL;
PFNGLISVERTEXARRAY_PROC* glpfIsVertexArray = NULL;

/* GL_VERSION_3_1 */

PFNGLDRAWARRAYSINSTANCED_PROC* glpfDrawArraysInstanced = NULL;
PFNGLDRAWELEMENTSINSTANCED_PROC* glpfDrawElementsInstanced = NULL;
PFNGLTEXBUFFER_PROC* glpfTexBuffer = NULL;
PFNGLPRIMITIVERESTARTINDEX_PROC* glpfPrimitiveRestartIndex = NULL;
PFNGLCOPYBUFFERSUBDATA_PROC* glpfCopyBufferSubData = NULL;
PFNGLGETUNIFORMINDICES_PROC* glpfGetUniformIndices = NULL;
PFNGLGETACTIVEUNIFORMSIV_PROC* glpfGetActiveUniformsiv = NULL;
PFNGLGETACTIVEUNIFORMNAME_PROC* glpfGetActiveUniformName = NULL;
PFNGLGETUNIFORMBLOCKINDEX_PROC* glpfGetUniformBlockIndex = NULL;
PFNGLGETACTIVEUNIFORMBLOCKIV_PROC* glpfGetActiveUniformBlockiv = NULL;
PFNGLGETACTIVEUNIFORMBLOCKNAME_PROC* glpfGetActiveUniformBlockName = NULL;
PFNGLUNIFORMBLOCKBINDING_PROC* glpfUniformBlockBinding = NULL;

/* GL_VERSION_3_2 */

PFNGLDRAWELEMENTSBASEVERTEX_PROC* glpfDrawElementsBaseVertex = NULL;
PFNGLDRAWRANGEELEMENTSBASEVERTEX_PROC* glpfDrawRangeElementsBaseVertex = NULL;
PFNGLDRAWELEMENTSINSTANCEDBASEVERTEX_PROC* glpfDrawElementsInstancedBaseVertex = NULL;
PFNGLMULTIDRAWELEMENTSBASEVERTEX_PROC* glpfMultiDrawElementsBaseVertex = NULL;
PFNGLPROVOKINGVERTEX_PROC* glpfProvokingVertex = NULL;
PFNGLFENCESYNC_PROC* glpfFenceSync = NULL;
PFNGLISSYNC_PROC* glpfIsSync = NULL;
PFNGLDELETESYNC_PROC* glpfDeleteSync = NULL;
PFNGLCLIENTWAITSYNC_PROC* glpfClientWaitSync = NULL;
PFNGLWAITSYNC_PROC* glpfWaitSync = NULL;
PFNGLGETINTEGER64V_PROC* glpfGetInteger64v = NULL;
PFNGLGETSYNCIV_PROC* glpfGetSynciv = NULL;
PFNGLGETINTEGER64I_V_PROC* glpfGetInteger64i_v = NULL;
PFNGLGETBUFFERPARAMETERI64V_PROC* glpfGetBufferParameteri64v = NULL;
PFNGLFRAMEBUFFERTEXTURE_PROC* glpfFramebufferTexture = NULL;
PFNGLTEXIMAGE2DMULTISAMPLE_PROC* glpfTexImage2DMultisample = NULL;
PFNGLTEXIMAGE3DMULTISAMPLE_PROC* glpfTexImage3DMultisample = NULL;
PFNGLGETMULTISAMPLEFV_PROC* glpfGetMultisamplefv = NULL;
PFNGLSAMPLEMASKI_PROC* glpfSampleMaski = NULL;

/* GL_VERSION_3_3 */

PFNGLBINDFRAGDATALOCATIONINDEXED_PROC* glpfBindFragDataLocationIndexed = NULL;
PFNGLGETFRAGDATAINDEX_PROC* glpfGetFragDataIndex = NULL;
PFNGLGENSAMPLERS_PROC* glpfGenSamplers = NULL;
PFNGLDELETESAMPLERS_PROC* glpfDeleteSamplers = NULL;
PFNGLISSAMPLER_PROC* glpfIsSampler = NULL;
PFNGLBINDSAMPLER_PROC* glpfBindSampler = NULL;
PFNGLSAMPLERPARAMETERI_PROC* glpfSamplerParameteri = NULL;
PFNGLSAMPLERPARAMETERIV_PROC* glpfSamplerParameteriv = NULL;
PFNGLSAMPLERPARAMETERF_PROC* glpfSamplerParameterf = NULL;
PFNGLSAMPLERPARAMETERFV_PROC* glpfSamplerParameterfv = NULL;
PFNGLSAMPLERPARAMETERIIV_PROC* glpfSamplerParameterIiv = NULL;
PFNGLSAMPLERPARAMETERIUIV_PROC* glpfSamplerParameterIuiv = NULL;
PFNGLGETSAMPLERPARAMETERIV_PROC* glpfGetSamplerParameteriv = NULL;
PFNGLGETSAMPLERPARAMETERIIV_PROC* glpfGetSamplerParameterIiv = NULL;
PFNGLGETSAMPLERPARAMETERFV_PROC* glpfGetSamplerParameterfv = NULL;
PFNGLGETSAMPLERPARAMETERIUIV_PROC* glpfGetSamplerParameterIuiv = NULL;
PFNGLQUERYCOUNTER_PROC* glpfQueryCounter = NULL;
PFNGLGETQUERYOBJECTI64V_PROC* glpfGetQueryObjecti64v = NULL;
PFNGLGETQUERYOBJECTUI64V_PROC* glpfGetQueryObjectui64v = NULL;
PFNGLVERTEXATTRIBDIVISOR_PROC* glpfVertexAttribDivisor = NULL;
PFNGLVERTEXATTRIBP1UI_PROC* glpfVertexAttribP1ui = NULL;
PFNGLVERTEXATTRIBP1UIV_PROC* glpfVertexAttribP1uiv = NULL;
PFNGLVERTEXATTRIBP2UI_PROC* glpfVertexAttribP2ui = NULL;
PFNGLVERTEXATTRIBP2UIV_PROC* glpfVertexAttribP2uiv = NULL;
PFNGLVERTEXATTRIBP3UI_PROC* glpfVertexAttribP3ui = NULL;
PFNGLVERTEXATTRIBP3UIV_PROC* glpfVertexAttribP3uiv = NULL;
PFNGLVERTEXATTRIBP4UI_PROC* glpfVertexAttribP4ui = NULL;
PFNGLVERTEXATTRIBP4UIV_PROC* glpfVertexAttribP4uiv = NULL;

/* GL_ARB_debug_output */

PFNGLDEBUGMESSAGECONTROLARB_PROC* glpfDebugMessageControlARB = NULL;
PFNGLDEBUGMESSAGEINSERTARB_PROC* glpfDebugMessageInsertARB = NULL;
PFNGLDEBUGMESSAGECALLBACKARB_PROC* glpfDebugMessageCallbackARB = NULL;
PFNGLGETDEBUGMESSAGELOGARB_PROC* glpfGetDebugMessageLogARB = NULL;



#ifdef __cplusplus
}
#endif
