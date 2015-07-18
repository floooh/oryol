#pragma once
//------------------------------------------------------------------------------
/**
    @file mtl_decl.h
    @brief Metal Objective-C++ declarations
*/
#include "Core/osx/osxAppBridge.h"
#if !defined(__OBJC__)
typedef void* CAMetalLayer;
typedef void* CAMetalDrawable;
typedef void* MTLDevice;
typedef void* MTLCommandQueue;
typedef void* MTLRenderPassDescriptor;
typedef void* MTLRenderCommandEncoder;
typedef void* MTLTexture;
typedef void* MTLBuffer;
typedef void* MTLLibrary;
typedef void* MTLFunction;
typedef void* MTLRenderPipelineState;
typedef void* MTLTexture;
typedef void* MTLSamplerState;
#endif
