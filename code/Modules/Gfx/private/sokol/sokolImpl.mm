//------------------------------------------------------------------------------
//  sokolImpl.mm
//------------------------------------------------------------------------------
#include "Core/Assertion.h"
#include "Core/Memory/Memory.h"
#include "Core/Log.h"
#define SOKOL_IMPL
#define SOKOL_ASSERT(c) o_assert_dbg(c)
#define SOKOL_MALLOC(s) Oryol::Memory::Alloc(s)
#define SOKOL_FREE(p) Oryol::Memory::Free(p)
#define SOKOL_LOG(m) Oryol::Log::Info("%s\n",m)
#define SOKOL_UNREACHABLE o_assert_dbg(false)
#if !ORYOL_METAL
#error "please include sokolImpl.cc for non-Metal builds"
#endif
#if ORYOL_MACOS
#define SOKOL_METAL_MACOS
#elif ORYOL_IOS
#define SOKOL_METAL_IOS
#else
#error "unknown Metal platform"
#endif
#include "sokol_gfx.h"
