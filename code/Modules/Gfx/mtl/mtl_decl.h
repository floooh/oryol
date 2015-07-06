#pragma once
//------------------------------------------------------------------------------
/**
    @file mtl_decl.h
    @brief Metal Objective-C++ declarations
*/
#if defined(__OBJC__)
#import <Cocoa/Cocoa.h>
#import <Metal/Metal.h>
#import <QuartzCore/QuartzCore.h>
#define ORYOL_OBJC_TYPED_ID(clazz) id<clazz>
#define ORYOL_OBJC_ID id
static_assert(sizeof(id) == sizeof(void*), "Obj-C id size doesn't match void* size");
#else
#define ORYOL_OBJC_TYPED_ID(clazz) void*
#define ORYOL_OBJC_ID void*
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
#endif

#if ORYOL_USE_ARC
#define ORYOL_OBJC_RELEASE(obj)
#else
#define ORYOL_OBJC_RELEASE(obj) [obj release]
#endif