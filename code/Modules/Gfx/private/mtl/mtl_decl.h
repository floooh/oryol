#pragma once
//------------------------------------------------------------------------------
/**
    @file mtl_decl.h
    @brief Metal Objective-C++ declarations
*/
#if ORYOL_IOS
#include "Core/private/ios/iosBridge.h"
namespace Oryol {
namespace _priv {
typedef iosBridge osBridge;
} }
#else
#include "Core/private/osx/osxBridge.h"
namespace Oryol {
namespace _priv {
typedef osxBridge osBridge;
} }
#endif
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
